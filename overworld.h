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

#ifndef OVERWORLD_H
#define OVERWORLD_H 1

/* An overworld (area) is:
 *  * a width and height in screens
 *  * a background palette
 *  * a chr bank and index to load
 *  * an array of screens, which is in turn arrays of tiles
 *     * Each tile consists of 6 bits of the tile, and a two-bit palette
 *  * an array of attributes, again by screen then tiles, but each tile is only
 *     2 bits
 *     * Each tile's bits are an 'actionable' bit then a 'passable' bit
 *  * an array of 15 sprites in OverworldSprite format
 *  * an array of actions */
struct Overworld {
    char w, h;
    char pal[0x10];
    char chrBank, chrIdx;
    char d[1];
};

/* A sprite in the static data for an overworld */
struct OverworldSprite {
    char xscreen, yscreen, xt, yt;
    char spriteIdx, spriteStatus;
    char actBank, actIdx;
};

/* A world action is just the function to call when an action occurs in the
 * world */
struct WorldAction {
    char bank, idx;
    char xscreen, yscreen, xt, yt;
};

#define TILE_ACTION     0x2
#define TILE_PASSABLE   0x1

/* The current overworld */
extern struct Overworld *world;

/* Jump to an overworld by index in the current bank (indexed like functions) */
void cdecl overworldPrime(char idx);

#endif
