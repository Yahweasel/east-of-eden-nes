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

#include <conio.h>

#include "lib.h"

static char l, c, opt, chr, inp;

static void blank(void);

void cdecl menuPrime()
{
    struct ScrollInfo si;

    fadetime = FADE_TIME_VERYFAST;
    fadeout();

    pushScrollInfo(&si);
    clrscr();
    clearSprites();

    paletteTarget[1] = 0x00;
    paletteTarget[2] = 0x10;
    paletteTarget[3] = 0x30;
    for (l = 0; l < 0x20; l += 4)
        paletteTarget[l] = 0x0c;

    /* Load the sprites early */
    ppu_direct();
    for (chr = 0; chr < NUM_PARTY; chr++) {
        if (characters[chr].spriteNum == 0xFF) break;
        loadSpriteIdx(characters[chr].spriteNum);
        spriteLocs[chr].xt = SCREEN_WIDTH - 3;
        spriteLocs[chr].yt = 0xFE;
    }
    ppu_enddirect();

    /* Draw the surround */
    printCursor(2, 2);
    printChar('#');
    for (c = 3; c < SCREEN_WIDTH_2 - 3; c++)
        printChar('_');
    printChar('\\');

    for (l = 3; l < SCREEN_HEIGHT_2 - 3; l++) {
        printCursor(2, l);
        printChar('|');
        for (c = 3; c < SCREEN_WIDTH_2 - 3; c++)
            printChar(' ');
        printChar('|');
    }

    printCursor(2, SCREEN_HEIGHT_2 - 3);
    printChar('%');
    for (c = 3; c < SCREEN_WIDTH_2 - 3; c++)
        printChar('_');
    printChar('&');

    ppubuf_waitempty();

    /* Start by showing the first character */
    opt = 0;
    chr = 0;
    fadein();

    /* Now show the menu */
    while (1) {
        if (opt == 0) {
            /* Option 0 = character info */
            blank();
            spriteLocs[chr].yt = 2;
            spriteLocs[chr].status = SPRITE_DOWN;
            updateSprite(chr);

            printCursor(4, 4);
            printStr("Name");
            printCursor(6, 5);
            loadName(characters[chr].spriteNum);
            printStr(strBuf);

            printCursor(4, 9);
            printStr("HP: ");
            printNum(characters[chr].hp);
            printChar('/');
            printNum(characters[chr].maxHP);

            if (characters[chr].maxMP) {
                printStr("  MP: ");
                printNum(characters[chr].mp);
                printChar('/');
                printNum(characters[chr].maxMP);
            }

            /* Now each of the stats */
            for (l = 0; l < 4; l++) {
                printCursor(4, 13 + l*3);
                loadStatName(characters[chr].spriteNum, l);
                printStr(strBuf);
                printCursor(6, 14 + l*3);
                printNum(characters[chr].stats[l]);
            }

            while (readjoy());
            while (!(inp = readjoy())) {
                waitvsync();
                spriteLocs[chr].status = SPRITE_DOWN | ((tickcount>>3)&0x3);
                updateSprite(chr);
            }

            spriteLocs[chr].yt = 0xFE;
            updateSprite(0);
        }

        if (inp & (JOY_START|JOY_B))
            break;
        else if (inp & (JOY_RIGHT|JOY_LEFT)) {
            /* Hide the old character */
            spriteLocs[chr].xt = SCREEN_WIDTH - 3;
            spriteLocs[chr].yt = 0xFE;
            updateSprite(chr);

            /* Advance to the new character */
            if (inp & JOY_RIGHT) {
                chr++;
                if (chr >= NUM_PARTY || characters[chr].spriteNum == 0xFF)
                    chr = 0;
            } else {
                if (chr == 0)
                    chr = NUM_PARTY-1;
                else chr--;
                while (chr != 0 && characters[chr].spriteNum == 0xFF)
                    chr--;
            }

        }
    }

    fadeout();

    for (l = 0; l < 0x20; l += 4)
        paletteTarget[l] = 0x0f;

    popScrollInfo(&si);
    overworldFullReload();

    fadein();
}

/* Blank the inner part */
static void blank()
{
    for (l = 3; l < SCREEN_HEIGHT_2 - 3; l++) {
        printCursor(3, l);
        for (c = 3; c < SCREEN_WIDTH_2 - 3; c++)
            printChar(' ');
    }
}

#pragma data-name (push, "JUMPS")
void *jm1 = (void *) menuPrime;
#pragma data-name (pop)
