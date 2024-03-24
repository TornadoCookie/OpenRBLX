#include "camera.h"
#include "debug.h"
#include <stdlib.h>

Camera_Instance *Camera_new(Instance *parent)
{
    Camera_Instance *newInst = Instance_new("Camera", parent);

    newInst->instance.DataCost = sizeof(Camera_Instance);
    newInst = realloc(newInst, sizeof(Camera_Instance));

    newInst->CameraType = CameraType_Custom;
    newInst->camera = (Camera){ 0 };

    if (parent) Instance_SetParent(newInst, parent);

    return newInst;
}

void Camera_PanUnits(Camera_Instance *this, int units)
{
    FIXME("this %p, units %d stub!\n", this, units);
}

void Camera_TiltUnits(Camera_Instance *this, int units)
{
    FIXME("this %p, units %d stub!\n", this, units);
}

bool Camera_Zoom(Camera_Instance *this, float distance)
{
    FIXME("this %p, distance %f stub!\n", this, distance);
}

void Camera_Process(Camera_Instance *this)
{
    FIXME("this %p stub!\n", this);
}
