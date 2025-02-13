#ifndef _MODULESCRIPT_
#define _MODULESCRIPT_

#include "luasourcecontainer.h"

typedef struct ModuleScript {
    LuaSourceContainer luasourcecontainer;

    const char *Source; // ProtectedString but C should treat it as just a string
                        // and can control access by scripts
} ModuleScript;

ModuleScript *ModuleScript_new(const char *className, Instance *parent);

void serialize_ModuleScript(ModuleScript *modulescript, SerializeInstance *inst);

#endif

