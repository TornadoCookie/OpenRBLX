#include "mouse.h"

Mouse *Mouse_new(const char *className, Instance *parent)
{
    INSTANCE_STUB_CONSTRUCTOR_BASE(Mouse, Instance);
}

void serialize_Mouse(Mouse *mouse, SerializeInstance *inst)
{
    serialize_Instance(mouse, inst);
}

