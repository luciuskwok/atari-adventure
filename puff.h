// puff.h

#include "types.h"

/* 
Notes on using puff():
This version of Puff is not re-entrant, because it uses globals for state in order to get faster execution.
*/

SInt8 puff(UInt8 *dest, UInt16 *destLen, const UInt8 *source, UInt16 *sourceLen);


/* Debugging */
extern UInt16 __fastcall__ bits_asm(UInt8 count);
extern UInt16 __fastcall__ decode_asm(const struct huffman *h);
extern UInt16 __fastcall__ get_one_bit(void);

int verify_fixed_tables(struct huffman *lencode, struct huffman *distcode);
void build_fixed_tables(struct huffman *lencode, struct huffman *distcode);


extern const UInt8 fixed_lengths[];
extern UInt16 fixed_lensym[];
extern UInt16 fixed_distsym[];
extern UInt8 fixed_lencnt[];
extern UInt8 fixed_distcnt[];



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

