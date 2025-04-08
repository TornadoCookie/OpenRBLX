#include "guiobject.h"

GuiObject *GuiObject_new(const char *className, Instance *parent)
{
    INSTANCE_STUB_CONSTRUCTOR_BASE(GuiObject, GuiBase2d);
}

void serialize_GuiObject(GuiObject *guiobject, SerializeInstance *inst)
{
    serialize_GuiBase2d(guiobject, inst);
}

