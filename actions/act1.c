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

    loadCharacter(NUM_CHARACTERS, CHAR_cain);

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

    loadCharacter(0, CHAR_cyrus);

    pos.bank = WORLD_civil_war_battlefield_BANK;
    pos.idx = WORLD_civil_war_battlefield;
    pos.xscreen = pos.yscreen = 0;
    pos.xt = pos.yt = 8;
    pos.dir = SPRITE_RIGHT;

    fullscreen("PART I");

    pause(60);

    return 0;
}

/* Part I Chapter 3 Section 1 */
char cdecl cyrusVsConfederate(char idx)
{
    static char i;
    char b;

    dialogue(idx, "Confederate: Have at thee!");

    for (i = 0; i < 4; i++)
        loadCharacter(NUM_CHARACTERS+i, SPR_confederate);

    battleConfig.inescapable = 1;
    b = battle();

    if (b) {
        discardScrollInfo();
        clrscr();
        fullscreen("OK, I'm not gonna warn you again. YOU'RE NOT SUPPOSED TO WIN.");
        pos.bank = 0;
        return 0;
    }

    overworldFullReload();
    fadein();

    dialogue(0, "Cyrus: My leg!");

    overworldReload(0);

    fadeout();

    loadCharacter(0, CHAR_cyrus_oneleg);

    /* FIXME: Some scene to transition to home life */

    return 0;
}

/* Part I Chapter 3 Section 3: Peewee */
char cdecl peeweeReq(char idx)
{
    dialogue(idx, "Charles: Hey Adam, let's play Peewee!");
    dialogue(-1, "Alice: Now children, be careful. Don't hurt yourselves!");
    dialogue(0, "Adam: You're not my real mom!");

    spriteLocs[idx].xscreen = -1;
    updateSprite(idx);

    loadCharacter(1, CHAR_charles);

    overworldReload(0);

    return 1;
}

void peeweeThrowTerrible(char pwIdx)
{
#include "gen-arc1.c"
}

void peeweeThrowGreat(char pwIdx)
{
    static char i;

    spriteLocs[pwIdx].xt = 8;
    spriteLocs[pwIdx].xs = 12;
    spriteLocs[pwIdx].yt = 8;
    spriteLocs[pwIdx].ys = 0;
    updateSprite(pwIdx);

    /* Move the peewee ball for Adam... */
    for (i = 0; i < 60; i++) {
        if (spriteLocs[pwIdx].xs >= 15) {
            spriteLocs[pwIdx].xt++;
            spriteLocs[pwIdx].xs = 0;
        } else {
            spriteLocs[pwIdx].xs++;
        }
        if (spriteLocs[pwIdx].ys == 0) {
            spriteLocs[pwIdx].yt--;
            spriteLocs[pwIdx].xs = 15;
        } else {
            spriteLocs[pwIdx].ys--;
        }
        updateSprite(pwIdx);
        pause(1);
    }
}

/* From leaving to play Peewee through... pretty much Adam and Charles' entire
 * childhoods */
