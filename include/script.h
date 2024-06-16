#ifndef _SCRIPT_
#define _SCRIPT_

#include "basescript.h"

typedef struct Script {
    BaseScript basescript;

    const char *Source;
} Script;

Script *Script_new(const char *className, Instance *parent);

void serialize_Script(Script *script, SerializeInstance *inst);

#endif
