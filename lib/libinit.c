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

#include <stdlib.h>
#include <string.h>

#include "lib.h"

extern void *world;

void libinit()
{
    memset(oamSprites, 0xFF, sizeof(oamSprites));
    fadetime = 10;
    currentBank = 0;
    scrollXScreen = scrollXC = scrollXT = scrollXS =
        scrollYScreen = scrollYT = scrollYS = 0;
    world = 0;
    pos.bank = pos.idx = pos.xscreen = pos.yscreen = pos.xt = pos.yt = 0;
    memset(characters, -1, sizeof(characters));
    memset(enemies, -1, sizeof(enemies));
    memset(&battleConfig, 0, sizeof(battleConfig));
}