char cdecl peeweeLeave(char idx)
{
    /* FIXME: Loop over sprites to choose the right one, don't just assume! */
    const char charlesIdx = 1;
    const char pwIdx = 2;

    pos.bank = WORLD_peewee_BANK;
    pos.idx = WORLD_peewee;
    pos.xscreen = pos.yscreen = 0;
    pos.xt = pos.yt = 8;
    pos.dir = SPRITE_RIGHT;

    fadeout();
    pause(60);

    overworldLoad(pos.bank, pos.idx);
    spriteLocs[pwIdx].xt = 8;
    spriteLocs[pwIdx].xs = 12;
    updateSprite(pwIdx);
    fadein();

    dialogue(charlesIdx, "Charles: I'm amazing at this, just watch me!");
    overworldReload(0);

    /* Move the peewee ball */
    peeweeThrowTerrible(pwIdx);

    dialogue(0, "Adam: That was great, let me try!");
    overworldReload(0);

    peeweeThrowGreat(pwIdx);

    dialogue(charlesIdx, "...");
    dialogue(0, "I guess it was just an accident...");
    overworldReload(0);

    peeweeThrowTerrible(pwIdx);

    dialogue(charlesIdx, "...");
    dialogue(0, "... I bet I couldn't do it again.");
    overworldReload(0);

    peeweeThrowGreat(pwIdx);

    dialogue(charlesIdx, "...");

    /* Now Adam fights Charles (and loses) */
    /* FIXME: Charles is probably also on our team */
    loadCharacter(NUM_CHARACTERS, CHAR_charles);

    fadetime = FADE_TIME_VERYFAST;
    fadeout();
    battleConfig.inescapable = 1;
    battleConfig.knockout = 1;
    battle();

    /* Move him back home for rehabilitation */
    pos.bank = WORLD_cyrus_home_BANK;
    pos.idx = WORLD_cyrus_home;
    pos.xscreen = pos.yscreen = 0;
    pos.xt = 5;
    pos.yt = 6;
    pos.dir = SPRITE_DOWN;

    overworldLoad(pos.bank, pos.idx);
    fadein();

    pause(120);

    fadeout();

    /* Prepare for full-screen text! */
    discardScrollInfo();
    clrscr();
    clearSprites();

    fullscreen("Years later...");

    /* Onto the rutty road */
    pos.bank = WORLD_rutty_road_BANK;
    pos.idx = WORLD_rutty_road;
    pos.xscreen = pos.yscreen = 0;
    pos.xt = 2;
    pos.yt = 6;
    pos.dir = SPRITE_RIGHT;

    loadCharacter(1, CHAR_charles);

    return 0;
}

#define RIGHT_ONLY() do { \
    if (pos.dir != SPRITE_RIGHT) { \
        pos.xt++; \
        spriteLocs[0].xt++; \
        updateSprite(0); \
        return 1; \
    } \
} while(0)

#define RIGHT_FORCE() do { \
    pos.xt++; \
    spriteLocs[0].xt++; \
    updateSprite(0); \
} while (0)

/* Walking down the rutty road */
char cdecl ruttyRoad1(char idx)
{
    RIGHT_ONLY();

    /* FIXME: Make this sprite appear correctly */
    dialogue(1, "Charles: Want to go in and see if anything's stirring at the inn?");
    dialogue(0, "Adam: I hadn't thought of it.");
    overworldReload(0);

    RIGHT_FORCE();

    return 1;
}

/* Walking down the rutty road part deux */
char cdecl ruttyRoad2(char idx)
{
    RIGHT_ONLY();

    dialogue(1, "Then what the hell are you walking out at night for?");
    dialogue(0, "You didn't have to come.");
    overworldReload(0);

    RIGHT_FORCE();

    return 1;
}

/* Walking down the rutty road part troix */
char cdecl ruttyRoad3(char idx)
{
    RIGHT_ONLY();

    dialogue(1, "What did he say to you this afternoon? I saw you walking together. What did he say?");
    dialogue(0, "He just talked about the army_like always.");
    dialogue(1, "Didn't look like that to me.");
    overworldReload(0);

    RIGHT_FORCE();

    return 1;
}

/* Walking down the rutty road part el four */
char cdecl ruttyRoad4(char idx)
{
    RIGHT_ONLY();

    dialogue(1, "I saw him leaning close, talking the way he talks to men_not telling, talking.");
    dialogue(0, "He was telling.");
    dialogue(1, "What did he tell you?!");
    overworldReload(0);

    RIGHT_FORCE();

    return 1;
}

/* Walking down the rutty road part el five */
char cdecl ruttyRoad5(char idx)
{
    RIGHT_ONLY();

    dialogue(0, "About the army and how it is to be a soldier.");
    dialogue(1, "I don't believe you! I think you're a goddam mealy-mouthed liar. What're you trying to get away with?");
    dialogue(0, "Nothing.");
    overworldReload(0);

    RIGHT_FORCE();

    return 1;
}

