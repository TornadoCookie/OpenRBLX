#include "camera.h"
#include "debug.h"
#include <stdlib.h>
#include "datamodel.h"

DEFAULT_DEBUG_CHANNEL(camera)

Camera_Instance *Camera_new(const char *className, Instance *parent)
{
    Camera_Instance *newInst = Instance_new("Camera", parent);

    newInst->instance.DataCost = sizeof(Camera_Instance);
    newInst = realloc(newInst, sizeof(Camera_Instance));

    newInst->CameraType = CameraType_Custom;
    newInst->camera = (Camera){ 0 };
    newInst->camera.position = (Vector3){ 0.0f, 2.0f, 4.0f };    // Camera position
    newInst->camera.target = (Vector3){ 0.0f, 2.0f, 0.0f };      // Camera looking at point
    newInst->camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    newInst->camera.fovy = 60.0f;                                // Camera field-of-view Y
    newInst->camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type

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
    FIXME("this %p stub!, %f\n", this, this->camera.position.y);
    UpdateCamera(&this->camera, CAMERA_FREE);
}

void serialize_Camera(Camera_Instance *camera, SerializeInstance *inst)
{
    DataModel *datamodel = GetDataModel();
    camera = datamodel->Workspace->CurrentCamera;

    serialize_Instance(camera, inst);

    camera->instance.ClassName = "Camera";

    serialize_atomic(Ref, camera, CameraSubject);
    serialize_atomic(token, camera, CameraType);
    serialize_atomic(CoordinateFrame, camera, CoordinateFrame);
    serialize_atomic(CoordinateFrame, camera, Focus);
}
