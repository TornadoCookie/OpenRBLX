#ifndef _LIGHTING_
#define _LIGHTING_

#include "instance.h"
#include "color3.h"
#include <raymath.h>

typedef struct Lighting {
    Instance instance;

    Color3 Ambient;
    float Brightness;
    Color3 ColorShift_Bottom;
    Color3 ColorShift_Top;
    float GeographicLatitude;
    char *TimeOfDay;

    RBXScriptSignal *LightingChanged;
} Lighting;

Lighting *Lighting_new(Instance *parent);

double Lighting_GetMinutesAfterMidnight(Lighting *this);
Vector3 Lighting_GetMoonDirection(Lighting *this);
float Lighting_GetMoonPhase(Lighting *this);
Vector3 Lighting_GetSunDirection(Lighting *this);
void Lighting_SetMinutesAfterMidnight(Lighting *this, double minutes);

#endif
