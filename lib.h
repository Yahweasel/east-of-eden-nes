/*
 * Copyright (c) 2019 Yahweasel
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef LIB_H
#define LIB_H 1

#include "banks-gen.h"

typedef unsigned int uint;

/***************************************************************
 * FUNCTIONS
 **************************************************************/

/* Initialize library variables */
void libinit(void);

/* General-purpose function with various argument counts, to be used by #defines below */
char farcall1(char arg, char bank);
char farcall2(char arg1, char arg2, char bank);
char gfarcall0(char bank, char idx);
char gfarcall1(char arg, char bank, char idx);
char gfarcall2(char arg1, char arg2, char bank, char idx);

/* Set the VRAM read/write address */
void vram_addr(uint addr);

/* Write data to VRAM */
void vram_write(char d);

/* Put to the PPU during vblank */
void ppubuf_put(uint addr, char d);

/* Wait until the PPU buffer has emptied (i.e., the instructions have all been
 * run) */
void ppubuf_waitempty(void);

/* Finalize vblank updates (set scroll) */
void finvsync(void);

/* Fade to black over several frames */
void fadeout(void);

/* Fade in over several frames */
void fadein(void);

/* Read input from the (1st) joypad */
char readjoy(void);

/* Wait for input from the joypad in the normal way. i.e., wait until there
 * isn't input, then wait until there is input, and return that */
char waitjoy(void);

/* Show dialogue, complete with dialogue box, line-wrapping on word boundaries,
 * the sprite speaking, and box-wrapping on box boundaries */
void dialogue(char sprite, const char *str);

/* Show full-screen text */
void fullscreen(const char *str);

/* Clear all on-screen sprites */
void clearSprites(void);

/* Load a sprite. Returns the sprite number it loaded, or -1 if it failed. */
char loadSprite(char *pal, char bank, char idx);

/* Duplicate a sprite */
char dupSprite(char idx);

/* Update an OAM sprite based on the data in SpriteLoc and scrolling state */
void updateSprite(char idx);

/* Unload a sprite. Note that this does not intelligently unload the CHR or
 * palette data. */
void unloadSprite(char idx);

/* Set the cursor at which to print */
void printCursor(char sx, char sy);

/* Print a single character */
void printChar(char v);

/* Print a string */
void printStr(const char *v);

/* Same, but center it on the line, and auto-advance to the next line */
void printCentered(const char *v);

/* Print a number */
void printNum(uint v);

/* Set the cursor at which to set attributes for printing */
void printAttrSet(char sx, char sy);

/* Flush any attributes set by printing attributes; to be done at the end of a
 * line */
void printAttrFlush(void);

/* "Print" an attribute character */
void printAttr(char v);

/* Discard scroll info (scroll to 0x0) */
void discardScrollInfo(void);

/* Push scroll info */
void pushScrollInfo(struct ScrollInfo *si);

/* Pop scroll info */
void popScrollInfo(struct ScrollInfo *si);


/***************************************************************
 * MACROS
 **************************************************************/

/* Load sprite information into an active sprite */
#define loadSpriteIdx(idx) (farcall1((idx), 0))

/* Load a character from sprite info */
#define loadCharacter(to, from) (gfarcall2((to), (from), 0, 2))

/* Load a character's name for sprite info */
#define loadName(idx) (gfarcall1((idx), 0, 4))

/* Load a character's stat name */
#define loadStatName(idx, st) (gfarcall2((idx), (st), 0, 6))

/* A battle! */
#define battle() (gfarcall0(0, 8))

/* The overworld menu */
#define menu() (gfarcall0(0, 10));

/* Load in a character palette:
 * Into the given CHR page,
 * from the given bank,
 * of the given index */
#define loadChr(page, bank, idx) (farcall2((idx), (page), (bank)))

/* Switch to an overworld on the given bank */
#define overworld(bank, idx) (farcall1((idx), (bank)))

/* Load an overworld on the given bank, but don't actually start overworld code */
#define overworldLoad(bank, idx) (gfarcall1((idx), (bank), 2))

/* Reload an overworld's graphics from the given row, if nothing has been
 * really unloaded */
#define overworldReload(from) (gfarcall1((from), pos.bank, 4))

/* Fully reload an overworld, after unloading it */
#define overworldFullReload() (gfarcall0(pos.bank, 6))

/* Pause for a number of frames */
#define pause(frames) do { \
    char p; \
    for (p = (frames); p; p--) waitvsync(); \
} while (0)

