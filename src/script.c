#include "script.h"
#include <string.h>

Script *Script_new(const char *className, Instance *parent)
{
    Script *newInst = BaseScript_new(className, parent);

    newInst->basescript.luasourcecontainer.instance.DataCost = sizeof(Script);
    newInst = realloc(newInst, sizeof(Script));

    if (!strcmp(className, "Script") && parent) Instance_SetParent(newInst, parent);

    newInst->Source = NULL;
    newInst->sourceLength = 0;
    newInst->isBytecode = false;

    return newInst;
}

void serialize_Script(Script *script, SerializeInstance *inst)
{
    serialize_BaseScript(script, inst);

    serialize_atomic(string, script, Source);
}
