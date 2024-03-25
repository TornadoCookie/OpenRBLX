#include "brickcolor.h"
#include <stdbool.h>

const BrickColor brickColors[] = {
    {"Really red", (Color3){1.0f, 0, 0}, 1004}
};

BrickColor GetBrickColorFromId(int brickColor)
{
    const int size = sizeof(brickColors) / sizeof(BrickColor);
    for (int i = 0; i < size; i++)
    {
        if (brickColors[i].paletteIndex == brickColor) return brickColors[i];
    }

    return (BrickColor){"Medium stone grey", (Color3){163, 162, 165}, 194};
}

static bool ceq(Color3 a, Color3 b)
{
    return a.R == b.R && a.G == b.G && a.B == b.B;
}

BrickColor GetBrickColorFromColor(Color3 color)
{
    const int size = sizeof(brickColors) / sizeof(BrickColor);
    for (int i = 0; i < size; i++)
    {
        if (ceq(color, brickColors[i].color)) return brickColors[i];
    }

    return (BrickColor){"Unknown", color, -1};
}
