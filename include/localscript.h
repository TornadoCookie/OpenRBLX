#ifndef _LOCALSCRIPT_
#define _LOCALSCRIPT_

#include "script.h"

typedef struct LocalScript {
    Script script;
} LocalScript;

LocalScript *LocalScript_new(const char *className, Instance *parent);

void serialize_LocalScript(LocalScript *localscript, SerializeInstance *inst);

#endif

