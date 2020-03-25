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

/* Screen workings:
 * At load time, the bottom part of 0x2000 is always
 * scrollXScreen-scrollYScreen, which is in turn either the screen we're
 * on, or the screen to the left or above it, i.e., the top-left of our
 * view. 0x2400 is the screen to right of that. We use vertical mirroring,
 * so if we're scrolled in Y at all, then the top part of both screens is
 * the screen down from that. As we move in any direction, we load single
 * tiles in to account for the movement. The seam in the horizontal
 * direction is 8 tiles away from our current location. If that's in the
 * top-left scrolled screen, flipCur is true; else, flipCur is false. The
 * flip point is scrollXTH */

#include <conio.h>
#include <nes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib.h"
#include "overworld.h"

extern struct Overworld *jumpTable[];

/* Make these global to avoid stack */
static char x, y, xs, ys, *tile, *nta, ntaBase, v, v2, scrollXTH, flipCur;
static uint addr, attrAddr, base, attrBase;

/* The location of some world subtables depends on its size, so store the
 * subtable pointers directly */
static char *worldAttrs;
static struct OverworldSprite *worldSprites;
static struct WorldAction *worldAct;

#define SCREEN_SIZE     (SCREEN_WIDTH*SCREEN_HEIGHT)
#define UPDATE_SCROLLXTH() do { \
    scrollXTH=scrollXT^(SCREEN_WIDTH/2); \
    flipCur = (scrollXT>=SCREEN_WIDTH/2)*2; \
} while (0)

#define UPDATE_SUBTABLES() do { \
    worldAttrs = world->d + SCREEN_SIZE * world->w * world->h; \
    worldSprites = (struct OverworldSprite *) (worldAttrs + SCREEN_SIZE / 4 * world->w * world->h); \
    worldAct = (struct WorldAction *) (worldSprites + WORLD_SPRITES); \
} while (0)

static void center()
{
    char sx, sy, xi;

    scrollXScreen = pos.xscreen;
    scrollYScreen = pos.yscreen;

    /* Try to make the character's position centered */
    sx = pos.xt - (SCREEN_WIDTH-1)/2;
    xi = 0;
    sy = pos.yt - SCREEN_HEIGHT/2;

    /* Now rectify any edge-of-screen problems */
    if (pos.xscreen == 0 && sx >= 128)
        sx = 0;
    if (pos.xscreen >= world->w - 1 && sx < 128)
        sx = 0;
    if (pos.yscreen == 0 && sy >= 128)
        sy = 0;
    if (pos.yscreen >= world->h - 1 && sy < 128)
        sy = 0;

    /* Then turn that into an actual scroll position */
    if (sx >= 128) {
        scrollXScreen--;
        sx += SCREEN_WIDTH;
    }
    if (sy >= 128) {
        scrollYScreen--;
        sy += SCREEN_HEIGHT;
    }
    scrollXT = sx;
    scrollXC = xi;
    scrollYT = sy;
    scrollXS = scrollYS = 0;
}

/* Transfer the given screen into PPU memory, address already sent */
static void transferScreen(char tx, char ty, char flip)
{
    tile = world->d + (world->w * SCREEN_SIZE * ty) + (SCREEN_SIZE * tx);
    if (flip)
        flip = scrollXTH;
    else
        flip = -1;

    /* Screen data */
    for (y = 0; y < SCREEN_HEIGHT; y++) {
        /* At the scroll point, we need to switch to the screen above */
        if (y == scrollYT)
            tile -= (world->w * SCREEN_SIZE);

        /* Transfer row 1 */
        for (x = 0; x < SCREEN_WIDTH; x++) {
            /* At the flip point, we need to switch to the screen to the left */
            if (x == flip)
                tile -= SCREEN_SIZE * 2;
            v = *tile & 0xfc;
            PPU.vram.data = v;
            PPU.vram.data = v+1;

            *nta >>= 2;
            *nta |= (*tile & 0x3) << 6;
            if (x&1)
                nta++;

            tile++;
        }
        if (flip < 128)
            tile += SCREEN_SIZE * 2;

        /* Transfer row 2 */
        tile -= SCREEN_WIDTH;
        for (x = 0; x < SCREEN_WIDTH; x++) {
            if (x == flip)
                tile -= SCREEN_SIZE * 2;
            v = (*tile++) & 0xfc;
            PPU.vram.data = v+2;
            PPU.vram.data = v+3;
        }
        if (flip < 128)
            tile += SCREEN_SIZE * 2;

        /* Get ready for the next attribute row */
        if (!(y&1))
            nta -= SCREEN_WIDTH/2;
    }

    /* The last attribute row is off by one (row) */
    for (x = 0; x < SCREEN_WIDTH; x += 2) {
        *nta >>= 4;
        nta++;
    }

    /* Attributes (palette) */
    nta -= 64;
    for (y = 0; y < 64; y++)
        PPU.vram.data = *nta++;
}

