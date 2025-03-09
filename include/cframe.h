#ifndef _CFRAME_
#define _CFRAME_

#include <raylib.h>
#include <raymath.h>

typedef struct CFrame {
    float X, Y, Z,
    R00, R01, R02,
    R10, R11, R12,
    R20, R21, R22;
} CFrame;

typedef CFrame CoordinateFrame;

#endif
