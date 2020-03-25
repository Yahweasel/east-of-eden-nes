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

#include <nes.h>

#include "lib.h"

static char l, c, i, len;

#define DIALOGUE_LEFT   2
#define DIALOGUE_WIDTH  28
#define DIALOGUE_TOP    22
#define DIALOGUE_HEIGHT 6

void dialogue(char sprite, const char *str)
{
    /* Get rid of any sprites that are in the way */
    for (l = 0; l < 64; l++) {
        struct OAMSprite *oam = oamSprites + l;
        if (oam->y >= (DIALOGUE_TOP*8-7) &&
            oam->y < ((DIALOGUE_TOP+DIALOGUE_HEIGHT)*8) &&
            oam->x >= (DIALOGUE_LEFT*8-7) &&
            oam->y < ((DIALOGUE_LEFT+DIALOGUE_WIDTH)*8))
            oam->y = 0xFE;
    }

    /* Wait for a vsync now, before making the sprite, which will be noticeable
     * in a weird place otherwise */
    waitvsync();

    /* Put the speaking sprite in its place */
    if (sprite < 128) {
        /* Duplicate the existing sprite */
        sprite = dupSprite(sprite);
    }

    if (sprite < 128) {
        struct OAMSprite *oam = oamSprites + (sprite*4);

        /* Face it the right way */
        spriteLocs[sprite].status = SPRITE_DOWN ;
        updateSprite(sprite);

        /* Put it in the right place */
        oam->x = (DIALOGUE_LEFT+1)*8 - 2;
        oam->y = (DIALOGUE_TOP+1)*8 - 4;
        for (l = 1; l < 4; l++) {
            oam[l].x = oam->x + (l&1)*8;
            oam[l].y = oam->y + (l&2)*4;
        }
    }

    /* Draw our dialogue box in attributes */
    for (l = DIALOGUE_TOP/2; l < (DIALOGUE_TOP+DIALOGUE_HEIGHT)/2; l++) {
        printAttrSet(DIALOGUE_LEFT/2, l);
        for (c = 0; c < DIALOGUE_WIDTH/2; c++)
            printAttr(0);
        printAttrFlush();
    }

    /* And in tiles */
    printCursor(DIALOGUE_LEFT, DIALOGUE_TOP);
    if (sprite < 128)
        printChar('`');
    else
        printChar('#');
    c = 0;
    if (sprite < 128)
        for (; c < 2; c++)
            printChar('>');
    for (; c < DIALOGUE_WIDTH-2; c++)
        printChar('_');
    printChar('\\');

    for (l = 0; l < DIALOGUE_HEIGHT-2; l++) {
        printCursor(DIALOGUE_LEFT, DIALOGUE_TOP+1+l);
        if (sprite < 128 && l == 0) {
            printChar('<');
            printChar('^');
            printChar('^');
            c = 2;
        } else if (sprite < 128 && l == 1) {
            printChar('{');
            printChar('~');
            printChar('~');
            c = 2;
        } else {
            printChar('|');
            c = 0;
        }
        for (; c < DIALOGUE_WIDTH-2; c++)
            printChar(' ');
        printChar('|');
    }

    printCursor(DIALOGUE_LEFT, DIALOGUE_TOP+DIALOGUE_HEIGHT-1);
    printChar('%');
    for (c = 0; c < DIALOGUE_WIDTH-2; c++)
        printChar('_');
    printChar('&');

    /* Then draw our dialogue */
    while (*str) {
        for (l = 0; l < DIALOGUE_HEIGHT-2; l++) {
            i = ((sprite<128 && l<2) ? 3 : 0);

            /* Figure out the length of this line */
            for (len = 0; str[len] && len < DIALOGUE_WIDTH - 2 - i; len++);
            for (c = len; str[c] && str[c] != ' ' && c > 0; c--);
            if (c != 0) len = c;

            /* Print it */
            printCursor(DIALOGUE_LEFT + 1 + i, DIALOGUE_TOP + 1 + l);
            for (c = 0; c < len; c++)
                printChar(*str++);
            for (; *str == ' '; str++);
            for (; c < DIALOGUE_WIDTH - 2 - i; c++)
                printChar(' ');
        }

        /* Draw a '...' if there's more */
        printCursor(DIALOGUE_LEFT+DIALOGUE_WIDTH-4, DIALOGUE_TOP+DIALOGUE_HEIGHT-1);
        if (*str)
            for (c = 0; c < 3; c++) printChar('.');
        else
            for (c = 0; c < 3; c++) printChar('_');

        /* Wait for this screen */
        while (readjoy());
        while (!readjoy());
    }

    /* Get rid of our duplicated sprite */
    if (sprite < 128)
        unloadSprite(sprite);
}