#if 0
/* Transfer the visible screen from line `from` into PPU memory via the buffer */
static void transferVisibleScreen(char from)
{
    /* Start at the 'from' position */
    x = scrollXT;
    y = scrollYT + from;
    if (y >= SCREEN_HEIGHT)
        y -= SCREEN_HEIGHT;

    tile = world->d + (world->w * SCREEN_SIZE * scrollYScreen) +
                      (SCREEN_SIZE * scrollXScreen) +
                      (y * SCREEN_WIDTH) +
                      x;
    addr = 0x2000 + scrollXC*0x400 +
                      (y * SCREEN_WIDTH * 4) +
                      (x * 2);


    /* Now draw */
    for (ym = from; ym < SCREEN_HEIGHT; ym++) {
        /* Row 1 */
        for (xm = 0; xm < SCREEN_WIDTH; xm++) {
            v = (*tile++) & 0xfc;
            ppubuf_put(addr++, v);
            ppubuf_put(addr++, v+1);

            if (++x >= SCREEN_WIDTH) {
                /* Advance to the next screen */
                x = 0;
                tile += SCREEN_SIZE;
                addr ^= 0x400;
                addr -= SCREEN_WIDTH * 2;
            }
        }
        tile -= SCREEN_WIDTH + SCREEN_SIZE;
        addr ^= 0x400;
        addr -= SCREEN_WIDTH * 2;

        /* Row 2 */
        for (xm = 0; xm < SCREEN_WIDTH; xm++) {
            v = (*tile++) & 0xfc;
            ppubuf_put(addr++, v+2);
            ppubuf_put(addr++, v+3);

            if (++x >= SCREEN_WIDTH) {
                /* Advance to the next screen */
                x = 0;
                tile += SCREEN_SIZE;
                addr ^= 0x400;
                addr -= SCREEN_WIDTH * 2;
            }
        }
        tile -= SCREEN_SIZE;

        /* Advance to the next row */
        if (++y >= SCREEN_HEIGHT) {
            y = 0;
            tile += (world->w-1) * SCREEN_SIZE;
        }
        addr = 0x2000 + scrollXC*0x400 +
            (y * SCREEN_WIDTH * 4) +
            scrollXT * 2;
    }
}
#endif

/* Transfer a single column into PPU memory, address already set */
static void transferCol(char tx, char ty, char off)
{
    char right = ((scrollXT&1) ? 2 : 0);
    tile = world->d + (world->w * SCREEN_SIZE * ty) + (SCREEN_SIZE * tx) + scrollXTH;

    for (y = 0; y < SCREEN_HEIGHT; y++) {
        /* Flip at the scroll point */
        if (y == scrollYT)
            tile -= (world->w * SCREEN_SIZE);

        /* Tile */
        v = *tile & 0xfc;
        ppubuf_put(addr, v+off);
        addr += SCREEN_WIDTH*2;
        ppubuf_put(addr, v+off+2);
        addr += SCREEN_WIDTH*2;

        /* Attribute */
        v = right + ((y&1)?4:0);
        *nta &= ~(0x3 << v);
        *nta |= (*tile & 0x3) << v;

        /* Flush on odd rows */
        if ((y&1) || y == SCREEN_HEIGHT - 1) {
            ppubuf_put(attrAddr, *nta);
            attrAddr += SCREEN_WIDTH/2;
            nta += SCREEN_WIDTH/2;
        }

        tile += SCREEN_WIDTH;
    }
}

