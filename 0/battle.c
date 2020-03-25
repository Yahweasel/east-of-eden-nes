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
#include <stdlib.h>
#include <string.h>

#include "lib.h"

/* NOTE: loadName is only safe in this file (that is, won't clobber these)
 * because they're both in bank 0! */
static char x, l, c;
static uint a;
static struct Character *actor, *target;

#define NUM_TOTAL (NUM_PARTY + NUM_ENEMIES)

static char sprites[NUM_TOTAL];
static char options[NUM_TOTAL];
static char args[NUM_TOTAL];
//static char order[NUM_TOTAL];

#define WIN_LEFT    1
#define WIN_WIDTH   30
#define WIN_DIV     15
#define WIN_TOP     19
#define WIN_HEIGHT  10

#define OPT_FIGHT   0
#define OPT_GUARD   1
#define OPT_RUN     2
#define OPT_MAGIC   3 /* lol */
#define OPT_ITEM    4 /* maybe in a future life... */

static void drawWindowAttrs(void);
static void drawWindowPane(void);
static void drawSplitWindow(void);
static void drawUnsplitWindow(void);
static void drawCharNames(void);
static void drawEnemyNames(void);
static void choosePartyActions(void);
static void chooseEnemyActions(void);
static char engage(void);
static void fight(char actorIsEnemy);
static void guard(void);
static char flee(void);

/* Perform a battle with the characters in characters[0-3] and the enemies in
 * enemies[0-3] */
char cdecl battlePrime()
{
    char victory;
    struct SpriteLoc *spriteLoc;
    struct ScrollInfo si;

    srand(tickcount);

    /* First clear out everything */
    pushScrollInfo(&si);
    clearSprites();

    paletteTarget[0] = 0x0f;
    paletteTarget[1] = 0x00;
    paletteTarget[2] = 0x10;
    paletteTarget[3] = 0x30;

    /* Draw the screen */
    ppu_direct();
    vram_addr(0x2000);
    for (a = 0x2000; a < 0x23c0; a++)
        PPU.vram.data = 0x80;
    for (; a < 0x2400; a++)
        PPU.vram.data = 0x55;
    ppu_enddirect();
    memset(nametableAttributes, 0x55, 128);

    /* And the window attrs */
    drawWindowAttrs();
    drawWindowPane();

    /* And the window */
    drawSplitWindow();

    /* Draw the names */
    drawCharNames();
    drawEnemyNames();
    ppubuf_waitempty();

    /* Load in the sprites */
    ppu_direct();
    for (x = 0; x < NUM_PARTY; x++) {
        if (characters[x].spriteNum == 0xFF) break;
        c = loadSpriteIdx(characters[x].spriteNum);
        if (c >= 128) continue;
        sprites[x] = c;
        spriteLoc = spriteLocs + c;
        spriteLoc->xt = 2;
        spriteLoc->yt = 1 + 2*x;
        spriteLoc->ys = 8;
        spriteLoc->status = SPRITE_BATTLE;
        updateSprite(c);
    }
    for (x = 0; x < NUM_ENEMIES; x++) {
        if (enemies[x].spriteNum == 0xFF) continue;
        c = loadSpriteIdx(enemies[x].spriteNum);
        if (c >= 128) continue;
        sprites[NUM_PARTY + x] = c;
        spriteLoc = spriteLocs + c;
        spriteLoc->xt = 11;
        spriteLoc->xs = 32; /* Room to slide using only xs */
        spriteLoc->yt = 1 + 2*x;
        spriteLoc->ys = 8;
        spriteLoc->status = SPRITE_ENEMY;
        updateSprite(c);
    }

    /* And icons */
    loadChr(0, CHR_icons1_BANK, CHR_icons1);
    ppu_enddirect();

    fadein();

    /* Now continue the battle until somebody wins! */
    while (1) {
        /* Is everybody on one side or the other dead? */
        for (x = 0; x < NUM_PARTY; x++) {
            if (characters[x].spriteNum == 0xFF) continue;
            if (characters[x].hp) break;
        }
        if (x == NUM_PARTY) {
            /* Party's dead! */
            victory = 0;
            break;
        }

        for (x = 0; x < NUM_ENEMIES; x++) {
            if (enemies[x].spriteNum == 0xFF) continue;
            if (enemies[x].hp) break;
        }
        if (x == NUM_ENEMIES) {
            /* Enemies dead! */
            victory = 1;
            break;
        }

        /* Not everyone is dead, so display the menu for each character */
        waitjoy();
        choosePartyActions();

        /* And "AI" for the enemies */
        chooseEnemyActions();

        /* Then do it */
        if (engage()) {
            victory = 2; /* 2 = fled */
            break;
        }
    }

    /* Battle over */
    memset(&battleConfig, 0, sizeof(battleConfig));
    ppubuf_waitempty();
    fadetime = victory ? FADE_TIME_VERYFAST : FADE_TIME_SLOW;
    fadeout();
    clearSprites();
    popScrollInfo(&si);
    return victory;
}

