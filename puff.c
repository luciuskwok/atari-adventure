// puff.c

#include "puff.h"
#include <setjmp.h>             /* for setjmp(), longjmp(), and jmp_buf */


// Function Prototypes
extern SInt16 __fastcall__ codes_asm(const struct huffman *lencode, const struct huffman *distcode);
extern UInt16 __fastcall__ decode_asm(const struct huffman *h);
extern UInt16 __fastcall__ bits_asm(UInt8 count);
extern UInt16 __fastcall__ get_one_bit(void);


// Define for including old code that has asm replacements
//#define OLD_CODE

/*
 * Maximums for allocations and loops.  It is not useful to change these --
 * they are fixed by the deflate format.
 */
#define MAXBITS 15              /* maximum bits in a code */
#define MAXLCODES 286           /* maximum number of literal/length codes */
#define MAXDCODES 30            /* maximum number of distance codes */
#define MAXCODES (MAXLCODES+MAXDCODES)  /* maximum codes lengths to read */
#define FIXLCODES 288           /* number of fixed literal/length codes */


/* input and output state */
typedef struct PuffState {
    /* output state */
    UInt8 *out;         /* output buffer */
    UInt16 outlen;      /* available space at out */
    UInt16 outcnt;      /* bytes written to out so far */

    /* input state */
    const UInt8 *inPtr;     /* pointer to next input byte */
    const UInt8 *inEndPtr;  /* pointer at end of input */
    UInt8 bitbuf;           /* bit buffer */
    UInt8 bitcnt;           /* number of bits in bit buffer */

    /* input limit error return state for bits() and decode() */
    jmp_buf env;
} PuffState;

PuffState *puffState = (PuffState *)0x9C; // TODO: fix this
//extern PuffState *puffState; // this version crashes because of the wrong value
#pragma zpsym("puffState");

/*
 * Huffman code decoding tables.  count[1..MAXBITS] is the number of symbols of
 * each length, which for a canonical code are stepped through in order.
 * symbol[] are the symbol values in canonical order, where the number of
 * entries is the sum of the counts in count[].  The decoding process can be
 * seen in the function decode() below.
 */
struct huffman {
    UInt8 *count;       /* number of symbols of each length */
    UInt16 *symbol;      /* canonically ordered symbols */
};


/*
 * Decode a code from the stream s using huffman table h.  Return the symbol or
 * a negative value if there is an error.  If all of the lengths are zero, i.e.
 * an empty code, or if the code is incomplete and an invalid code is received,
 * then -10 is returned after reading MAXBITS bits.
 *
 * Format notes:
 *
 * - The codes as stored in the compressed data are bit-reversed relative to
 *   a simple integer ordering of codes of the same lengths.  Hence below the
 *   bits are pulled from the compressed data one at a time and used to
 *   build the code value reversed from what is in the stream in order to
 *   permit simple integer comparisons for decoding.  A table-based decoding
 *   scheme (as used in zlib) does not need to do this reversal.
 *
 * - The first code for the shortest length is all zeros.  Subsequent codes of
 *   the same length are simply integer increments of the previous code.  When
 *   moving up a length, a zero bit is appended to the code.  For a complete
 *   code, the last code of the longest length will be all ones.
 *
 * - Incomplete codes are handled by this decoder, since they are permitted
 *   in the deflate format.  See the format notes for fixed() and dynamic().
 */

// decode() was replaced by decode_asm()


