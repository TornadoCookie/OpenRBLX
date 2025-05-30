#include "player.h"
#include "datamodel.h"
#include "part.h"
#include "decal.h"
#include <string.h>
#include "scriptruntime.h"
#include "starterplayerscripts.h"
#include "startercharacterscripts.h"
#include "playerscripts.h"

static void replicate_scripts(Instance *src, Instance *dest)
{
    int childCount;
    Instance **children = Instance_GetChildren(src, &childCount);
    for (int i = 0; i < childCount; i++)
    {
        printf("%s\n", children[i]->ClassName);
        Instance *clone = Instance_Clone(children[i]);
        Instance_SetParent(clone, dest);
    }
}

Player *Player_new(const char *className, Instance *parent)
{
    Player *newInst = Instance_new(className, parent);

    newInst->instance.DataCost = sizeof(Player);
    newInst = realloc(newInst, sizeof(Player));

    if (parent) Instance_SetParent(newInst, parent);

    newInst->Character = NULL;

    Instance **children;
    int childCount;

    // StarterPack
    // StarterGui
    // StarterPlayerScripts
    StarterPlayerScripts *starterPlayerScripts = Instance_FindFirstChildOfClass(ServiceProvider_GetService(GetDataModel(), "StarterPlayer"), "StarterPlayerScripts");
    if (starterPlayerScripts)
    {
        PlayerScripts *playerScripts = PlayerScripts_new("PlayerScripts", newInst);
        replicate_scripts(starterPlayerScripts, playerScripts);
    }

    return newInst;
}

static void run_scripts(Instance *inst)
{
    if (!inst) return;

    printf("%s\n", inst->ClassName);

    if (!strcmp(inst->ClassName, "LocalScript"))
    {
        ScriptRuntime *scrRuntime = ScriptRuntime_new("ScriptRuntime", inst);
        ScriptRuntime_RunLocalScript(scrRuntime, inst);
    }

    for (int i = 0; i < inst->childCount; i++)
    {
        run_scripts(inst->children[i]);
    }
}

void Player_LoadCharacter(Player *this)
{
    Model_Instance *mdl = Model_new("Model", GetDataModel()->Workspace);
    mdl->pvinstance.instance.Name = this->instance.Name;

    Part *torso = Part_new("Part", mdl);
    torso->formfactorpart.basepart.pvinstance.instance.Name = "Torso";
    BasePart_SetCFrame(torso, (CFrame){0, 3, 0, -1, 0, 0, 0, 1, 0, 0, 0, -1});
    torso->formfactorpart.basepart.size = (Vector3){2, 2, 1};
    BasePart_SetColor(torso, (Color3){0.565, 0.78, 0.922});

    Part *head = Part_new("Part", mdl);
    head->formfactorpart.basepart.pvinstance.instance.Name = "Head";
    BasePart_SetCFrame(head, (CFrame){0, 4.5, 0, -1, 0, 0, 0, 1, 0, 0, 0, -1});
    head->formfactorpart.basepart.size = (Vector3){2, 1, 1};
    BasePart_SetColor(head, (Color3){0.953, 1, 0.686});

    Part *leftArm = Part_new("Part", mdl);
    leftArm->formfactorpart.basepart.pvinstance.instance.Name = "Left Arm";
    BasePart_SetCFrame(leftArm, (CFrame){1.5, 3, 0, -1, 0, 0, 0, 1, 0, 0, 0, -1});
    leftArm->formfactorpart.basepart.size = (Vector3){1, 2, 1};
    BasePart_SetColor(leftArm, (Color3){0.953, 1, 0.686});

    Part *rightArm = Part_new("Part", mdl);
    rightArm->formfactorpart.basepart.pvinstance.instance.Name = "Right Arm";
    BasePart_SetCFrame(rightArm, (CFrame){-1.5, 3, 0, -1, 0, 0, 0, 1, 0, 0, 0, -1});
    rightArm->formfactorpart.basepart.size = (Vector3){1, 2, 1};
    BasePart_SetColor(rightArm, (Color3){0.953, 1, 0.686});

    Part *leftLeg = Part_new("Part", mdl);
    leftLeg->formfactorpart.basepart.pvinstance.instance.Name = "Left Leg";
    BasePart_SetCFrame(leftLeg, (CFrame){0.5, 1, 0, -1, 0, 0, 0, 1, 0, 0, 0, -1});
    leftLeg->formfactorpart.basepart.size = (Vector3){1, 2, 1};
    BasePart_SetColor(leftLeg, (Color3){0.443, 0.729, 0.42});

    Part *rightLeg = Part_new("Part", mdl);
    rightLeg->formfactorpart.basepart.pvinstance.instance.Name = "Right Leg";
    BasePart_SetCFrame(rightLeg, (CFrame){-0.5, 1, 0, -1, 0, 0, 0, 1, 0, 0, 0, -1});
    rightLeg->formfactorpart.basepart.size = (Vector3){1, 2, 1};
    BasePart_SetColor(rightLeg, (Color3){0.443, 0.729, 0.42});

    mdl->PrimaryPart = head;

    // StarterCharacterScripts
    StarterCharacterScripts *starterCharacterScripts = Instance_FindFirstChildOfClass(ServiceProvider_GetService(GetDataModel(), "StarterPlayer"), "StarterCharacterScripts");
    if (starterCharacterScripts)
    {
        replicate_scripts(starterCharacterScripts, mdl);
    }

    this->Character = mdl;

    run_scripts(this->Character);
}

void Player_Move(Player *this, Vector3 walkDirection, bool relativeToCamera)
{
    if (this->Character)
    {
        //if (relativeToCamera)
        //{
        //    Camera cam = GetDataModel()->Workspace->CurrentCamera->camera;
        //    Matrix lookMatrix = MatrixInvert(MatrixLookAt(cam.position, cam.target, cam.up));
        //    Vector3 axis;
        //    float angle;
        //    QuaternionToAxisAngle(QuaternionFromMatrix(lookMatrix), &axis, &angle);
        //    walkDirection = Vector3RotateByAxisAngle(walkDirection, axis, angle);
        //}
        Camera *cam = &GetDataModel()->Workspace->CurrentCamera->camera;
        cam->position = Vector3Add(cam->position, walkDirection);
        Model_TranslateBy(this->Character, walkDirection);
    }
}


void Player_RunScripts(Player *this)
{
    run_scripts(ServiceProvider_GetService(GetDataModel(), "ReplicatedFirst"));
    run_scripts(Instance_FindFirstChildOfClass(this, "PlayerScripts"));
    run_scripts(Instance_FindFirstChildOfClass(this, "PlayerGui"));
    run_scripts(Instance_FindFirstChildOfClass(this, "Backpack"));
}