static void clrLeft()
{
    for (l = WIN_TOP + 1; l < WIN_TOP + WIN_HEIGHT - 1; l++) {
        printCursor(WIN_LEFT + 1, l);
        for (c = WIN_LEFT + 1; c < WIN_DIV; c++)
            printChar(' ');
    }
}

static void clrRight()
{
    for (l = WIN_TOP + 1; l < WIN_TOP + WIN_HEIGHT - 1; l++) {
        printCursor(WIN_DIV + 1, l);
        for (c = WIN_DIV + 1; c < WIN_LEFT + WIN_WIDTH - 1; c++)
            printChar(' ');
    }
}

static void drawWindowAttrs()
{
    for (l = WIN_TOP/2; l < SCREEN_HEIGHT_2/2; l++) {
        printAttrSet(0, l);
        for (c = 0; c < SCREEN_WIDTH_2/2; c++)
            printAttr(0);
        printAttrFlush();
    }
}

static void drawWindowPane()
{
    printCursor(0, WIN_TOP - 1);
    for (c = 0; c < SCREEN_WIDTH_2; c++)
        printChar(2);

    for (l = WIN_TOP; l < SCREEN_HEIGHT_2; l++) {
        printCursor(0, l);
        for (c = 0; c < SCREEN_WIDTH_2; c++)
            printChar(' ');
    }
}

static void drawSplitWindow()
{
    printCursor(WIN_LEFT, WIN_TOP);
    printChar('#');
    for (c = WIN_LEFT + 1; c < WIN_DIV; c++)
        printChar('_');
    printChar('[');
    for (c++; c < WIN_LEFT + WIN_WIDTH - 1; c++)
        printChar('_');
    printChar('\\');

    for (l = 0; l < WIN_HEIGHT - 2; l++) {
        printCursor(WIN_LEFT, WIN_TOP + 1 + l);
        printChar('|');
        for (c = WIN_LEFT + 1; c < WIN_DIV; c++)
            printChar(' ');
        printChar('|');
        for (c++; c < WIN_LEFT + WIN_WIDTH - 1; c++)
            printChar(' ');
        printChar('|');
    }

    printCursor(WIN_LEFT, WIN_TOP + WIN_HEIGHT - 1);
    printChar('%');
    for (c = WIN_LEFT + 1; c < WIN_DIV; c++)
        printChar('_');
    printChar(']');
    for (c++; c < WIN_LEFT + WIN_WIDTH - 1; c++)
        printChar('_');
    printChar('&');
}

static void drawUnsplitWindow()
{
    printCursor(WIN_LEFT, WIN_TOP);
    printChar('#');
    for (c = WIN_LEFT + 1; c < WIN_LEFT + WIN_WIDTH - 1; c++)
        printChar('_');
    printChar('\\');

    for (l = 0; l < WIN_HEIGHT - 2; l++) {
        printCursor(WIN_LEFT, WIN_TOP + 1 + l);
        printChar('|');
        for (c = WIN_LEFT + 1; c < WIN_LEFT + WIN_WIDTH - 1; c++)
            printChar(' ');
        printChar('|');
    }

    printCursor(WIN_LEFT, WIN_TOP + WIN_HEIGHT - 1);
    printChar('%');
    for (c = WIN_LEFT + 1; c < WIN_LEFT + WIN_WIDTH - 1; c++)
        printChar('_');
    printChar('&');
}