/*
 * Given the list of code lengths length[0..n-1] representing a canonical
 * Huffman code for n symbols, construct the tables required to decode those
 * codes.  Those tables are the number of codes of each length, and the symbols
 * sorted by length, retaining their original order within each length.  The
 * return value is zero for a complete code set, negative for an over-
 * subscribed code set, and positive for an incomplete code set.  The tables
 * can be used if the return value is zero or positive, but they cannot be used
 * if the return value is negative.  If the return value is zero, it is not
 * possible for decode() using that table to return an error--any stream of
 * enough bits will resolve to a symbol.  If the return value is positive, then
 * it is possible for decode() using that table to return an error for received
 * codes past the end of the incomplete lengths.
 *
 * Not used by decode(), but used for error checking, h->count[0] is the number
 * of the n symbols not in the code.  So n - h->count[0] is the number of
 * codes.  This is useful for checking for incomplete codes that have more than
 * one symbol, which is an error in a dynamic block.
 *
 * Assumption: for all i in 0..n-1, 0 <= length[i] <= MAXBITS
 * This is assured by the construction of the length arrays in dynamic() and
 * fixed() and is not verified by construct().
 *
 * Format notes:
 *
 * - Permitted and expected examples of incomplete codes are one of the fixed
 *   codes and any code with a single symbol which in deflate is coded as one
 *   bit instead of zero bits.  See the format notes for fixed() and dynamic().
 *
 * - Within a given code length, the symbols are kept in ascending order for
 *   the code bits definition.
 */
int construct(struct huffman *h, const UInt8 *length, int n)
{
    UInt16 symbol;         /* current symbol when stepping through length[] */
    UInt8 len;            /* current length when stepping through h->count[] */
    SInt16 left;           /* number of possible codes left of current length */
    SInt16 offs[MAXBITS+1];      /* offsets in symbol table for each length */

    /* count number of codes of each length */
    for (len = 0; len <= MAXBITS; ++len)
        h->count[len] = 0;
    for (symbol = 0; symbol < n; ++symbol)
        ++(h->count[length[symbol]]);   /* assumes lengths are within bounds */
    if (h->count[0] == n)               /* no codes! */
        return 0;                       /* complete, but decode() will fail */

    /* check for an over-subscribed or incomplete set of lengths */
    left = 1;                           /* one possible code of zero length */
    for (len = 1; len <= MAXBITS; ++len) {
        left <<= 1;                     /* one more bit, double codes left */
        left -= h->count[len];          /* deduct count from possible codes */
        if (left < 0)
            return left;                /* over-subscribed--return negative */
    }                                   /* left > 0 means incomplete */

    /* generate offsets into symbol table for each length for sorting */
    offs[1] = 0;
    for (len = 1; len < MAXBITS; ++len)
        offs[len + 1] = offs[len] + h->count[len];

    /*
     * put symbols in table sorted by length, by symbol order within each
     * length
     */
    for (symbol = 0; symbol < n; ++symbol)
        if (length[symbol] != 0)
            h->symbol[offs[length[symbol]]++] = symbol;

    /* return zero for complete set, positive for incomplete set */
    return left;
}


