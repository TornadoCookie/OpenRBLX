#include "frame.h"

Frame *Frame_new(const char *className, Instance *parent)
{
    INSTANCE_STUB_CONSTRUCTOR(Frame, GuiObject);
}

void serialize_Frame(Frame *frame, SerializeInstance *inst)
{
    serialize_GuiObject(frame, inst);
}

