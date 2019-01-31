// atari_memmap.h

#ifndef ATARI_MEMMAP_H
#define ATARI_MEMMAP_H

// Zero Page - OS
#define APPMHI (0x0E)
#define ATRACT (0x4D)

#define TMPCHR (0x50) /* Temporary register used by display handler */
#define HOLD1  (0x51) /* Also temporary register */
#define LMARGN (0x52) /* Left margin */
#define RMARGN (0x53) /* Right margin */
#define ROWCRS (0x54) /* Current graphics or text cursor row */
#define COLCRS (0x55) /* Current graphics or text cursor column, 16-bit */
#define DINDEX (0x57) /* Current screen/display mode */
#define SAVMSC (0x58) /* Pointer to screen memory, 16-bit */
#define OLDROW (0x5A) /* Previous graphics cursor row */
#define OLDCOL (0x5B) /* Previous graphics cursor column, 16-bit */
#define NEWROW (0x60)
#define NEWCOL (0x61)
#define SAVADR (0x68) /* Temporary pointer for screen row */
#define RAMTOP (0x6A)
#define BITMSK (0x6E)
#define DELTAR (0x76)
#define DELTAC (0x77)
#define ROWINC (0x79)
#define COLINC (0x7A)

#define Clock16 (PEEK(20) + 256 * PEEK(19))


// Page 2 - OS
#define VDSLST (0x0200) /* Display list interrupt vector */
#define VVBLKI (0x0222) /* Immediate vertical blank interrupt vector */
#define VVBLKD (0x0224) /* Deferred vertical blank interrupt vector */
#define GPRIOR_ (*(unsigned char *)0x026F) /* cc65 atari.h has wrong address as of 2019-01-31 */

// Text Window
#define TXTMSC (0x0294) /* Pointer to text window memory */
#define BOTSCR (0x02BF)

// Custom Colors
#define TXTBKG (0x02CA)

// Sprites (GTIA)
#define HPOSP0 (0xD000)
#define HPOSP1 (0xD001)
#define HPOSP2 (0xD002)
#define HPOSP3 (0xD003)
#define HPOSM0 (0xD004)
#define HPOSM1 (0xD005)
#define HPOSM2 (0xD006)
#define HPOSM3 (0xD007)
#define SIZEP0 (0xD008)
#define SIZEP1 (0xD009)
#define SIZEP2 (0xD00A)
#define SIZEP3 (0xD00B)

// Sound (POKEY)
#define AUDF1  (0xD200)
#define AUDC1  (0xD201)
#define AUDCTL (0xD208)
#define RANDOM (0xD20A)
#define SKCTL  (0xD20F)

// ANTIC
#define NMIEN  (0xD40E) 

// Macros
#define POKE(addr,val)  (*(unsigned char*) (addr) = (val))
#define POKEW(addr,val)  (*(unsigned*) (addr) = (val))
#define PEEK(addr)  (*(unsigned char*) (addr))
#define PEEKW(addr)  (*(unsigned*) (addr))

// Accessors for C
#define ATRACT_value (*(char*)ATRACT)
#define SAVMSC_ptr (*(char**)SAVMSC)
#define SAVADR_ptr (*(char**)SAVADR)
#define ROWCRS_value (*(char*)ROWCRS)
#define COLCRS_value (*(char*)COLCRS)
#define BITMSK_value (*(char*)BITMSK)

#define RANDOM_value (*(char*)RANDOM)

#endif