/*
 * Decode literal/length and distance codes until an end-of-block code.
 *
 * Format notes:
 *
 * - Compressed data that is after the block type if fixed or after the code
 *   description if dynamic is a combination of literals and length/distance
 *   pairs terminated by and end-of-block code.  Literals are simply Huffman
 *   coded bytes.  A length/distance pair is a coded length followed by a
 *   coded distance to represent a string that occurs earlier in the
 *   uncompressed data that occurs again at the current location.
 *
 * - Literals, lengths, and the end-of-block code are combined into a single
 *   code of up to 286 symbols.  They are 256 literals (0..255), 29 length
 *   symbols (257..285), and the end-of-block symbol (256).
 *
 * - There are 256 possible lengths (3..258), and so 29 symbols are not enough
 *   to represent all of those.  Lengths 3..10 and 258 are in fact represented
 *   by just a length symbol.  Lengths 11..257 are represented as a symbol and
 *   some number of extra bits that are added as an integer to the base length
 *   of the length symbol.  The number of extra bits is determined by the base
 *   length symbol.  These are in the static arrays below, lens[] for the base
 *   lengths and lext[] for the corresponding number of extra bits.
 *
 * - The reason that 258 gets its own symbol is that the longest length is used
 *   often in highly redundant files.  Note that 258 can also be coded as the
 *   base value 227 plus the maximum extra value of 31.  While a good deflate
 *   should never do this, it is not an error, and should be decoded properly.
 *
 * - If a length is decoded, including its extra bits if any, then it is
 *   followed a distance code.  There are up to 30 distance symbols.  Again
 *   there are many more possible distances (1..32768), so extra bits are added
 *   to a base value represented by the symbol.  The distances 1..4 get their
 *   own symbol, but the rest require extra bits.  The base distances and
 *   corresponding number of extra bits are below in the static arrays dist[]
 *   and dext[].
 *
 * - Literal bytes are simply written to the output.  A length/distance pair is
 *   an instruction to copy previously uncompressed bytes to the output.  The
 *   copy is from distance bytes back in the output stream, copying for length
 *   bytes.
 *
 * - Distances pointing before the beginning of the output data are not
 *   permitted.
 *
 * - Overlapped copies, where the length is greater than the distance, are
 *   allowed and common.  For example, a distance of one and a length of 258
 *   simply copies the last byte 258 times.  A distance of four and a length of
 *   twelve copies the last four bytes three times.  A simple forward copy
 *   ignoring whether the length is greater than the distance or not implements
 *   this correctly.  You should not use memcpy() since its behavior is not
 *   defined for overlapped arrays.  You should not use memmove() or bcopy()
 *   since though their behavior -is- defined for overlapping arrays, it is
 *   defined to do the wrong thing in this case.
 */
#ifdef OLD_CODE
SInt16 codes_old(const struct huffman *lencode,
                const struct huffman *distcode)
{
    static const UInt16 lens[29] = { /* Size base for length codes 257..285 */
        3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31,
        35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258};
    static const UInt8 lext[29] = { /* Extra bits for length codes 257..285 */
        0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2,
        3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0};
    static const UInt16 dists[30] = { /* Offset base for distance codes 0..29 */
        1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193,
        257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145,
        8193, 12289, 16385, 24577};
    static const UInt8 dext[30] = { /* Extra bits for distance codes 0..29 */
        0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6,
        7, 7, 8, 8, 9, 9, 10, 10, 11, 11,
        12, 12, 13, 13};
 	UInt16 symbol;   	/* decoded symbol */
    UInt16 len;      	/* length for copy */
    UInt16 dist;    	/* distance for copy */
 
    /* decode literals and length/distance pairs */
    do {
        symbol = decode_asm(lencode);
        if (symbol > MAXLCODES)
            return -10;              /* invalid symbol */
        if (symbol < 256) {             /* literal: symbol is the byte */
            /* write out the literal */
            if (puffState->out != NULL) {
                if (puffState->outcnt == puffState->outlen)
                    return 1;
                puffState->out[puffState->outcnt] = symbol;
            }
            ++puffState->outcnt;
        }
        else if (symbol > 256) {        /* length */
            /* get and compute length */
        	symbol -= 257;
            len = lens[symbol] + bits_asm(lext[symbol]);

            /* get and check distance */
            symbol = decode_asm(distcode);
            if (symbol >= MAXDCODES)
                return -10;          /* invalid symbol */
            dist = dists[symbol] + bits_asm(dext[symbol]);
            if (dist > puffState->outcnt)
                return -11;     /* distance too far back */

            /* copy length bytes from distance bytes back */
            if (puffState->out != NULL) {
                if (puffState->outcnt + len > puffState->outlen)
                    return 1;
                while (len--) {
                    puffState->out[puffState->outcnt] = puffState->out[puffState->outcnt - dist];
                    ++puffState->outcnt;
                }
            }
            else
                puffState->outcnt += len;
        }
    } while (symbol != 256);            /* end of block symbol */

    /* done with a valid fixed or dynamic block */
    return 0;
}
#endif /* OLD_CODE */


