#include "studiotheme.h"

StudioTheme *StudioTheme_new(const char *className, Instance *parent)
{
    INSTANCE_STUB_CONSTRUCTOR(StudioTheme, Instance);
}

void serialize_StudioTheme(StudioTheme *studiotheme, SerializeInstance *inst)
{
    serialize_Instance(studiotheme, inst);
}

