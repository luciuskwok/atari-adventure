// atari_memmap.h

#ifndef ATARI_MEMMAP_H
#define ATARI_MEMMAP_H

// Zero Page
#define APPMHI (0x000E)
#define ATRACT (0x004D)
#define LMARGN (0x0052)
#define RMARGN (0x0053)
#define ROWCRS (0x0054)
#define COLCRS (0x0055)
#define SAVMSC (0x0058)
#define RAMTOP (0x006A)
#define SHORT_CLOCK (PEEK(20) + 256 * PEEK(19))

// Page 2 - OS
#define VDSLST (0x0200) /* Display list interrupt vector */
#define VVBLKI (0x0222) /* Immediate vertical blank interrupt vector */
#define VVBLKD (0x0224) /* Deferred vertical blank interrupt vector */

#define SDMCTL (0x022F)
#define SDLSTL (0x0230)
#define GPRIOR (0x026F)

// Joystick
#define STICK0 (0x0278)
#define STRIG0 (0x0284)

// Colors
#define PCOLR0 (0x02C0)
#define PCOLR1 (0x02C1)
#define PCOLR2 (0x02C2)
#define PCOLR3 (0x02C3)
#define COLOR0 (0x02C4)
#define COLOR1 (0x02C5)
#define COLOR2 (0x02C6)
#define COLOR3 (0x02C7)
#define COLOR4 (0x02C8)
#define TXTBKG (0x02CA)

// Sprites
#define HPOSP0 (0xD000)
#define SIZEP0 (0xD008)

// Memory Management
#define MEMTOP (0x02E5)
#define CHBAS (0x02F4)

// Macros
#define POKE(addr,val)  (*(unsigned char*) (addr) = (val))
#define POKEW(addr,val)  (*(unsigned*) (addr) = (val))
#define PEEK(addr)  (*(unsigned char*) (addr))
#define PEEKW(addr)  (*(unsigned*) (addr))

#endif
