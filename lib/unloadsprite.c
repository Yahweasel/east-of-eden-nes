#include <string.h>

#include "lib.h"

void unloadSprite(char idx)
{
    memset(spriteLocs + idx, -1, sizeof(struct SpriteLoc));
    idx *= 4;
    memset(oamSprites + idx, -1, sizeof(struct OAMSprite) * 4);
}
