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

/* Discard scroll info (scroll to 0x0) */
void discardScrollInfo()
{
    scrollXC = scrollXScreen = scrollXT = scrollXS =
        scrollYScreen = scrollYT = scrollYS = 0;
}

/* Push scroll info */
void pushScrollInfo(struct ScrollInfo *si)
{
    si->xc = scrollXC;
    si->xscreen = scrollXScreen;
    si->xt = scrollXT;
    si->xs = scrollXS;

    si->yscreen = scrollYScreen;
    si->yt = scrollYT;
    si->ys = scrollYS;

    discardScrollInfo();
}

/* Pop scroll info */
void popScrollInfo(struct ScrollInfo *si)
{
    scrollXC = si->xc;
    scrollXScreen = si->xscreen;
    scrollXT = si->xt;
    scrollXS = si->xs;

    scrollYScreen = si->yscreen;
    scrollYT = si->yt;
    scrollYS = si->ys;
}