/*
 * Process a fixed codes block.
 *
 * Format notes:
 *
 * - This block type can be useful for compressing small amounts of data for
 *   which the size of the code descriptions in a dynamic block exceeds the
 *   benefit of custom codes for that block.  For fixed codes, no bits are
 *   spent on code descriptions.  Instead the code lengths for literal/length
 *   codes and distance codes are fixed.  The specific lengths for each symbol
 *   can be seen in the "for" loops below.
 *
 * - The literal/length code is complete, but has two symbols that are invalid
 *   and should result in an error if received.  This cannot be implemented
 *   simply as an incomplete code since those two symbols are in the "middle"
 *   of the code.  They are eight bits long and the longest literal/length\
 *   code is nine bits.  Therefore the code must be constructed with those
 *   symbols, and the invalid symbols must be detected after decoding.
 *
 * - The fixed distance codes also have two invalid symbols that should result
 *   in an error if received.  Since all of the distance codes are the same
 *   length, this can be implemented as an incomplete code.  Then the invalid
 *   codes are detected while decoding.
 */

UInt8  fixed_lencnt[MAXBITS+1] = { 
	0, 0, 0, 0, 0, 0, 0, 0x18, 0x98, 0x70, 0, 0, 0, 0, 0, 0 
};
UInt16 fixed_lensym[FIXLCODES] = {
    0x0100, 0x0101, 0x0102, 0x0103, 0x0104, 0x0105, 0x0106, 0x0107,
    0x0108, 0x0109, 0x010A, 0x010B, 0x010C, 0x010D, 0x010E, 0x010F,
    0x0110, 0x0111, 0x0112, 0x0113, 0x0114, 0x0115, 0x0116, 0x0117,
    0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007,
    0x0008, 0x0009, 0x000A, 0x000B, 0x000C, 0x000D, 0x000E, 0x000F,
    0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017,
    0x0018, 0x0019, 0x001A, 0x001B, 0x001C, 0x001D, 0x001E, 0x001F,
    0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027,
    0x0028, 0x0029, 0x002A, 0x002B, 0x002C, 0x002D, 0x002E, 0x002F,
    0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037,
    0x0038, 0x0039, 0x003A, 0x003B, 0x003C, 0x003D, 0x003E, 0x003F,
    0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047,
    0x0048, 0x0049, 0x004A, 0x004B, 0x004C, 0x004D, 0x004E, 0x004F,
    0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057,
    0x0058, 0x0059, 0x005A, 0x005B, 0x005C, 0x005D, 0x005E, 0x005F,
    0x0060, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067,
    0x0068, 0x0069, 0x006A, 0x006B, 0x006C, 0x006D, 0x006E, 0x006F,
    0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077,
    0x0078, 0x0079, 0x007A, 0x007B, 0x007C, 0x007D, 0x007E, 0x007F,
    0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087,
    0x0088, 0x0089, 0x008A, 0x008B, 0x008C, 0x008D, 0x008E, 0x008F,
    0x0118, 0x0119, 0x011A, 0x011B, 0x011C, 0x011D, 0x011E, 0x011F,
    0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097,
    0x0098, 0x0099, 0x009A, 0x009B, 0x009C, 0x009D, 0x009E, 0x009F,
    0x00A0, 0x00A1, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7,
    0x00A8, 0x00A9, 0x00AA, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00AF,
    0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x00B6, 0x00B7,
    0x00B8, 0x00B9, 0x00BA, 0x00BB, 0x00BC, 0x00BD, 0x00BE, 0x00BF,
    0x00C0, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x00C7,
    0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF,
    0x00D0, 0x00D1, 0x00D2, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x00D7,
    0x00D8, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x00DD, 0x00DE, 0x00DF,
    0x00E0, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x00E7,
    0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF,
    0x00F0, 0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x00F7,
    0x00F8, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x00FD, 0x00FE, 0x00FF
};
UInt8  fixed_distcnt[MAXBITS+1] = { 
	0, 0, 0, 0, 0, 0x1E, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 
};
 UInt16 fixed_distsym[MAXDCODES] = {
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 
	16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29  
};


