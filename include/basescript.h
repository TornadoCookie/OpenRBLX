#ifndef _BASESCRIPT_
#define _BASESCRIPT_

#include "luasourcecontainer.h"

typedef struct BaseScript {
    LuaSourceContainer luasourcecontainer;

    bool Disabled;
} BaseScript;

BaseScript *BaseScript_new(const char *className, Instance *parent);

void serialize_BaseScript(BaseScript *basescript, SerializeInstance *inst);

#endif
