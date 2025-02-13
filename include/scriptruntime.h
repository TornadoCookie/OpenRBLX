#ifndef _SCRIPTRUNTIME_
#define _SCRIPTRUNTIME_

#include "instance.h"
#include "localscript.h"

typedef struct ScriptRuntime {
    Instance instance;
} ScriptRuntime;

ScriptRuntime *ScriptRuntime_new(const char *className, Instance *parent);

void ScriptRuntime_RunScript(ScriptRuntime *this, Script *script);
void ScriptRuntime_RunLocalScript(ScriptRuntime *this, LocalScript *script);

// no serialize, this is internally used by OpenRblx.

#endif

