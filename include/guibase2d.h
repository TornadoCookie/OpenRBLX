#ifndef _GUIBASE2D_
#define _GUIBASE2D_

#include "guibase.h"

typedef struct GuiBase2d {
    GuiBase guibase;
} GuiBase2d;

GuiBase2d *GuiBase2d_new(const char *className, Instance *parent);
void serialize_GuiBase2d(GuiBase2d *guibase2d, SerializeInstance *inst);

#endif

