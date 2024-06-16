#include "basescript.h"


BaseScript *BaseScript_new(const char *className, Instance *parent)
{
    BaseScript *newInst = LuaSourceContainer_new(className, parent);

    newInst->luasourcecontainer.instance.DataCost = sizeof(BaseScript);
    newInst = realloc(newInst, sizeof(BaseScript));

    return newInst;
}

void serialize_BaseScript(BaseScript *basescript, SerializeInstance *inst)
{
    serialize_LuaSourceContainer(basescript, inst);
    
    serialize_atomic(bool, basescript, Disabled);
}
