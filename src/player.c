#include "player.h"
#include "datamodel.h"
#include "part.h"
#include "decal.h"

Player *Player_new(const char *className, Instance *parent)
{
    Player *newInst = Instance_new(className, parent);

    newInst->instance.DataCost = sizeof(Player);
    newInst = realloc(newInst, sizeof(Player));

    if (parent) Instance_SetParent(newInst, parent);

    newInst->Character = NULL;

    return newInst;
}

void Player_LoadCharacter(Player *this)
{
    Model_Instance *mdl = Model_new("Model", GetDataModel()->Workspace);
    mdl->pvinstance.instance.Name = this->instance.Name;

    Part *torso = Part_new("Part", mdl);
    torso->formfactorpart.basepart.pvinstance.instance.Name = "Torso";
    BasePart_SetCFrame(torso, (CFrame){0, 3, 0, -1, 0, 0, 0, 1, 0, 0, 0, -1});
    torso->formfactorpart.basepart.size = (Vector3){2, 2, 1};

    Part *head = Part_new("Part", mdl);
    torso->formfactorpart.basepart.pvinstance.instance.Name = "Head";
    BasePart_SetCFrame(torso, (CFrame){0, 4.5, 0, -1, 0, 0, 0, 1, 0, 0, 0, -1});
    torso->formfactorpart.basepart.size = (Vector3){2, 1, 1};

    Part *leftArm = Part_new("Part", mdl);
    torso->formfactorpart.basepart.pvinstance.instance.Name = "Left Arm";
    BasePart_SetCFrame(torso, (CFrame){1.5, 3, 0, -1, 0, 0, 0, 1, 0, 0, 0, -1});
    torso->formfactorpart.basepart.size = (Vector3){1, 2, 1};

    Part *rightArm = Part_new("Part", mdl);
    torso->formfactorpart.basepart.pvinstance.instance.Name = "Right Arm";
    BasePart_SetCFrame(torso, (CFrame){-1.5, 3, 0, -1, 0, 0, 0, 1, 0, 0, 0, -1});
    torso->formfactorpart.basepart.size = (Vector3){1, 2, 1};

    Part *leftLeg = Part_new("Part", mdl);
    torso->formfactorpart.basepart.pvinstance.instance.Name = "Left Leg";
    BasePart_SetCFrame(torso, (CFrame){0.5, 1, 0, -1, 0, 0, 0, 1, 0, 0, 0, -1});
    torso->formfactorpart.basepart.size = (Vector3){1, 2, 1};

    Part *rightLeg = Part_new("Part", mdl);
    torso->formfactorpart.basepart.pvinstance.instance.Name = "Right Leg";
    BasePart_SetCFrame(torso, (CFrame){-0.5, 1, 0, -1, 0, 0, 0, 1, 0, 0, 0, -1});
    torso->formfactorpart.basepart.size = (Vector3){1, 2, 1};

    mdl->PrimaryPart = head;

    this->Character = mdl;
}

void Player_Move(Player *this, Vector3 walkDirection, bool relativeToCamera)
{
    if (this->Character)
    {
        if (relativeToCamera)
        {
            Camera cam = GetDataModel()->Workspace->CurrentCamera->camera;
            Matrix lookMatrix = MatrixInvert(MatrixLookAt(cam.position, cam.target, cam.up));
            Vector3 axis;
            float angle;
            QuaternionToAxisAngle(QuaternionFromMatrix(lookMatrix), &axis, &angle);
            walkDirection = Vector3RotateByAxisAngle(walkDirection, axis, angle);
        }
        Model_TranslateBy(this->Character, walkDirection);
    }
}