/* Transfer a single row into PPU memory, address already set */
static void transferRow(char tx, char ty, char flip, char off)
{
    char bot = ((scrollYT&1) ? 4 : 0);
    tile = world->d + (world->w * SCREEN_SIZE * ty) + (SCREEN_SIZE * tx) + (SCREEN_WIDTH * scrollYT);
    if (flip)
        flip = scrollXTH;
    else
        flip = -1;

    for (x = 0; x < SCREEN_WIDTH; x++) {
        if (x == flip)
            tile -= SCREEN_SIZE * 2;

        /* Tile */
        v = *tile & 0xfc;
        ppubuf_put(addr++, v+off);
        ppubuf_put(addr++, v+off+1);

        /* Attribute */
        v = ((x&1)?2:0) + bot;
        *nta &= ~(0x3 << v);
        *nta |= (*tile & 0x3) << v;

        /* Flush on odd columns */
        if (x&1)
            ppubuf_put(attrAddr++, *nta++);

        tile++;
    }
}

/* Scroll just the character sprite */
static void scrollSprite()
{
#define SL spriteLocs[0]
    SL.status = pos.dir | ((tickcount>>3)&0x3);
    SL.xs += pos.xm;
    if (SL.xs >= 128) {
        SL.xt--;
        if (SL.xt >= 128) {
            SL.xscreen--;
            SL.xt = SCREEN_WIDTH - 1;
        }
        SL.xs = 15;
    } else if (SL.xs >= 16) {
        SL.xt++;
        if (SL.xt >= SCREEN_WIDTH) {
            SL.xscreen++;
            SL.xt = 0;
        }
        SL.xs = 0;
    }
    SL.ys += pos.ym;
    if (SL.ys >= 128) {
        SL.yt--;
        if (SL.yt >= 128) {
            SL.yscreen--;
            SL.yt = SCREEN_HEIGHT - 1;
        }
        SL.ys = 15;
    } else if (SL.ys >= 16) {
        SL.yt++;
        if (SL.yt >= SCREEN_HEIGHT) {
            SL.yscreen++;
            SL.yt = 0;
        }
        SL.ys = 0;
    }
#undef SL
    updateSprite(0);
}