static void drawCharNames()
{
    clrLeft();
    for (x = 0; x < NUM_PARTY; x++) {
        if (characters[x].spriteNum == 0xFF) break;
        loadName(characters[x].spriteNum);
        printCursor(WIN_LEFT + 2, WIN_TOP + x*2 + 1);
        printStr(strBuf);
        printCursor(WIN_LEFT + 3, WIN_TOP + x*2 + 2);
        printNum(characters[x].hp);
        printChar('/');
        printNum(characters[x].maxHP);
    }
}

static void charStep(char by)
{
    struct SpriteLoc *spriteLoc = &spriteLocs[sprites[x]];
    for (c = 0; c < 8; c++) {
        spriteLoc->xs += by;
        spriteLoc->status = (spriteLoc->status&0xf0) | ((tickcount>>3)&0x1);
        updateSprite(sprites[x]);
        waitvsync();
    }
    spriteLoc->status = (spriteLoc->status&0xf0);
    updateSprite(sprites[x]);
}

static void drawEnemyNames()
{
    clrRight();
    for (x = 0; x < NUM_ENEMIES; x++) {
        if (enemies[x].spriteNum == 0xFF) continue;
        loadName(enemies[x].spriteNum);
        printCursor(WIN_DIV + 2, WIN_TOP + x*2 + 1);
        printStr(strBuf);
        printCursor(WIN_DIV + 3, WIN_TOP + x*2 + 2);
        printNum(enemies[x].hp);
        printChar('/');
        printNum(enemies[x].maxHP);
    }
}

static void choosePartyActions()
{
    for (x = 0; x < NUM_PARTY; x++) {
backoff:
        if (characters[x].spriteNum == 0xFF) break;
        if (!characters[x].hp) continue;

        charStep(2);

        /* Present options for this character
         * FIXME: Per-character options */
        clrLeft();

        printCursor(WIN_LEFT + 2, WIN_TOP + 1);
        loadName(characters[x].spriteNum);
        printStr(strBuf);
        printCursor(WIN_LEFT + 3, WIN_TOP + 3);
        printStr("Fight");
        printCursor(WIN_LEFT + 3, WIN_TOP + 5);
        printStr("Guard");
        printCursor(WIN_LEFT + 3, WIN_TOP + 7);
        printStr("Run");

        c = 0;

        while (1) {
            printCursor(WIN_LEFT + 1, WIN_TOP + 3 + (2*c));
            printChar(0);
            printChar(1);
            printCursor(WIN_LEFT + 1, WIN_TOP + 3 + (2*c));

            while (readjoy());
            while (!(l = readjoy()));

            if (l & JOY_UP) {
                printStr("  ");
                c--;
                if (c >= 128) c = OPT_RUN;

            } else if (l & JOY_DOWN) {
                printStr("  ");
                c++;
                if (c > OPT_RUN) c = 0;

            } else if (l & JOY_B) {
                if (x > 0) {
                    charStep(-2);
                    x--;
                    goto backoff;
                }

            } else if (l & JOY_A) {
                /* Finally, just choosing this option! */
                break;

            }
        }

        options[x] = c;

        /* Possibly choose an opponent */
        if (c == OPT_FIGHT) {
            for (c = 0;
                 enemies[c].spriteNum == 0xFF ||
                 !enemies[c].hp;
                 c++);

            while (1) {
                printCursor(WIN_DIV + 1, WIN_TOP + 1 + (2*c));
                printChar(1);
                printCursor(WIN_DIV + 1, WIN_TOP + 1 + (2*c));

                while (readjoy());
                while (!(l = readjoy()));

                if (l & JOY_UP) {
                    printChar(' ');
                    while (1) {
                        c--;
                        if (c >= 128) c = NUM_ENEMIES - 1;
                        if (enemies[c].spriteNum != 0xFF && enemies[c].hp) break;
                    }

                } else if (l & JOY_DOWN) {
                    printChar(' ');
                    while (1) {
                        c++;
                        if (c >= NUM_ENEMIES) c = 0;
                        if (enemies[c].spriteNum != 0xFF && enemies[c].hp) break;
                    }

                } else if (l & JOY_B) {
                    printChar(' ');
                    charStep(-2);
                    goto backoff;

                } else if (l & JOY_A) {
                    printChar(' ');
                    /* Enemy chosen */
                    break;

                }
            }

            args[x] = c;
        }

        charStep(-2);
    }

    drawCharNames();
    drawEnemyNames();
}