SInt16 fixed(void)
{
   	struct huffman l, d;

	l.count = fixed_lencnt;
	l.symbol = fixed_lensym;
	d.count = fixed_distcnt;
	d.symbol = fixed_distsym;

    /* decode data until end-of-block code */
    return codes_asm(&l, &d);
}

/*
 * Verify that the huffman tables used for a fixed codes block is correct.
 */
//#define VERIFY_FIXED
#ifdef VERIFY_FIXED
const UInt8 fixed_lengths[FIXLCODES] = {
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
    9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
    9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
    9,9,9,9,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,8,8,8,8,8,8,8,8
};
const UInt8 fixed_dist_lengths[MAXDCODES] = { 5,5,5,5,5, 5,5,5,5,5, 5,5,5,5,5, 5,5,5,5,5, 5,5,5,5,5, 5,5,5,5,5 };
UInt8  fixed_lencnt_verify[MAXBITS+1];
UInt16 fixed_lensym_verify[FIXLCODES];
UInt8  fixed_distcnt_verify[MAXBITS+1];
UInt16 fixed_distsym_verify[MAXDCODES];

int verify_fixed_tables(void) 
{
	UInt16 symbol;
	struct huffman vl, vd;
	int i;

	for (symbol = 0; symbol < 144; ++symbol) {
		if (fixed_lengths[symbol] != 8) return -1;
	}
	for (; symbol < 256; ++symbol) {
		if (fixed_lengths[symbol] != 9) return -2;
	}
	for (; symbol < 280; ++symbol) {
		if (fixed_lengths[symbol] != 7) return -3;
	}
	for (; symbol < FIXLCODES; ++symbol) {
		if (fixed_lengths[symbol] != 8) return -4;
	}

	// Verify lencode table
	vl.count = fixed_lencnt_verify;
	vl.symbol = fixed_lensym_verify;
	construct(&vl, fixed_lengths, FIXLCODES);
	for (i=0; i<MAXBITS+1; ++i) {
		if (fixed_lencnt_verify[i] != fixed_lencnt[i]) return -5;
	}
	for (i=0; i<FIXLCODES; ++i) {
		if (fixed_lensym_verify[i] != fixed_lensym[i]) return -6;
	}

	vd.count = fixed_distcnt_verify;
	vd.symbol = fixed_distsym_verify;
	construct(&vd, fixed_dist_lengths, MAXDCODES);
	for (i=0; i<MAXBITS+1; ++i) {
		if (fixed_distcnt_verify[i] != fixed_distcnt[i]) return -7;
	}
	for (i=0; i<MAXDCODES; ++i) {
		if (fixed_distsym_verify[i] != fixed_distsym[i]) return -8;
	}


	return 0;
}
#endif /* VERIFY_FIXED */