/* Take direct control of the PPU, bypassing NMI */
#define ppu_direct() do { \
    waitvsync(); \
    PPU.control = 0; \
    PPU.mask = 0; \
} while (0)

/* Re-enable NMI after taking direct control */
#define ppu_enddirect() do { \
    waitvsync(); \
    PPU.mask = 0x1a; \
    finvsync(); \
} while (0)


/***************************************************************
 * DEFINES
 **************************************************************/

/* In 16x16 tiles */
#define SCREEN_WIDTH    16
#define SCREEN_HEIGHT   15

/* In 8x8 tiles */
#define SCREEN_WIDTH_2  32
#define SCREEN_HEIGHT_2 30

/* The number of sprites we can load into CHR */
#define CHR_SPRITES     8

/* The number of 16x16 sprites we can load */
#define FULL_SPRITES    16

/* The number of 16x16 sprites a world is allowed to define */
#define WORLD_SPRITES   15

/* Size we reserve for sharing strings across banks */
#define STR_BUF_SZ      32

#undef JOY_SELECT
#undef JOY_START
#define JOY_A       0x80
#define JOY_B       0x40
#define JOY_SELECT  0x20
#define JOY_START   0x10
#define JOY_BUTTON  0xF0
#define JOY_UP      0x08
#define JOY_DOWN    0x04
#define JOY_LEFT    0x02
#define JOY_RIGHT   0x01
#define JOY_DPAD    0x0F

/* Sprite positions */
#define SPRITE_UP       0x0
#define SPRITE_DOWN     0x10
#define SPRITE_LEFT     0x20
#define SPRITE_RIGHT    0x30
#define SPRITE_BATTLE   0x40
#define SPRITE_ENEMY    0x50
#define SPRITE_EXTRA    0x60

/* Default fade times */
#define FADE_TIME_SLOW      10
#define FADE_TIME_FAST      2
#define FADE_TIME_VERYFAST  0

/* Number of characters and enemies the game supports */
#define NUM_CHARACTERS  8
#define NUM_PARTY       4
#define NUM_ENEMIES     4

/* Characters have up to 4 stats */
#define STAT_CT     4
#define STAT_STR    0
#define STAT_VIT    1
#define STAT_DUMB   2
#define STAT_SPD    3


/***************************************************************
 * STRUCTS
 **************************************************************/

/* Sprite DMA data (as defined by NES) */
struct OAMSprite {
    char y, tile, attr, x;
};

/* Sprite load data in CHR (as defined by us) */
struct CHRSprite {
    char bank, idx;
};

/* In-world sprite info. We only use most of this in overworld mode. Top 4 bits
 * of status is direction (up, down, left, right, battle), bottom 4 bits are
 * walking step. */
struct SpriteLoc {
    char xscreen, yscreen, xt, yt, xs, ys;
    char chrIdx, status;
};

/* The position and direction of a character in the (whole) world, including
 * direction as both movement and direct */
struct Pos {
    char bank, idx, xscreen, yscreen, xt, yt, xm, ym, dir;
};

/* Information on a given player character. spriteNum is -1 for "not loaded".
 * The statistics are generic, because different characters have different
 * statistics. The statistics are completely meaningless; in fact, the whole
 * battle system is meaningless. */
struct Character {
    char spriteNum;
    uint hp, maxHP,
                 mp, maxMP;
    uint stats[STAT_CT];
};

/* Saved scrolling info, for e.g. coming out of a battle */
struct ScrollInfo {
    char xc, xscreen, xt, xs, yscreen, yt, ys;
};

/* Battle configuration */
struct Battle {
    char inescapable;
};


/***************************************************************
 * VARIABLES
 * (documented in globals.c)
 **************************************************************/

extern unsigned int tickcount;
#pragma zpsym ("tickcount");

extern char nametableAttributes[128];

extern struct OAMSprite oamSprites[64];

extern char strBuf[STR_BUF_SZ];

extern char fadetime;

extern char currentBank;

extern char
    scrollXC, scrollXScreen, scrollXT, scrollXS,
              scrollYScreen, scrollYT, scrollYS;

extern struct CHRSprite chrSprites[CHR_SPRITES];

extern struct SpriteLoc spriteLocs[FULL_SPRITES];

extern char paletteTarget[0x20];

extern struct Pos pos;

extern struct Character characters[NUM_CHARACTERS];

extern struct Character enemies[NUM_ENEMIES];

extern struct Battle battleConfig;
#endif
