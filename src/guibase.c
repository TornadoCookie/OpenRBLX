#include "guibase.h"

#include "debug.h"
DEFAULT_DEBUG_CHANNEL(guibase);

static void nodrawfunc(GuiBase *inst)
{
    FIXME("classname %s has no draw func", inst->instance.ClassName);
}

GuiBase *GuiBase_new(const char *className, Instance *parent)
{
    GuiBase *newInst = Instance_new(className, parent);

    newInst->instance.DataCost = sizeof(*newInst);
    newInst = realloc(newInst, sizeof(*newInst));

    newInst->drawFunc = nodrawfunc;

    return newInst;
}

void GuiBase_Draw(GuiBase *this)
{
    if (this->drawFunc)
        this->drawFunc(this);
}

void serialize_GuiBase(GuiBase *guibase, SerializeInstance *inst)
{
    serialize_Instance(guibase, inst);
}