/* Scroll the screen to move the character */
static void scrollScreen()
{
    static char i;

    /* Update scroll values if we're scrolling left/up (so *S is always positive) */
    if (pos.xm >= 128) {
        scrollXT--;
        if (scrollXT >= 128) {
            scrollXScreen--;
            scrollXT = SCREEN_WIDTH - 1;
            scrollXC = !scrollXC;
        }
        UPDATE_SCROLLXTH();
        scrollXS = 16;
    }
    if (pos.ym >= 128) {
        scrollYT--;
        if (scrollYT >= 128) {
            scrollYScreen--;
            scrollYT = SCREEN_HEIGHT - 1;
        }
        scrollYS = 16;
    }

    /* Base address for mid-scroll updates */
    if (pos.xm) {
        if (scrollXTH >= 8) {
            base = 0x2000 + (!scrollXC)*0x400 + 2 * scrollXTH;
            attrBase = 0x23c0 + (!scrollXC)*0x400 + scrollXTH / 2;
            ntaBase = (!scrollXC)*64 + scrollXTH / 2;
        } else {
            base = 0x2000 + scrollXC*0x400 + 2 * scrollXTH;
            attrBase = 0x23c0 + scrollXC*0x400 + scrollXTH / 2;
            ntaBase = scrollXC*64 + scrollXTH / 2;
        }
    } else if (pos.ym) {
        base = 0x2000 + scrollXC*0x400 + (SCREEN_WIDTH * 4 * scrollYT);
        attrBase = 0x23c0 + scrollXC*0x400 + (SCREEN_WIDTH/2 * (scrollYT/2));
        ntaBase = scrollXC*64 + (SCREEN_WIDTH/2 * (scrollYT/2));
    }

    /* Perform the actual scroll */
    for (i = 0; i < 16; i++) {
        scrollXS += pos.xm;
        scrollYS += pos.ym;

        /* Update the sprites */
        scrollSprite();
        for (x = 1; x < FULL_SPRITES; x++) {
            if (spriteLocs[x].xscreen == 255) break;
            updateSprite(x);
        }

        /* Wait for the frame */
        if (!(i&1))
            waitvsync();

        /* Transfer PPU data as well */
        if (!(i&0x7)) {
            /* Transfer the column of the correct screen if needed */
            if (pos.xm >= 128) {
                /* We moved left, so transfer in a column of our own screen or the screen to our left */
                v2 = (i^8)/8;
                addr = base + v2;
                attrAddr = attrBase;
                nta = nametableAttributes + ntaBase;
                transferCol(scrollXScreen - (scrollXT<8), scrollYScreen + 1, v2);

            } else if (pos.xm) {
                /* We moved right, so transfer in a column the screen one or two to our right */
                v2 = i/8;
                addr = base + v2;
                attrAddr = attrBase;
                nta = nametableAttributes + ntaBase;
                transferCol(scrollXScreen + 1 + (scrollXT>=8), scrollYScreen + 1, v2);

            }

            /* Now transfer in the row of the correct screen if needed */
            if (pos.ym >= 128) {
                /* We moved up, so transfer in a row of the screen we're on */
                v2 = (i^8)/4;

                addr = base + (SCREEN_WIDTH * v2);
                attrAddr = attrBase;
                nta = nametableAttributes + ntaBase;
                transferRow(scrollXScreen + flipCur, scrollYScreen, flipCur, v2);

                addr = (base^0x400) + (SCREEN_WIDTH * v2);
                attrAddr = attrBase^0x400;
                nta = nametableAttributes + (ntaBase^64);
                transferRow(scrollXScreen + 1, scrollYScreen, !flipCur, v2);

            } else if (pos.ym) {
                /* We moved down, so transfer in a row of the screen below */
                v2 = i/4;

                addr = base + (SCREEN_WIDTH * v2);
                attrAddr = attrBase;
                nta = nametableAttributes + ntaBase;
                transferRow(scrollXScreen + flipCur, scrollYScreen + 1, flipCur, v2);

                addr = (base^0x400) + (SCREEN_WIDTH * v2);
                attrAddr = attrBase^0x400;
                nta = nametableAttributes + (ntaBase^64);
                transferRow(scrollXScreen + 1, scrollYScreen + 1, !flipCur, v2);
            }
        }

    }

    /* Update scrolling attributes right/down */
    if (pos.xm && pos.xm < 128) {
        scrollXT++;
        if (scrollXT >= SCREEN_WIDTH) {
            scrollXScreen++;
            scrollXT = 0;
            scrollXC = !scrollXC;
        }
        UPDATE_SCROLLXTH();
        scrollXS = 0;
    }
    if (pos.ym && pos.ym < 128) {
        scrollYT++;
        if (scrollYT >= SCREEN_HEIGHT) {
            scrollYScreen++;
            scrollYT = 0;
        }
        scrollYS = 0;
    }
    finvsync();
}

/* Scroll the character's position */
static void scrollChar()
{
    for (y = 0; y < 16; y++) {
        if (!(y&1))
            waitvsync();

        /* Update the sprite */
        scrollSprite();

        for (x = 0; x < 4; x++) {
            oamSprites[x].x += pos.xm;
            oamSprites[x].y += pos.ym;
        }
    }
}

/* Get the attributes of the tile we're looking at into xs, x, ys, y, v */
static void getTileAttrs()
{
    xs = pos.xscreen;
    ys = pos.yscreen;
    x = pos.xt + pos.xm;
    y = pos.yt + pos.ym;
    if (x >= 128) {
        x += SCREEN_WIDTH;
        xs--;
    } else if (x >= SCREEN_WIDTH) {
        x -= SCREEN_WIDTH;
        xs++;
    }
    if (y >= 128) {
        y += SCREEN_HEIGHT;
        ys--;
    } else if (y >= SCREEN_HEIGHT) {
        y -= SCREEN_HEIGHT;
        ys++;
    }
    v = worldAttrs[world->w * SCREEN_SIZE/4 * ys +
                   SCREEN_SIZE/4 * xs +
                   SCREEN_WIDTH/4 * y +
                   x/4] >> ((x&3)*2);
}

