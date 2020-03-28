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

/* Info for a drawable sprite */
struct SpriteInfo {
    char name[STR_BUF_SZ], pal[3], bank, idx;
};

/* Info on statistic names for playable characters */
struct StatNames {
    char names[STAT_CT][STR_BUF_SZ];
};

/* Info for a loadable character (includes enemies), aside from the sprite */
struct CharacterInfo {
    uint hp, mp, stats[STAT_CT];
    const struct StatNames *statNames;
};

extern const struct SpriteInfo *spriteTable[];
extern const struct CharacterInfo *characterTable[];

/* Load into a sprite */
char cdecl loadSpriteIdxPrime(char idx)
{
    const struct SpriteInfo *sprite;
    sprite = spriteTable[idx];
    return loadSprite(sprite->pal, sprite->bank, sprite->idx);
}

/* Load into a character slot */
void cdecl loadCharacterPrime(char to, char from)
{
    const struct CharacterInfo *ch;
    const struct SpriteInfo *sprite;
    ch = characterTable[from];
    sprite = spriteTable[from];
    characters[to].spriteNum = from;
    characters[to].hp = characters[to].maxHP = ch->hp;
    characters[to].mp = characters[to].maxMP = ch->mp;
    memcpy(characters[to].stats, ch->stats, sizeof(ch->stats));
}

/* Load a sprite's name into the string buffer */
void cdecl loadNamePrime(char idx)
{
    memcpy(strBuf, spriteTable[idx]->name, STR_BUF_SZ);
}

/* Load a stat name into the string buffer */
void cdecl loadStatNamePrime(char idx, char st)
{
    memcpy(strBuf, characterTable[idx]->statNames->names[st], STR_BUF_SZ);
}


/* The sprites themselves */
#define SA(id, aof, name, p1, p2, p3) \
const struct SpriteInfo id = {name, {0x ## p1, 0x ## p2, 0x ## p3}, CHR_ ## aof ## _BANK, CHR_ ## aof}
#define S(id, name, p1, p2, p3) SA(id, id, name, p1, p2, p3)
S(abel,  "Abel",        f,  f,  8);
S(cain,  "Cain",        f, 16,  8);
S(cyrus, "Cyrus",       f,  2, 28);
S(confederate,
         "Confederate", f, 2d, 38);
S(cyrus_oneleg,
         "Cyrus",       f,  2, 28);
S(every_woman, "Female character",
                       30, 17, 38);
SA(adam, abel, "Adam",
                        f,  1, 38);
SA(charles, cain, "Charles",
                        f, 16, 38);

/* And the characters themselves */
#define C(id, hp, mp, s1, s2, s3, s4) \
const struct CharacterInfo id ## C = {hp, mp, {s1, s2, s3, s4}, NULL}

#define CF(id, hp, mp, s1, s2, s3, s4, sn1, sn2, sn3, sn4) \
const struct StatNames id ## N = {{sn1, sn2, sn3, sn4}}; \
const struct CharacterInfo id ## C = {hp, mp, {s1, s2, s3, s4}, &id ## N}

CF(abel,    100,    0,      100,     40,    100,     94,
            "Love for his Lord God",
            "Prowess with sheep",
            "Original sin",
            "Love for Cain");
C(cain,     10000,  10000,  500,    500,    500,    500);
CF(cyrus,   173,    0,       86,      2,      1,     43,
            "Love for his country",
            "Number of legs",
            "Number of children",
            "Love for children");
C(confederate, 10000, 10000,500,    500,    500,    500);
CF(cyrus_oneleg, 173,0,      47,      1,      1,     39,
            "Love for his country",
            "Number of legs",
            "Number of children",
            "Love for children");
CF(adam, 1, 1, 1, 1, 1, 1,
            "A",
            "B",
            "C",
            "D");


#pragma data-name (push, "JUMPS")
void *jc1 = (void *) loadSpriteIdxPrime;
void *jc2 = (void *) loadCharacterPrime;
void *jc3 = (void *) loadNamePrime;
void *jc4 = (void *) loadStatNamePrime;
#pragma data-name (pop)
