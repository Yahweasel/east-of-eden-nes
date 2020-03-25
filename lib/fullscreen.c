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
#include <nes.h>

#include "lib.h"

static char l, c, yt, ys, len;
static uint a;

void fullscreen(const char *str)
{
    /* Clear the screen's attributes */
    ppu_direct();
    vram_addr(0x23c0);
    for (a = 0x23c0; a < 0x2400; a++)
        PPU.vram.data = 0;
    ppu_enddirect();

    /* Then draw our text */
    while (*str) {
        scrollYT = scrollYS = 0;
        yt = SCREEN_HEIGHT / 2;
        ys = 8;
        clrscr();

        for (l = 0; *str && l < SCREEN_HEIGHT_2 - 4; l++) {
            /* Figure out the length of this line */
            for (len = 0; str[len] && str[len] != '\n' && len < SCREEN_WIDTH_2 - 4; len++);
            for (c = len; str[c] && str[c] != '\n' && str[c] != ' ' && c > 0; c--);
            if (c != 0) len = c;

            /* Print it */
            printCursor(15 - (len-1)/2, l);
            for (c = 0; c < len; c++)
                printChar(*str++);
            if (*str == '\n') str++;
            for (; *str == ' '; str++);

            /* And keep it centered */
            if (ys < 12) {
                ys += 4;
            } else {
                yt++;
                ys = 0;
            }
        }

        /* Wait for this screen */
        scrollYT = yt;
        scrollYS = ys;
        ppubuf_waitempty();
        fadein();
        waitjoy();
        fadeout();
    }

    scrollYT = scrollYS = 0;
}