/* Perform an action associated with the given tile */
static char performTileAction(char xscreen, char yscreen, char xt, char yt)
{
    static struct WorldAction *act;
    for (act = worldAct; act->bank; act++) {
        if (act->xscreen == xscreen &&
            act->yscreen == yscreen &&
            act->xt == xt &&
            act->yt == yt) {
            /* Perform this action */
            return gfarcall0(act->bank, act->idx);
        }
    }
    return 1;
}

/* Perform an action associated with the given sprite */
static char performSpriteAction(char idx)
{
    struct OverworldSprite *sprite = worldSprites + idx;
    if (sprite->actBank)
        return gfarcall1(idx+1, sprite->actBank, sprite->actIdx);
    return 1;
}

/* The actual overworld behavior */
void cdecl overworldPrime(char idx)
{
    static char i, inp;

    world = jumpTable[idx];

    /* Figure out how we ought to be centered */
    center();

    /* Load in the world */
    overworldFullReload();
    fadein();

    /* Now take overworld input */
    while (1) {
        waitvsync();
        inp = readjoy();
        if (inp & JOY_DPAD) {
            /* Get the direction */
            pos.xm = pos.ym = 0;
            if (inp & JOY_UP) {
                pos.ym = -1;
                pos.dir = SPRITE_UP;
            } else if (inp & JOY_DOWN) {
                pos.ym = 1;
                pos.dir = SPRITE_DOWN;
            } else if (inp & JOY_LEFT) {
                pos.xm = -1;
                pos.dir = SPRITE_LEFT;
            } else if (inp & JOY_RIGHT) {
                pos.xm = 1;
                pos.dir = SPRITE_RIGHT;
            }

            /* Figure out what tile we're going into */
            getTileAttrs();

            if (v & TILE_PASSABLE) {
                /* Check if there's a sprite in the way */
                for (v2 = 1; v2 < FULL_SPRITES; v2++) {
                    struct SpriteLoc *spriteLoc = spriteLocs + v2;
                    if (spriteLoc->xscreen == 255) break;
                    if (spriteLoc->xscreen == xs &&
                        spriteLoc->yscreen == ys &&
                        spriteLoc->xt == x &&
                        spriteLoc->yt == y) {
                        v &= ~(TILE_PASSABLE);
                        break;
                    }
                }
            }

            if (v & TILE_PASSABLE) {
                /* OK, we're allowed to move this way, so do so */
                pos.xscreen = xs;
                pos.yscreen = ys;
                pos.xt = x;
                pos.yt = y;

                y -= scrollYT;
                y += (ys-scrollYScreen)*SCREEN_HEIGHT;
                x -= scrollXT;
                x += (xs-scrollXScreen)*SCREEN_WIDTH;

                /* We store whether to scroll the screen (1) or just character (0) in v2 */
                v2 = 0;

                /* FIXME: Make the distance from screen edge to scroll not a magic number */
                if (pos.ym == 255 && y < 5 && (scrollYScreen || scrollYT))
                    v2 = 1;
                else if (pos.ym == 1 && y >= SCREEN_HEIGHT - 5 && scrollYScreen < world->h - 1)
                    v2 = 1;
                else if (pos.xm == 255 && x < 5 && (scrollXScreen || scrollXT))
                    v2 = 1;
                else if (pos.xm == 1 && x >= SCREEN_WIDTH - 5 && scrollXScreen < world->w - 1)
                    v2 = 1;

                /* Scrolling can lose global temporaries, so keep v */
                i = v;

                if (v2)
                    scrollScreen();
                else
                    scrollChar();

                /* Was there an action associated with this tile? */
                if (i & TILE_ACTION) {
                    if (!performTileAction(pos.xscreen, pos.yscreen, pos.xt, pos.yt)) {
                        /* Don't continue in this overworld */
                        return;
                    }
                    UPDATE_SUBTABLES();
                    while (readjoy());
                }

            } else {
                spriteLocs[0].status = pos.dir;
                updateSprite(0);

            }

        } else {
            /* If we're not moving, make sure the sprite is still */
            spriteLocs[0].status = pos.dir;
            updateSprite(0);

        }

        if (inp & JOY_A) {
            getTileAttrs();

            /* Perform an action associated with the target tile, if any */
            if ((i & (TILE_PASSABLE|TILE_ACTION)) == TILE_ACTION) {
                if (!performTileAction(xs, ys, x, y))
                    return;
                UPDATE_SUBTABLES();
                while (readjoy());

            } else {
                /* There could be a sprite there */
                for (i = 1; i < FULL_SPRITES; i++) {
                    struct SpriteLoc *spriteLoc = spriteLocs + i;
                    if (spriteLoc->xscreen == xs &&
                        spriteLoc->yscreen == ys &&
                        spriteLoc->xt == x &&
                        spriteLoc->yt == y) {
                        spriteLoc->status = (pos.dir^0x10);
                        updateSprite(i);
                        if (!performSpriteAction(i-1))
                            return;
                        UPDATE_SUBTABLES();
                        while (readjoy());
                    }
                }

            }

        } else if (inp & JOY_START) {
            /* Menu */
            menu();
            UPDATE_SUBTABLES();
            while (readjoy());

        }
    }

    while (1);
}

