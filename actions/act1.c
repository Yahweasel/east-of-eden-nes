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

/* Prologue: Cain and Abel */
char cdecl cainAndAbel(char idx)
{
    char cab;

    dialogue(0,     "Abel: Your fields of fine grain truly reflect the divine "
                    "glory of our Lord God, Cain! Be not concerned with the "
                    "respect God shows for my sheep; have faith that He only "
                    "wants what's best for us both!");
    dialogue(idx,   "Cain: ...");
    dialogue(0,     "... Cain?");
    dialogue(idx,   "...");
    dialogue(0,     "Brother?");

    fadetime = FADE_TIME_VERYFAST;
    fadeout();

    loadCharacter(NUM_CHARACTERS, SPR_cain);

    battleConfig.inescapable = 1;
    cab = battle();

    /* Prepare for full-screen text! */
    discardScrollInfo();
    clrscr();

    if (cab) {
        /* Abel won?! */
        fullscreen("So... Abel wasn't supposed to win.\n\nYou cheating bastard.");
        pos.bank = 0;
        return 0;
    }

    pause(60);

    /* Now we spit some bible at the hapless user */
    fullscreen(
        "And the Lord said unto Cain, Where is Abel thy brother? And he said, I "
        "know not: Am I my brother's keeper?"
        "\n\n"
        "And he said, What hast thou done? the voice of thy brother's blood "
        "crieth unto me from the ground."
        "\n\n"
        "And now art thou cursed from the earth, which hath opened her mouth to "
        "receive thy brother's blood from thy hand;"
        "\n\n"
        "When thou tillest the ground, it shall not henceforth yield unto thee "
        "her strength; a fugitive and a vagabond shalt thou be in the earth."
    );

    fullscreen(
        "And Cain said unto the Lord, My punishment is greater than I can bear."
        "\n\n"
        "Behold, thou hast driven me out this day from the face of the earth; "
        "and from thy face shall I be hid; and I shall be a fugitive and a "
        "vagabond in the earth; and it shall come to pass, that every one that "
        "findeth me shall slay me."
        "\n\n"
        "And the Lord said unto him, Therefore whosoever slayeth Cain, vengeance "
        "shall be taken on him sevenfold. And the Lord set a mark upon Cain, "
        "lest any finding him should kill him."
    );

    fullscreen(
        "And Cain went out from the presence of the Lord, and dwelt in the land "
        "of Nod, on the east of Eden."
    );

    pause(60);

    fullscreen(
        "Cain lived and had children, and Abel lives only in the story."
        "\n\n"
        "We are all Cain's children."
    );

    pause(60);

    fullscreen(
        "EDITOR'S NOTE"
        "\n\n"
        "Even if the Tanakh is taken literally, there is no canonical evidence "
        "that we are the descendants of Cain. Adam and Eve had other children, "
        "and Noah_and thus the entire male progeny surviving the flood_descended "
        "from Seth, their third. If Noah's wife or the wife of one of his "
        "children was a descendant of Cain, then we're descended through that "
        "line, but scripture treats women with a bit less respect than it treats "
        "cattle, so whether this is the case is undocumented."
        "\n\n"
        "But hey, let's not fault Steinbeck for the ignorance of one of his "
        "characters."
    );

    pause(60);

    fullscreen("PART I");

    pause(60);

    pos.bank = 0;
    return 0;
}

#define TEST(n) \
char cdecl test ## n() \
{ \
    pos.bank = WORLD_ff3a_BANK; \
    pos.idx = WORLD_ff3a + n; \
    fadetime = FADE_TIME_FAST; \
    fadeout(); \
    while (readjoy()); \
    return 0; \
}

TEST(0)
TEST(1)
TEST(2)
TEST(3)
TEST(4)
TEST(5)
TEST(6)
TEST(7)
TEST(8)

char cdecl testCain(char idx)
{
    dialogue(idx,   "Cain: WELL HEY THERE LI'L BUDDY!!!!!!!!! It's your old buddy Cain who definitely ain't gonna kill ya dead!");
    dialogue(0,     "Abel: ... please don't kill me.");
    dialogue(idx,   "LOL NICE TRY BITCH");
    overworldReload(11);
    return 1;
}

char cdecl testTalk()
{
    dialogue(0, "Abel: What a beautiful day!");
    overworldReload(11);
    return 1;
}