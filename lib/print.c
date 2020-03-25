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

static char x, y, i, right, bot, *nta;
static uint a;

#define CURSOR(b) a = (0x2000 + (b)*0x400 + ((y)<<5) + (x))

/* Set the cursor at which to print */
void printCursor(char sx, char sy)
{
    x = scrollXT*2 + sx;
    y = scrollYT*2 + sy;
    if (y >= SCREEN_HEIGHT_2) y -= SCREEN_HEIGHT_2;
    if (x >= SCREEN_WIDTH_2) {
        x -= SCREEN_WIDTH_2;
        CURSOR(!scrollXC);
    } else {
        CURSOR(scrollXC);
    }
}

/* Print a single character */
void printChar(char v)
{
    ppubuf_put(a++, v);
    if (++x >= SCREEN_WIDTH_2) {
        x = 0;
        CURSOR(!scrollXC);
    }
}

/* Print a string */
void printStr(const char *v)
{
    while (*v)
        printChar(*v++);
}

/* Print a string on the center of the current line */
void printCentered(const char *str)
{
    char sy, sx;
    sy = y;
    sx = (SCREEN_WIDTH_2/2) - (strlen(str)+1)/2;
    printCursor(sx, sy);
    y = sy;
    printStr(str);
    y++;
}

/* Print a number */
void printNum(uint v)
{
    char ct;
    uint spare;
    ct = 0;
    spare = 0;

    /* Reverse it into a spare uint */
    while (v) {
        spare *= 10;
        spare += (v%10);
        v /= 10;
        ct++;
    }
    if (ct == 0) ct++;

    /* Then print it in natural order */
    while (ct--) {
        printChar('0' + (spare%10));
        spare /= 10;
    }
}

#define ATTR_CURSOR(b) do { \
    a = (0x23c0 + (b)*0x400 + ((y)/2*8 + (x)/2)); \
    nta = nametableAttributes + (b)*64 + ((y)/2*8 + (x)/2); \
    right = x&1; \
    bot = y&1; \
} while (0)

/* Set the cursor at which to set attributes for printing */
void printAttrSet(char sx, char sy)
{
    x = scrollXT + (sx);
    y = scrollYT + (sy);
    if (y >= SCREEN_HEIGHT_2/2) y -= SCREEN_HEIGHT_2/2;
    if (x >= SCREEN_WIDTH_2/2) {
        x -= SCREEN_WIDTH_2/2;
        ATTR_CURSOR(!scrollXC);
    } else {
        ATTR_CURSOR(scrollXC);
    }
}

/* Flush any attributes set by printing attributes; to be done at the end of a
 * line */
void printAttrFlush()
{
    ppubuf_put(a++, *nta++);
}

/* "Print" an attribute character */
void printAttr(char v)
{
    i = (right)*2 + (bot)*4;
    *nta &= ~(0x3 << i);
    *nta |= ((v) << i);
    if (right) {
        printAttrFlush();

        if (++x >= SCREEN_WIDTH_2/2) {
            x = 0;
            ATTR_CURSOR(!scrollXC);
        }

        right = 0;
    } else right = 1;
}
