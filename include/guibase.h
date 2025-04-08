#ifndef _GUIBASE_
#define _GUIBASE_

#include "instance.h"

typedef struct GuiBase GuiBase;

typedef struct GuiBase {
    Instance instance;

    void (*drawFunc)(GuiBase *);
} GuiBase;

GuiBase *GuiBase_new(const char *className, Instance *parent);
void GuiBase_Draw(GuiBase *this);

void serialize_GuiBase(GuiBase *guibase, SerializeInstance *inst);

#endif