void cdecl overworldReloadPrime(char from)
{
    char dft;

    /* Transfer the current screen into PPU memory */
    UPDATE_SUBTABLES();
    UPDATE_SCROLLXTH();
    /*transferVisibleScreen(11);*/
    (void) from;

    dft = fadetime;
    fadetime = 0;
    fadeout();

    /* Restore the background */
    ppu_direct();
    vram_addr(0x2000 + scrollXC*0x400);
    nta = nametableAttributes + scrollXC*64;
    transferScreen(scrollXScreen + flipCur, scrollYScreen + 1, flipCur);
    vram_addr(0x2000 + (!scrollXC)*0x400);
    nta = nametableAttributes + (!scrollXC)*64;
    transferScreen(scrollXScreen + 1, scrollYScreen + 1, !flipCur);
    ppu_enddirect();

    /* Restore the sprites */
    for (x = 0; x < FULL_SPRITES; x++)
        updateSprite(x);

    fadein();
    fadetime = dft;
}

void cdecl overworldFullReloadPrime()
{
    UPDATE_SUBTABLES();
    UPDATE_SCROLLXTH();

    /* Load in the palette */
    memcpy(paletteTarget, world->pal, 0x10);

    /* Reset the sprite info */
    clearSprites();

    ppu_direct();

    /* Load in the background CHR set */
    x = world->chrBank;
    y = world->chrIdx;
    for (v = 8; v < 16; v++)
        loadChr(v, x, y++);

    /* Load in the character sprite */
    loadSpriteIdx(characters[0].spriteNum);
    spriteLocs[0].xscreen = pos.xscreen;
    spriteLocs[0].yscreen = pos.yscreen;
    spriteLocs[0].xt = pos.xt;
    spriteLocs[0].yt = pos.yt;
    spriteLocs[0].status = pos.dir;
    updateSprite(0);

    /* And all the other sprites */
    for (v = 0; v < WORLD_SPRITES; v++) {
        struct OverworldSprite *sprite = worldSprites + v;
        if (sprite->xscreen == 255) break;
        x = loadSpriteIdx(sprite->spriteIdx);
        if (x == 255) break;

        spriteLocs[x].xscreen = sprite->xscreen;
        spriteLocs[x].yscreen = sprite->yscreen;
        spriteLocs[x].xt = sprite->xt;
        spriteLocs[x].yt = sprite->yt;
        spriteLocs[x].status = sprite->spriteStatus;
        updateSprite(x);
    }

    /* Transfer the initial screen into PPU memory */
    vram_addr(0x2000 + scrollXC*0x400);
    nta = nametableAttributes;
    transferScreen(scrollXScreen + flipCur, scrollYScreen+1, flipCur);
    vram_addr(0x2000 + (!scrollXC)*0x400);
    nta = nametableAttributes + 64;
    transferScreen(scrollXScreen + 1, scrollYScreen+1, !flipCur);

    ppu_enddirect();
}

/* Jump table for overworld banks */
#pragma data-name ("JUMPS")
#define J(x) void *x ## jump = x
J(overworldPrime);
J(overworldReloadPrime);
J(overworldFullReloadPrime);
#undef J