/*
 * Process a dynamic codes block.
 *
 * Format notes:
 *
 * - A dynamic block starts with a description of the literal/length and
 *   distance codes for that block.  New dynamic blocks allow the compressor to
 *   rapidly adapt to changing data with new codes optimized for that data.
 *
 * - The codes used by the deflate format are "canonical", which means that
 *   the actual bits of the codes are generated in an unambiguous way simply
 *   from the number of bits in each code.  Therefore the code descriptions
 *   are simply a list of code lengths for each symbol.
 *
 * - The code lengths are stored in order for the symbols, so lengths are
 *   provided for each of the literal/length symbols, and for each of the
 *   distance symbols.
 *
 * - If a symbol is not used in the block, this is represented by a zero as
 *   as the code length.  This does not mean a zero-length code, but rather
 *   that no code should be created for this symbol.  There is no way in the
 *   deflate format to represent a zero-length code.
 *
 * - The maximum number of bits in a code is 15, so the possible lengths for
 *   any code are 1..15.
 *
 * - The fact that a length of zero is not permitted for a code has an
 *   interesting consequence.  Normally if only one symbol is used for a given
 *   code, then in fact that code could be represented with zero bits.  However
 *   in deflate, that code has to be at least one bit.  So for example, if
 *   only a single distance base symbol appears in a block, then it will be
 *   represented by a single code of length one, in particular one 0 bit.  This
 *   is an incomplete code, since if a 1 bit is received, it has no meaning,
 *   and should result in an error.  So incomplete distance codes of one symbol
 *   should be permitted, and the receipt of invalid codes should be handled.
 *
 * - It is also possible to have a single literal/length code, but that code
 *   must be the end-of-block code, since every dynamic block has one.  This
 *   is not the most efficient way to create an empty block (an empty fixed
 *   block is fewer bits), but it is allowed by the format.  So incomplete
 *   literal/length codes of one symbol should also be permitted.
 *
 * - If there are only literal codes and no lengths, then there are no distance
 *   codes.  This is represented by one distance code with zero bits.
 *
 * - The list of up to 286 length/literal lengths and up to 30 distance lengths
 *   are themselves compressed using Huffman codes and run-length encoding.  In
 *   the list of code lengths, a 0 symbol means no code, a 1..15 symbol means
 *   that length, and the symbols 16, 17, and 18 are run-length instructions.
 *   Each of 16, 17, and 18 are follwed by extra bits to define the length of
 *   the run.  16 copies the last length 3 to 6 times.  17 represents 3 to 10
 *   zero lengths, and 18 represents 11 to 138 zero lengths.  Unused symbols
 *   are common, hence the special coding for zero lengths.
 *
 * - The symbols for 0..18 are Huffman coded, and so that code must be
 *   described first.  This is simply a sequence of up to 19 three-bit values
 *   representing no code (0) or the code length for that symbol (1..7).
 *
 * - A dynamic block starts with three fixed-size counts from which is computed
 *   the number of literal/length code lengths, the number of distance code
 *   lengths, and the number of code length code lengths (ok, you come up with
 *   a better name!) in the code descriptions.  For the literal/length and
 *   distance codes, lengths after those provided are considered zero, i.e. no
 *   code.  The code length code lengths are received in a permuted order (see
 *   the order[] array below) to make a short code length code length list more
 *   likely.  As it turns out, very short and very long codes are less likely
 *   to be seen in a dynamic code description, hence what may appear initially
 *   to be a peculiar ordering.
 *
 * - Given the number of literal/length code lengths (nlen) and distance code
 *   lengths (ndist), then they are treated as one long list of nlen + ndist
 *   code lengths.  Therefore run-length coding can and often does cross the
 *   boundary between the two sets of lengths.
 *
 * - So to summarize, the code description at the start of a dynamic block is
 *   three counts for the number of code lengths for the literal/length codes,
 *   the distance codes, and the code length codes.  This is followed by the
 *   code length code lengths, three bits each.  This is used to construct the
 *   code length code which is used to read the remainder of the lengths.  Then
 *   the literal/length code lengths and distance lengths are read as a single
 *   set of lengths using the code length codes.  Codes are constructed from
 *   the resulting two sets of lengths, and then finally you can start
 *   decoding actual compressed data in the block.
 *
 * - For reference, a "typical" size for the code description in a dynamic
 *   block is around 80 bytes.
 */
