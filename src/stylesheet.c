#include "stylesheet.h"

StyleSheet *StyleSheet_new(const char *className, Instance *parent)
{
    INSTANCE_STUB_CONSTRUCTOR(StyleSheet, StyleBase);
}

void serialize_StyleSheet(StyleSheet *stylesheet, SerializeInstance *inst)
{
    serialize_StyleBase(stylesheet, inst);
}

