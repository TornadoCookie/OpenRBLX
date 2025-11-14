#include "humanoid.h"
#include "model.h"
#include "basepart.h"
#include "datamodel.h"
#include <string.h>

Humanoid *Humanoid_new(const char *className, Instance *parent)
{
    Humanoid *newInst = Instance_new(className, parent);
    ((Instance*)newInst)->DataCost = sizeof(*newInst);
    newInst = realloc(newInst, sizeof(*newInst)); 

    newInst->didBakeText = false;
    

    return newInst;
}

void serialize_Humanoid(Humanoid *humanoid, SerializeInstance *inst)
{
    serialize_Instance(humanoid, inst);
}

void Humanoid_Draw(Humanoid *this)
{
    if (!this->didBakeText && this->instance.Parent)
    {
        Image textImg = GenImageColor(512, 128, MAGENTA);
        ImageDrawText(&textImg, this->instance.Parent->Name, 0, 0, 50, WHITE);
    
        this->bakedText = LoadTextureFromImage(textImg);
        UnloadImage(textImg);
        this->didBakeText = true;
    }

    Vector3 point;

    //if (!strcmp(this->instance.Parent->ClassName, "Model"))
    {
        Model_Instance *mdl = this->instance.Parent;
        BasePart *head = Instance_FindFirstChild(mdl, "Head", false);
        //if (!Instance_IsA(head, "BasePart")) return;
        
        if (head)
        {
            point = head->Position;
        }
        else
        {
            CFrame cf = Model_GetModelCFrame(mdl);
            point = (Vector3){cf.X, cf.Y, cf.Z};
        }

        point.y += 2;

        DrawBillboard(GetDataModel()->Workspace->CurrentCamera->camera, this->bakedText, point, 1.0f, WHITE);
        
    }
    //else
    {
    //    return;
    }
}

