#ifndef _LUASOURCECONTAINER_
#define _LUASOURCECONTAINER_

#include "instance.h"

typedef struct LuaSourceContainer {
    Instance instance;
} LuaSourceContainer;

LuaSourceContainer *LuaSourceContainer_new(const char *className, Instance *parent);

void serialize_LuaSourceContainer(LuaSourceContainer *luasourcecontainer, SerializeInstance *inst);

#endif