UInt8  dynamic_lengths[MAXCODES];        /* descriptor code lengths */
UInt16 dynamic_lensym[MAXLCODES];        /* lencode memory */
UInt16 dynamic_distsym[MAXDCODES];       /* distcode memory */
SInt16 dynamic(void)
{
    int nlen, ndist, ncode;             /* number of lengths in descriptor */
    int index;                          /* index of lengths[] */
    int err;                            /* construct() return value */
    UInt8 lencnt[MAXBITS+1];         /* lencode memory */
    UInt8 distcnt[MAXBITS+1];       /* distcode memory */
    struct huffman lencode, distcode;   /* length and distance codes */
    static const UInt8 order[19] =      /* permutation of code length codes */
        {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};

    //profiling_checkpoint[0] = SHORT_CLOCK;

    /* construct lencode and distcode */
    lencode.count = lencnt;
    lencode.symbol = dynamic_lensym;
    distcode.count = distcnt;
    distcode.symbol = dynamic_distsym;

    /* get number of lengths in each table, check lengths */
    nlen = bits_asm(5) + 257;
    ndist = bits_asm(5) + 1;
    ncode = bits_asm(4) + 4;
    if (nlen > MAXLCODES || ndist > MAXDCODES)
        return -3;                      /* bad counts */

    /* read code length code lengths (really), missing lengths are zero */
    for (index = 0; index < ncode; index++)
        dynamic_lengths[order[index]] = bits_asm(3);
    for (; index < 19; index++)
        dynamic_lengths[order[index]] = 0;

    /* build huffman table for code lengths codes (use lencode temporarily) */
    err = construct(&lencode, dynamic_lengths, 19);
    if (err != 0)               /* require complete code set here */
        return -4;

    /* read length/literal and distance code length tables */
    index = 0;
    while (index < nlen + ndist) {
        UInt16 symbol;            /* decoded value */
        UInt8 len;                /* last length to repeat */

        symbol = decode_asm(&lencode);

        // Debugging
        // if (debugging_index < debugging_length) {
        // 	debugging[debugging_index++] = symbol;
        // }

        if (symbol > MAXCODES)
            return symbol;          /* invalid symbol */
        if (symbol < 16)                /* length in 0..15 */
            dynamic_lengths[index++] = symbol;
        else {                          /* repeat instruction */
            len = 0;                    /* assume repeating zeros */
            if (symbol == 16) {         /* repeat last length 3..6 times */
                if (index == 0)
                    return -5;          /* no last length! */
                len = dynamic_lengths[index - 1];       /* last length */
                symbol = 3 + bits_asm(2);
            }
            else if (symbol == 17)      /* repeat zero 3..10 times */
                symbol = 3 + bits_asm(3);
            else                        /* == 18, repeat zero 11..138 times */
                symbol = 11 + bits_asm(7);
            if (index + symbol > nlen + ndist)
                return -6;              /* too many lengths! */
            while (symbol--)            /* repeat last or zero symbol times */
                dynamic_lengths[index++] = len;
        }
    }

    /* check for end-of-block code -- there better be one! */
    if (dynamic_lengths[256] == 0)
        return -9;

    // profiling_checkpoint[2] = SHORT_CLOCK;

    /* build huffman table for literal/length codes */
    err = construct(&lencode, dynamic_lengths, nlen);
    if (err && (err < 0 || nlen != lencode.count[0] + lencode.count[1]))
        return -7;      /* incomplete code ok only for single length 1 code */

    /* build huffman table for distance codes */
    err = construct(&distcode, dynamic_lengths + nlen, ndist);
    if (err && (err < 0 || ndist != distcode.count[0] + distcode.count[1]))
        return -8;      /* incomplete code ok only for single length 1 code */

    // Debugging
	// return 1;

    /* decode data until end-of-block code */
    return codes_asm(&lencode, &distcode);
}


/*
 * Process a stored block.
 *
 * Format notes:
 *
 * - After the two-bit stored block type (00), the stored block length and
 *   stored bytes are byte-aligned for fast copying.  Therefore any leftover
 *   bits in the byte that has the last bit of the type, as many as seven, are
 *   discarded.  The value of the discarded bits are not defined and should not
 *   be checked against any expectation.
 *
 * - The second inverted copy of the stored block length does not have to be
 *   checked, but it's probably a good idea to do so anyway.
 *
 * - A stored block can have zero length.  This is sometimes used to byte-align
 *   subsets of the compressed data for random access or partial recovery.
 */
