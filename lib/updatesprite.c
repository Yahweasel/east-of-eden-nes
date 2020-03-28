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

static struct OAMSprite *oamSprite;
static struct SpriteLoc *spriteLoc;
static char s, v;
static uint x, y;

/* In order to make updates faster, we cache all the correct tiles in an easily
 * indexable array. The tile arrangement in CHR is as follows:
 *  0: down top l, down top r, down bot l, down bot r,
 *  4: down bot w l, down bot w r, up top l, up top r,
 *  8: up bot l, up bot r, up bot w l, up bot w r,
 *  c: left top l, left top r, left bot l, left bot r,
 * 10: left bot w1 l, left bot w1 r, left bot w2 l, left bot w2 r,
 * 14: bat top l, bat top r, bat bot l, bat bot r, bat bot act l, bat bot act r
 * 1a: extra top l, extra top r, extra bot l, extra bot r
 *
 * The second-to-top bit is used to indicate flipping
 */
static const char offCache[] = {
    /* Up */
    0x6, 0x7, 0x8, 0x9,
    /* walk phases */
    0x6, 0x7, 0xa, 0xb,
    0x6, 0x7, 0x8, 0x9,
    0x6, 0x7, 0x4b, 0x4a,

    /* Down */
    0x0, 0x1, 0x2, 0x3,
    /* walk phases */
    0x0, 0x1, 0x4, 0x5,
    0x0, 0x1, 0x2, 0x3,
    0x0, 0x1, 0x45, 0x44,

    /* Left */
    0xc, 0xd, 0xe, 0xf,
    /* walk phases */
    0xc, 0xd, 0x10, 0x11,
    0xc, 0xd, 0xe, 0xf,
    0xc, 0xd, 0x12, 0x13,

    /* Right */
    0x4d, 0x4c, 0x4f, 0x4e,
    /* walk phases */
    0x4d, 0x4c, 0x51, 0x50,
    0x4d, 0x4c, 0x4f, 0x4e,
    0x4d, 0x4c, 0x53, 0x52,

    /* Battle */
    0x14, 0x15, 0x16, 0x17,
    /* battle action */
    0x14, 0x15, 0x18, 0x19,
    0x14, 0x15, 0x18, 0x19,
    0x14, 0x15, 0x18, 0x19,

    /* Battle (reverse) */
    0x55, 0x54, 0x57, 0x56,
    /* Reversed battle action */
    0x55, 0x54, 0x59, 0x58,
    0x55, 0x54, 0x59, 0x58,
    0x55, 0x54, 0x59, 0x58,

    /* Extra */
    0x1a, 0x1b, 0x1c, 0x1d,
    0x1a, 0x1b, 0x1c, 0x1d,
    0x1a, 0x1b, 0x1c, 0x1d,
    0x1a, 0x1b, 0x1c, 0x1d
};

void updateSprite(char idx)
{
    char i;
    spriteLoc = spriteLocs + idx;
    oamSprite = oamSprites + idx*4;

    /* First check if it's even on screen */
    x = spriteLoc->xt - (uint) scrollXT;
    if (spriteLoc->xscreen == scrollXScreen + 1) {
        x += SCREEN_WIDTH;
    } else if (spriteLoc->xscreen != scrollXScreen) {
        goto offscreen;
    }
    if (x >= SCREEN_WIDTH + 1)
        goto offscreen;

    y = spriteLoc->yt - (uint) scrollYT;
    if (spriteLoc->yscreen == scrollYScreen + 1) {
        y += SCREEN_HEIGHT;
    } else if (spriteLoc->yscreen != scrollYScreen) {
        goto offscreen;
    }
    if (y >= SCREEN_HEIGHT + 1)
        goto offscreen;

    /* Get and set its exact location */
    x = x*16 + spriteLoc->xs - scrollXS;
    y = y*16 + spriteLoc->ys - scrollYS - 3;
    for (i = 0; i < 4; i++) {
        uint xp = x + (i&0x1)*8;
        uint yp = y + (i&0x2)*4;
        if (xp < 256 && yp < 256) {
            oamSprite[i].x = xp;
            oamSprite[i].y = yp;
        } else {
            oamSprite[i].y = 0xFE;
        }
    }

    /* Pack the status for the offset array */
    s =  (spriteLoc->status & 0xF0) |
        ((spriteLoc->status & 0x03) << 2);

    /* And update the sprites */
    for (i = 0; i < 4; i++) {
        v = offCache[s+i];
        oamSprite[i].tile = (oamSprite[i].tile&0xe0) | (v&0x3F);
        oamSprite[i].attr = (oamSprite[i].attr&0xbf) | (v&0x40);
    }

    return;

offscreen:
    /* Use FE instead of FF so we can distinguish offscreen from unloaded */
    oamSprite[0].y = oamSprite[1].y = oamSprite[2].y = oamSprite[3].y = 0xFE;
}
