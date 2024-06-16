#ifndef _CAMERA_
#define _CAMERA_

#include "instance.h"
#include <raylib.h>
#include "cframe.h"

typedef enum {
    CameraType_Fixed,
    CameraType_Attach,
    CameraType_Watch,
    CameraType_Track,
    CameraType_Follow,
    CameraType_Custom,
    CameraType_Scriptable,
} CameraType;

typedef struct Camera_Instance {
    Instance instance;

    Instance *CameraSubject;
    CameraType CameraType;
    CFrame CoordinateFrame;
    CFrame Focus;

    Camera camera;
} Camera_Instance;

Camera_Instance *Camera_new(const char *className, Instance *parent);

void Camera_PanUnits(Camera_Instance *this, int units);
void Camera_TiltUnits(Camera_Instance *this, int units);
bool Camera_Zoom(Camera_Instance *this, float distance);

void Camera_Process(Camera_Instance *this);

#endif
