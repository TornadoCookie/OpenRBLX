#include "modulescript.h"

ModuleScript *ModuleScript_new(const char *className, Instance *parent)
{
    ModuleScript *newInst = LuaSourceContainer_new(className, parent);

    newInst->luasourcecontainer.instance.DataCost = sizeof(*newInst);
    newInst = realloc(newInst, sizeof(*newInst));

    newInst->Source = NULL;

    return newInst;
}

void serialize_ModuleScript(ModuleScript *modulescript, SerializeInstance *inst)
{
    serialize_LuaSourceContainer(modulescript, inst);
    serialize_atomic(string, modulescript, Source);
}

