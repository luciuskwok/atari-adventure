// atari_memmap.h

#ifndef ATARI_MEMMAP_H
#define ATARI_MEMMAP_H

// Zero Page - OS
#define APPMHI (0x000E)
#define ATRACT (0x004D)
#define LMARGN (0x0052)
#define RMARGN (0x0053)
#define ROWCRS (0x0054)
#define COLCRS (0x0055)
#define SAVMSC (0x0058) /* Pointer to screen memory */
#define RAMTOP (0x006A)
#define ROWINC (0x0079)
#define COLINC (0x007A)

#define SHORT_CLOCK (PEEK(20) + 256 * PEEK(19))

// Page 2 - OS
#define VDSLST (0x0200) /* Display list interrupt vector */
#define VVBLKI (0x0222) /* Immediate vertical blank interrupt vector */
#define VVBLKD (0x0224) /* Deferred vertical blank interrupt vector */

#define SDMCTL (0x022F)
#define SDLSTL (0x0230) /* Pointer to display list */
#define GPRIOR (0x026F)

// Joystick
#define STICK0 (0x0278)
#define STRIG0 (0x0284)

// Text Window
#define TXTMSC (0x0294) /* Pointer to text window memory */
#define BOTSCR (0x02BF)

// Colors (Shadow)
#define PCOLR0 (0x02C0)
#define PCOLR1 (0x02C1)
#define PCOLR2 (0x02C2)
#define PCOLR3 (0x02C3)
#define COLOR0 (0x02C4)
#define COLOR1 (0x02C5) /* text luminance */
#define COLOR2 (0x02C6) /* text background color */
#define COLOR3 (0x02C7)
#define COLOR4 (0x02C8)
#define TXTBKG (0x02CA)

// Memory Management
#define MEMTOP (0x02E5)
#define CHBAS (0x02F4)

// Keyboard
#define CH1 (0x02F2)
#define CH_ (0x02FC)

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

// Sound (POKEY)
#define AUDF1  (0xD200)
#define AUDC1  (0xD201)
#define AUDCTL (0xD208)
#define SKCTL  (0xD20F)


// Macros
#define POKE(addr,val)  (*(unsigned char*) (addr) = (val))
#define POKEW(addr,val)  (*(unsigned*) (addr) = (val))
#define PEEK(addr)  (*(unsigned char*) (addr))
#define PEEKW(addr)  (*(unsigned*) (addr))

#endif
