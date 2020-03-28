/*
 * Copyright (c) 2019-2020 Yahweasel
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
#include <stdlib.h>

#include "lib.h"

static void license(void);

void main()
{
    char i, c, j;
    uint l;
    libinit();

    /* Direct PPU control while we load things in */
    ppu_direct();

    /* Load the basic ASCII and title CHR set */
    for (i = 2; i < 8; i++)
        loadChr(i, CHR_ascii_BANK, CHR_ascii + i - 2);
    for (; i < 16; i++)
        loadChr(i, CHR_title_BANK, CHR_title + i - 8);

    /* Start all black except for the main palette */
    vram_addr(0x3f00);
    for (i = 0; i < 0x20; i++) {
        vram_write(0x0f);
        paletteTarget[i] = 0x0f;
    }
    paletteTarget[1] = 0x16;
    paletteTarget[2] = 0x07;
    paletteTarget[3] = 0x10;

    /* Give PPU control back to the NMI */
    ppu_enddirect();

    /* And clear the screen */
    clrscr();
    goto gettoit;

    /* Load our doofusy title screen */
    l = 0x2108;
    for (i = 0x80; i; i += 0x10) {
        for (c = 0; c < 16; c++)
            ppubuf_put(l + c, i + c);
        l += 0x0020;
    }

    printCursor(0, 18);
    //           .                          .
    printCentered("The video game that");
    printCentered("no one has ever asked");
    printCentered("for or wanted");

    printCursor(0, 24);
    printCentered("by Yahweasel");
    printCentered("youtube.com/c/Yahweasel");
    printCentered("based on the novel by");
    printCentered("John Steinbeck");

    ppubuf_waitempty();

    /* And fade it in */
    fadein();

    /* Give the title screen the jitters while we wait for input */ 
    c = 0;
    while (readjoy());
    while (!(j = readjoy())) {
        for (i = 0; i < 4 && !readjoy(); i++) {
            srand(c);
            while (!(PPU.status & 0x80)) {
                PPU.scroll = (rand()>0x7f00);
                PPU.scroll = 0;
            }
        }
        c++;
    }
    fadeout();
    clrscr();

    /* Show license text if requested */
    if (j & JOY_SELECT) {
        license();
        return;
    }

#if 0
    /* Disclaimer */
    paletteTarget[3] = 0x30;
    printCursor(0, 11);
    printCentered("DISCLAIMER");
    //           .                          .
    printCursor(0, 13);
    printCentered("This game is an absurdist");
    printCentered("parody: The fact that it's");
    printCentered("incredibly stupid to make");
    printCentered("a video game of East of");
    printCentered("Eden is why I did. Don't");
    printCentered("take it too seriously!");
    ppubuf_waitempty();
    fadein();
    while (readjoy());
    while (!readjoy());
    fadeout();
#endif

    pause(60);

    /* Intro */
    fullscreen("PROLOGUE");

    pause(60);

    /* Get our initial position */
    gettoit:
    pos.bank = WORLD_cain_and_abel_BANK;
    pos.idx = WORLD_cain_and_abel;
    pos.xt = pos.yt = 8;
    pos.dir = SPRITE_DOWN;
    loadCharacter(0, CHAR_abel);

    pos.bank = WORLD_cyrus_home_BANK;
    pos.idx = WORLD_cyrus_home;
    loadCharacter(0, CHAR_adam);

    /* And run the game */
    while (1) {
        if (!pos.bank) {
            /* Game must be over */
            break;
        }

        overworld(pos.bank, pos.idx);
    }

    /* When we return, the game effectively restarts */
#pragma warn (unused-label, push, off)
}
#pragma warn (unused-label, pop)

/* Display license text */
static void license()
{
    fadetime = FADE_TIME_VERYFAST;

#define P printCentered
#define W() do { \
    ppubuf_waitempty(); \
    fadein(); \
    while (readjoy()); \
    while (!readjoy()); \
    fadeout(); \
    clrscr(); \
    printCursor(0, 4); \
} while (0)

    fullscreen(
        "MAIN LICENSE"
        "\n\n"
        "Copyright (c) 2019 Yahweasel"
        "\n\n"
        "Permission to use, copy, modify, and/or distribute this software for any "
        "purpose with or without fee is hereby granted, provided that the above "
        "copyright notice and this permission notice appear in all copies."
        "\n\n"
        "THE SOFTWARE IS PROVIDED \"AS IS\" AND THE AUTHOR DISCLAIMS ALL WARRANTIES "
        "WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF "
        "MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR "
        "ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES "
        "WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN "
        "ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF "
        "OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE."
    );

    fullscreen(
        "CC65 LICENSE"
        "\n\n"
        "Parts of this code are derived from the cc65 compiler, and this code "
        "uses the cc65 compiler's libraries. cc65 is under the following "
        "license. Note that portions of this code have been modified, and are "
        "labeled as such in the source distribution."
        "\n\n"
        "This software is provided 'as-is', without any express or implied warranty. "
        "In no event will the authors be held liable for any damages arising from "
        "the use of this software."
        "\n\n"
        "Permission is granted to anyone to use this software for any purpose, "
        "including commercial applications, and to alter it and redistribute it "
        "freely, subject to the following restrictions:"
        "\n\n"
        "1. The origin of this software must not be misrepresented; you must not "
        "claim that you wrote the original software. If you use this software in "
        "a product, an acknowledgment in the product documentation would be "
        "appreciated but is not required."
        "\n\n"
        "2. Altered source versions must be plainly marked as such, and must not "
        "be misrepresented as being the original software."
        "\n\n"
        "3. This notice may not be removed or altered from any source distribution."
    );

    fullscreen(
        "ART ASSETS"
        "\n\n"
        "The art assets in this game are lifted from other sources, as a parody "
        "of common bootleg games_and because the creator of this game has no "
        "knack for art_but it is likely that such a use does not fall under fair "
        "use clauses, as it is not the original source which is being parodied. "
        "They will be removed upon request from their owners."
    );
}
