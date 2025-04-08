#include "guibase2d.h"

GuiBase2d *GuiBase2d_new(const char *className, Instance *parent)
{
    INSTANCE_STUB_CONSTRUCTOR_BASE(GuiBase2d, GuiBase);
}

void serialize_GuiBase2d(GuiBase2d *guibase2d, SerializeInstance *inst)
{
    serialize_GuiBase(guibase2d, inst);
}

