#include <stdio.h>
#include <math.h>

#define PI 3.14159265358979323

int main()
{
    int i;
    for (i = 0; i < 60; i++) {
        double vpos = sin((i/60.0)*PI);
        int ypix = round(vpos * 16);
        int yt = (ypix / 16);
        int ys = (ypix % 16);
        int xpix = i;
        int xt = (xpix / 16);
        int xs = (xpix % 16);
        printf(
            "spriteLocs[pwIdx].yt = %d;\n"
            "spriteLocs[pwIdx].ys = %d;\n"
            "spriteLocs[pwIdx].xt = %d;\n"
            "spriteLocs[pwIdx].xs = %d;\n"
            "updateSprite(pwIdx);\n"
            "pause(1);\n",
            (8-yt), (16-ys), (9+xt), xs);
    }
    return 0;
}
