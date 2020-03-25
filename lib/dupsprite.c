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

#include <string.h>

#include "lib.h"

char dupSprite(char idx)
{
    char oamidx;

    /* Find an unused OAM/loc slot */
    for (oamidx = 0; oamidx < FULL_SPRITES; oamidx++)
        if (spriteLocs[oamidx].xscreen == 0xff) break;

    if (oamidx >= FULL_SPRITES) {
        /* Can't proceed here! */
        return -1;
    }

    /* Duplicate the existing sprite */
    memcpy(spriteLocs + oamidx, spriteLocs + idx, sizeof(struct SpriteLoc));
    idx *= 4;
    oamidx *= 4;
    memcpy(oamSprites + oamidx, oamSprites + idx, sizeof(struct OAMSprite) * 4);

    return oamidx / 4;
}
