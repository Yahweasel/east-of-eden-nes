#include <string.h>

#include "lib.h"

void clearSprites()
{
    memset(oamSprites, -1, sizeof(oamSprites));
    memset(chrSprites, 0, sizeof(chrSprites));
    memset(spriteLocs, -1, sizeof(spriteLocs));
    memset(paletteTarget + 0x11, -1, sizeof(paletteTarget) - 0x11);
}
