#include <raylib.h>
#include "lighting.h"
#include "debug.h"
#include <stdlib.h>

DEFAULT_DEBUG_CHANNEL(lighting)

Lighting *Lighting_new(const char *className, Instance *parent)
{
    Lighting *newInst = Instance_new("Lighting", parent);

    newInst->instance.DataCost = sizeof(Lighting);
    newInst = realloc(newInst, sizeof(Lighting));
    
    newInst->LightingChanged = RBXScriptSignal_new();
    newInst->instance.Name = "Lighting";

    if (parent) Instance_SetParent(newInst, parent);

    return newInst;
}

double Lighting_GetMinutesAfterMidnight(Lighting *this)
{
    FIXME("this %p stub!\n", this);
    return 0.0;
}

Vector3 Lighting_GetMoonDirection(Lighting *this)
{
    FIXME("this %p stub!\n", this);
    return (Vector3){0, 0, 0};
}

float Lighting_GetMoonPhase(Lighting *this)
{
    FIXME("this %p stub!\n", this);
    return 0.0f;
}

Vector3 Lighting_GetSunDirection(Lighting *this)
{
    FIXME("this %p stub!\n", this);
    return (Vector3){0, 0, 0};
}

void Lighting_SetMinutesAfterMidnight(Lighting *this, double minutes)
{
    FIXME("this %p, minutes %lf stub!\n", this, minutes);
}

void Lighting_draw(Lighting *this)
{
    
}

void serialize_Lighting(Lighting *lighting, SerializeInstance *inst)
{
    serialize_Instance(lighting, inst);

    serialize_atomic(Color3, lighting, Ambient);
    serialize_atomic(float, lighting, Brightness);
    serialize_atomic(Color3, lighting, ColorShift_Bottom);
    serialize_atomic(Color3, lighting, ColorShift_Top);
    serialize_atomic(float, lighting, GeographicLatitude);
    serialize_atomic(string, lighting, TimeOfDay);
}
