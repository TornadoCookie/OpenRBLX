#include "luasourcecontainer.h"

LuaSourceContainer *LuaSourceContainer_new(const char *className, Instance *parent)
{
    LuaSourceContainer *newInst = Instance_new(className, parent);

    newInst->instance.DataCost = sizeof(LuaSourceContainer);
    newInst = realloc(newInst, sizeof(LuaSourceContainer));

    return newInst;
}

void serialize_LuaSourceContainer(LuaSourceContainer *luasourcecontainer, SerializeInstance *inst)
{
    serialize_Instance(luasourcecontainer, inst);
}