/* Walking down the rutty road part el six */
char cdecl ruttyRoad6(char idx)
{
    RIGHT_ONLY();

    dialogue(1, "Your crazy mother drowned herself. Maybe she took a look at you. That'd do it.");
    overworldReload(0);

    RIGHT_FORCE();

    return 1;
}

/* Walking down the rutty road part el seven */
char cdecl ruttyRoad7(char idx)
{
    RIGHT_ONLY();

    dialogue(1, "You're trying to take him away! I don't know how you're going about it. What do you think you're doing?");
    dialogue(0, "Nothing.");
    overworldReload(0);

    RIGHT_FORCE();

    return 1;
}

/* Walking down the rutty road part el eight */
char cdecl ruttyRoad8(char idx)
{
    RIGHT_ONLY();

    dialogue(1, "Look at his birthday!");
    dialogue(1, "I took six bits and I bought him a knife made in Germany_three blades and a corkscrew, pearl-handled. Where's that knife? Do you ever see him use it? Did he give it to you? I never even saw him hone it. Have you got that knife in your pocket? What did he do with it? \"Thanks,\" he said, like that. And that’s the last I heard of a pearl-handled German knife that cost six bits.");
    overworldReload(0);

    RIGHT_FORCE();

    return 1;
}

char cathyIntro(void);

/* Walking down the rutty road part el nine */
char cdecl ruttyRoad9(char idx)
{
    RIGHT_ONLY();

    dialogue(1, "What did you do on his birthday? You think I didn't see? Did you spend six bits or even four bits? You brought him a mongrel pup you picked up in the woodlot. You laughed like a fool and said it would make a good bird dog. That dog sleeps in his room. He plays with it while he's reading. He's got it all trained. And where's the knife? \"Thanks,\" he said, just \"Thanks.\"");

    /* Now Adam fights Charles (and loses again) */
    characters[1].spriteNum = -1;
    loadCharacter(NUM_CHARACTERS, CHAR_charles);

    fadetime = FADE_TIME_VERYFAST;
    fadeout();
    battleConfig.inescapable = 1;
    battleConfig.knockout = 1;
    battle();

    /* Prepare for full-screen text! */
    discardScrollInfo();
    clrscr();
    clearSprites();

    fullscreen("Consciousness came back quick and frightening to Adam. His mind rolled in a painful mist. He pushed himself up on his knees and dragged himself off the road to the ditch that kept it drained. There was a foot of water in the ditch. Adam crawled quietly into the water, being very careful to make no splash.");

    fullscreen("The footsteps came close, slowed, moved on a little, came back. A sulphur match was struck and burned a tiny blue until the wood caught, lighting his brother's face grotesquely from below. Charles raised the match and peered around, and Adam could see the hatchet in his right hand.");

    fullscreen("When the match went out the night was blacker than before. At last Charles gave it up. His right hand rose and he threw the hatchet far off into the field. He walked rapidly away toward the pinched lights of the village.");

    return cathyIntro();
}

/* Part I Chapter 8 Section I: Cathy's introduction */
char cathyIntro() {
    pause(120);

    /* Move to Cathy's home as her parents */
    pos.bank = WORLD_ames_home_BANK;
    pos.idx = WORLD_ames_home;
    pos.xt = 5;
    pos.yt = 5;

    loadCharacter(0, CHAR_mrsames);
    loadCharacter(1, CHAR_mrames);

    fullscreen("ACT II");

    fullscreen("I believe there are monsters born in the world to human parents.");
    fullscreen("It is my belief that Cathy Ames was born with the tendencies, or lack of them, which drove and forced her all of her life. Some balance wheel was misweighted, some gear out of ratio. She was not like other people, never was from birth. And just as a cripple may learn to utilize his lack so that he becomes more effective in a limited field than the uncrippled, so did Cathy, using her difference, make a painful and bewildering stir in her world.");

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
