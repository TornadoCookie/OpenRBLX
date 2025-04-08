#ifndef _GUIOBJECT_
#define _GUIOBJECT_

#include "guibase2d.h"

typedef struct GuiObject {
    GuiBase2d guibase2d;
} GuiObject;

GuiObject *GuiObject_new(const char *className, Instance *parent);

void serialize_GuiObject(GuiObject *guiobject, SerializeInstance *inst);

#endif

