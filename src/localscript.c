#include "localscript.h"

LocalScript *LocalScript_new(const char *className, Instance *parent)
{
    return Script_new(className, parent); // No members defined so we can use this shortcut
}

void serialize_LocalScript(LocalScript *localscript, SerializeInstance *inst)
{
    serialize_Script(localscript, inst);
}