static void chooseEnemyActions()
{
    for (x = 0; x < NUM_ENEMIES; x++) {
        if (enemies[x].spriteNum == 0xFF) continue;
        if (!enemies[x].hp) continue;

        /* Not the most intelligent AI... */
        options[NUM_PARTY+x] = OPT_FIGHT;
        for (c = 0; c < NUM_PARTY; c++)
            if (characters[c].hp) break;
        args[NUM_PARTY+x] = c;
    }
}

static char engage()
{
    /* FIXME: Incorporate speed */
    for (x = 0; x < NUM_PARTY; x++) {
        actor = &characters[x];
        if (actor->spriteNum == 0xFF) break;
        if (!actor->hp) continue;
        charStep(2);
        drawUnsplitWindow();
        switch (options[x]) {
            case OPT_FIGHT:
                target = &enemies[args[x]];
                fight(0);
                break;

            case OPT_GUARD:
                guard();
                break;

            case OPT_RUN:
                if (flee())
                    return 1;
                break;

            /* FIXME: Other options */
        }
        charStep(-2);
    }

    for (; x < NUM_TOTAL; x++) {
        actor = &enemies[x-NUM_PARTY];
        if (actor->spriteNum == 0xFF) continue;
        if (!actor->hp) continue;
        charStep(-2);
        drawUnsplitWindow();
        switch (options[x]) {
            case OPT_FIGHT:
                target = &characters[args[x]];
                fight(1);
                break;

            /* Enemy AI never takes another option for now */
        }
        charStep(2);
    }

    drawSplitWindow();
    drawCharNames();
    drawEnemyNames();

    return 0;
}

static void fight(char actorIsEnemy)
{
    uint damage;
    uint rr;

    /* Calculate damage (FIXME: I really should read a book on RPG mechanics...) */
    if (actor->stats[STAT_STR] >= target->stats[STAT_VIT]) {
        damage = actor->stats[STAT_STR] / 2 + (actor->stats[STAT_STR] - target->stats[STAT_VIT]) / 2;

    } else {
        damage = target->stats[STAT_VIT] - actor->stats[STAT_STR];
        if (damage >= actor->stats[STAT_STR])
            damage = rand()%5;
        else
            damage = actor->stats[STAT_STR] / 2 - damage / 2;

    }

    /* Randomize it */
    if (damage >= 16) {
        rr = rand()%(damage/16) - damage/32;
        damage += rr;
    }

    /* Apply guard */
    if (actorIsEnemy && options[args[x]] == OPT_GUARD) {
        /* Like a second vitality application */
        if (damage >= target->stats[STAT_VIT]) {
            damage = damage / 2 + (damage - target->stats[STAT_VIT]) / 2;

        } else {
            rr = target->stats[STAT_VIT] - damage;
            if (rr >= damage)
                damage = rand()%5;
            else
                damage = damage / 2 - rr / 2;

        }
    }

    /* Apply damage */
    if (damage > target->hp)
        target->hp = 0;
    else
        target->hp -= damage;

    /* Report it */
    printCursor(WIN_LEFT+2, WIN_TOP+2);
    loadName(actor->spriteNum);
    printStr(strBuf);
    printStr(" attacks ");
    loadName(target->spriteNum);
    printStr(strBuf);

    printCursor(WIN_LEFT+4, WIN_TOP+4);
    printNum(damage);
    printStr(" damage!");

    if (!target->hp) {
        printCursor(WIN_LEFT+2, WIN_TOP+7);
        printStr(strBuf);
        printStr(" dies!");
    }

    waitjoy();
}

static void guard()
{
    /* Just report it, nothing else to do here */
    printCursor(WIN_LEFT+2, WIN_TOP+2);
    loadName(actor->spriteNum);
    printStr(strBuf);
    printStr(" is on guard");

    waitjoy();
}

static char flee()
{
    char success = 0;
    if (!battleConfig.inescapable) {
        /* We at least *might* escape */
        /* ... eventually */
    }

    printCursor(WIN_LEFT+2, WIN_TOP+2);
    loadName(actor->spriteNum);
    printStr(strBuf);
    printStr(" attempts to flee!");

    printCursor(WIN_LEFT+4, WIN_TOP+4);
    printStr("Failed!");

    waitjoy();

    return success;
}


#pragma data-name (push, "JUMPS")
void *jb1 = (void *) battlePrime;
#pragma data-name (pop)
