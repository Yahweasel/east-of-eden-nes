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

char loadSprite(char *pal, char bank, char idx)
{
    char pidx, chridx, oamidx, i;

    /* Find an unused palette position */
    for (pidx = 0x11; pidx < 0x22; pidx += 4) {
        if (paletteTarget[pidx] == 0xff) {
            /* Load it in */
            memcpy(paletteTarget + pidx, pal, 3);
            break;
        }
        if (!memcmp(paletteTarget + pidx, pal, 3)) break;
    }

    if (pidx >= 0x22) {
        /* Just use an available palette */
        pidx = 0x11;
    }

    pidx = (pidx - 0x11) / 4;

    /* Find an unused CHR slot */
    for (chridx = 0; chridx < CHR_SPRITES; chridx++) {
        if (chrSprites[chridx].bank == 0) {
            /* Load it in */
            loadChr(chridx*2 + 16, bank, idx);
            loadChr(chridx*2 + 17, bank, idx+1);
            chrSprites[chridx].bank = bank;
            chrSprites[chridx].idx = idx;
            break;
        }
        if (chrSprites[chridx].bank == bank &&
            chrSprites[chridx].idx == idx) break;
    }

    if (chridx >= CHR_SPRITES) {
        /* Just use an available sprite (!) */
        chridx = 0;
    }

    /* Find an unused OAM/loc slot */
    for (oamidx = 0; oamidx < FULL_SPRITES; oamidx++)
        if (spriteLocs[oamidx].xscreen == 0xff) break;

    if (oamidx >= FULL_SPRITES) {
        /* Can't proceed here! */
        return -1;
    }

    /* Put it all together */
    memset(spriteLocs + oamidx, 0, sizeof(struct SpriteLoc));
    spriteLocs[oamidx].chrIdx = chridx;

    oamidx *= 4;
    oamSprites[oamidx].tile = chridx * 32;
    oamSprites[oamidx].attr = 0x10 | pidx;
    for (i = oamidx+1; i < oamidx+4; i++) {
        oamSprites[i].tile = oamSprites[oamidx].tile + i;
        oamSprites[i].attr = 0x10 | pidx;
    }

    return oamidx / 4;
}