SInt16 stored(void)
{
    UInt16 len;       /* length of stored block */

    /* discard leftover bits from current byte (assumes puffState->bitcnt < 8) */
    puffState->bitbuf = 0;
    puffState->bitcnt = 0;

    /* get length and check against its one's complement */
    if (puffState->inPtr + 4 > puffState->inEndPtr)
        return 2;                               /* not enough input */
    len = *puffState->inPtr++;
    len |= *puffState->inPtr++ << 8;
    if (*puffState->inPtr++ != (~len & 0xff) ||
        *puffState->inPtr++ != ((~len >> 8) & 0xff))
        return -2;                              /* didn't match complement! */

    /* copy len bytes from in to out */
    if (puffState->inPtr + len > puffState->inEndPtr)
        return 2;                               /* not enough input */
    if (puffState->outcnt + len > puffState->outlen)
        return 1;                           /* not enough output space */
    while (len--)
        puffState->out[puffState->outcnt++] =  *puffState->inPtr++;


    /* done with a valid stored block */
    return 0;
}


/*
 * Inflate source to dest.  On return, destlen and sourcelen are updated to the
 * size of the uncompressed data and the size of the deflate data respectively.
 * On success, the return value of puff() is zero.  If there is an error in the
 * source data, i.e. it is not in the deflate format, then a negative value is
 * returned.  If there is not enough input available or there is not enough
 * output space, then a positive error is returned.  In that case, destlen and
 * sourcelen are not updated to facilitate retrying from the beginning with the
 * provision of more input data or more output space.  In the case of invalid
 * inflate data (a negative error), the dest and source pointers are updated to
 * facilitate the debugging of deflators.
 *
 * The return codes are:
 *
 *   2:  available inflate data did not terminate
 *   1:  output space exhausted before completing inflate
 *   0:  successful inflate
 *  -1:  invalid block type (type == 3)
 *  -2:  stored block length did not match one's complement
 *  -3:  dynamic block code description: too many length or distance codes
 *  -4:  dynamic block code description: code lengths codes incomplete
 *  -5:  dynamic block code description: repeat lengths with no first length
 *  -6:  dynamic block code description: repeat more than specified lengths
 *  -7:  dynamic block code description: invalid literal/length code lengths
 *  -8:  dynamic block code description: invalid distance code lengths
 *  -9:  dynamic block code description: missing end-of-block code
 * -10:  invalid literal/length or distance code in fixed or dynamic block
 * -11:  distance is too far back in fixed or dynamic block
 *
 * Format notes:
 *
 * - Three bits are read for each block to determine the kind of block and
 *   whether or not it is the last block.  Then the block is decoded and the
 *   process repeated if it was not the last block.
 *
 * - The leftover bits in the last byte of the deflate data after the last
 *   block (if it was a fixed or dynamic block) are undefined and have no
 *   expected values to check.
 */
SInt16 puff(UInt8 *dest, UInt16 destLen, const UInt8 *source, UInt16 sourceLen) {
    UInt8 last, type;             /* block information */
    SInt16 err;                    /* return value */

    if (dest == NULL) {
    	return 1;
    }

    /* initialize output state */
    puffState->out = dest;
    puffState->outlen = destLen;
    puffState->outcnt = 0;

    /* initialize input state */
    puffState->inPtr = source;
    puffState->inEndPtr = source + sourceLen;
    puffState->bitbuf = 0;
    puffState->bitcnt = 0;

    /* return if bits() or decode() tries to read past available input */
    if (setjmp(puffState->env) != 0) {           /* if came back here via longjmp() */
        err = 2;                        /* then skip do-loop, return error */
    } else {
        /* process blocks until last block or error */
        do {
            last = bits_asm(1);         /* one if last block */
            type = bits_asm(2);         /* block type 0..3 */
            err = type == 0 ?
                    stored() :
                    (type == 1 ?
                        fixed() :
                        (type == 2 ?
                            dynamic() :
                            -1));       /* type == 3, invalid */
            if (err != 0)
                break;                  /* return with error */
        } while (!last);
    }

    return err;
}
