#ifndef _NUMBERVALUE_
#define _NUMBERVALUE_

#include "valuebase.h"

typedef struct NumberValue {
    ValueBase valuebase;
    double Value;
    RBXScriptSignal *Changed;
} NumberValue;

NumberValue *NumberValue_new(const char *className, Instance *parent);
void NumberValue_SetValue(NumberValue *this, double value);

void serialize_Vector3Value(NumberValue *vector3value, SerializeInstance *inst);

#endif
