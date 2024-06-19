#include "decal.h"

Decal *Decal_new(const char *className, Instance *parent)
{
    Decal *newInst = FaceInstance_new(className, parent);

    newInst->faceinstance.instance.DataCost = sizeof(Decal);
    newInst = realloc(newInst, sizeof(Decal));

    return newInst;
}

void serialize_Decal(Decal *decal, SerializeInstance *inst)
{
    serialize_FaceInstance(decal, inst);

    serialize_atomic(float, decal, Transparency);
    serialize_atomic(string, decal, Texture);
    serialize_atomic(float, decal, Shiny);
    serialize_atomic(float, decal, Specular);
}
