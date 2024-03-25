#ifndef _BRICKCOLOR_
#define _BRICKCOLOR_

#include "color3.h"

typedef struct BrickColor {
    const char *name;
    Color3 color;
    int paletteIndex;
} BrickColor;

BrickColor GetBrickColorFromId(int brickColor);
BrickColor GetBrickColorFromColor(Color3 color);

#endif
