#include "studio.h"
#include "studiotheme.h"

Studio *Studio_new(const char *className, Instance *parent)
{
    Studio *newInst = Instance_new(className, parent);

    newInst = realloc(newInst, sizeof(*newInst));

    newInst->instance.Name = "Studio";

    newInst->Theme = StudioTheme_new("StudioTheme", newInst);
    newInst->Theme->Name = "Dark";

    newInst->ThemeChanged = RBXScriptSignal_new();

    if (parent)
        Instance_SetParent(newInst, parent);

    return newInst;
}

void serialize_Studio(Studio *studio, SerializeInstance *inst)
{
    serialize_Instance(studio, inst);

    serialize_atomic(event, studio, ThemeChanged);
    serialize_atomic(Ref, studio, Theme);
}

