#include "stylebase.h"

StyleBase *StyleBase_new(const char *className, Instance *parent)
{
    INSTANCE_STUB_CONSTRUCTOR_BASE(StyleBase, Instance);
}

void serialize_StyleBase(StyleBase *stylebase, SerializeInstance *inst)
{
    serialize_Instance(stylebase, inst);
}

