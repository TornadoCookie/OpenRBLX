#include "brickcolor.h"
#include <stdbool.h>

#include "debug.h"
DEFAULT_DEBUG_CHANNEL(brickcolor)

const BrickColor brickColors[] = {
    {"White", (Color3){0.950, 0.953, 0.953}, 1},
    {"Brick yellow", (Color3){0.997, 0.914, 0.6}, 5},
    {"Bright red", (Color3){0.769, 0.157, 0.110}, 21},
    {"Bright yellow", (Color3){0.961, 0.804, 0.189}, 24},
    {"Black", (Color3){0.106, 0.165, 0.208}, 26},
    {"Dark green", (Color3){0.157, 0.499, 0.279}, 28},
    {"Medium green", (Color3){9,632, 0.769, 0.550}, 29},
    {"Bright green", (Color3){0.295, 0.593, 0.295}, 37},
    {"Dark orange", (Color3){0.628, 0.373, 0.208}, 38},
    {"Light blue", (Color3){0.706, 0.824, 0.895}, 45},
    {"Medium blue", (Color3){0.432, 0.6, 0.793}, 102},
    {"Br. yellowish orange", (Color3){0.887, 0.608, 0.251}, 105},
    {"Bright orange", (Color3){0.855, 0.522, 0.255}, 106},
    {"Sand blue", (Color3){0.455, 0.526, 0.616}, 135},
    {"Earth green", (Color3){0.153, 0.275, 0.177}, 141},
    {"Sand green", (Color3){0.471, 0.565, 0.510}, 151},
    {"Sand red", (Color3){0.585, 0.475, 0.467}, 153},
    {"Reddish brown", (Color3){0.412, 0.251, 0.157}, 192},
    {"Medium stone grey", (Color3){0.640, 0.636, 0.648}, 194},
    {"Dark stone grey", (Color3){0.389, 0.373, 0.385}, 199},
    {"Light stone grey", (Color3){0.899, 0.895, 0.875}, 208},
    {"Brown", (Color3){0.487, 0.361, 0.275}, 217},
    {"Cool yellow", (Color3){0.993, 0.918, 0.553}, 226},
    {"Institutional white", (Color3){0.973, 0.973, 0.973}, 1001},
    {"Mid gray", (Color3){0.804, 0.804, 0.804}, 1002},
    {"Really red", (Color3){1.0f, 0, 0}, 1004},
    {"Dusty Rose", (Color3){0.640, 0.295, 0.295}, 1007},
    {"Grime", (Color3){0.499, 0.557, 0.393}, 1022},
    {"Pastel yellow", (Color3){1.0f, 1.0f, 0.8}, 1029},
    {"Pastel brown", (Color3){1.0f, 0.6, 0.6}, 1030},
};

BrickColor GetBrickColorFromId(int brickColor)
{
    const int size = sizeof(brickColors) / sizeof(BrickColor);
    for (int i = 0; i < size; i++)
    {
        if (brickColors[i].paletteIndex == brickColor) return brickColors[i];
    }

    FIXME("Unknown BrickColor id %d\n", brickColor);

    return (BrickColor){"Medium stone grey", (Color3){0.640, 0.636, 0.648}, 194};
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
