#include "stylerule.h"

StyleRule *StyleRule_new(const char *className, Instance *parent)
{
    INSTANCE_STUB_CONSTRUCTOR(StyleRule, StyleBase);
}

void serialize_StyleRule(StyleRule *stylerule, SerializeInstance *inst)
{
    serialize_StyleBase(stylerule, inst);
}

