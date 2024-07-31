#include "folder.h"

Folder *Folder_new(const char *className, Instance *parent)
{
    Folder *newInst = Instance_new(className, parent);

    newInst = realloc(newInst, sizeof(Folder));
    newInst->instance.DataCost = sizeof(Folder);

    if (parent) Instance_SetParent(newInst, parent);

    return newInst;
}

void serialize_Folder(Folder *folder, SerializeInstance *inst)
{
    serialize_Instance(folder, inst);
}
