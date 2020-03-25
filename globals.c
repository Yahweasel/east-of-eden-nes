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

#include "lib.h"
#include "overworld.h"

/* The OAM sprites as defined by the NES, in the OAM segment */
#pragma bss-name (push, "OAM")
struct OAMSprite oamSprites[64];
#pragma bss-name (pop)

/* We keep a CPU-memory copy of the nametable attributes, since we can only write a byte at a time */
char nametableAttributes[128];

/* A general-purpose buffer for transferring strings */
char strBuf[STR_BUF_SZ];

/* The fade timing, in terms of frames per step minus one */
char fadetime;

/* The current bank, so generic code doesn't have to be compiled differently
 * for different banks */
char currentBank;

/* Position of the cursor for printing */
char cursorX, cursorY;

/* The current overworld */
struct Overworld *world;

/* Current scroll position in three parts:
 * The bit for the control flag (only in X),
 * the 16-pixel tile, and
 * the sub-tile position.
 * XScreen and YScreen are which screen the top-left of our view is scrolled to
 * in the overworld, and is only used by the overworld code, not the scrolling
 * code */
char
    scrollXC, scrollXScreen, scrollXT, scrollXS,
              scrollYScreen, scrollYT, scrollYS;

/* The sprite data currently loaded into CHR. Set the bank to 0 to indicate
 * nothing loaded. */
struct CHRSprite chrSprites[CHR_SPRITES];

/* The sprites which exist in the visible-ish world and OAM memory (even if
 * they're actually scrolled offscreen). Set xt to -1 to indicate nothing
 * loaded. */
struct SpriteLoc spriteLocs[FULL_SPRITES];

/* Target palette for fadein */
char paletteTarget[0x20];

/* The current position is:
 * An overworld bank and index,
 * which screen you're on within that world as X and Y, and
 * which tile you're on within that screen as X and Y */
struct Pos pos;

/* All player characters. The first four are our party, the remainder are other
 * characters we're remembering the stats of. Set spriteBank to 0 to indicate
 * no character loaded. */
struct Character characters[NUM_CHARACTERS];

/* The enemies for the current battle */
struct Character enemies[NUM_ENEMIES];

/* The configuration for the current battle */
struct Battle battleConfig;
