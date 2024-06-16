#ifndef _VECTOR3VALUE_
#define _VECTOR3VALUE_

#include "valuebase.h"

typedef struct Vector3Value {
    ValueBase valuebase;
    Vector3 Value;
    RBXScriptSignal *Changed;
} Vector3Value;

Vector3Value *Vector3Value_new(const char *className, Instance *parent);
void Vector3Value_SetValue(Vector3Value *this, Vector3 value);

void serialize_Vector3Value(Vector3Value *vector3value, SerializeInstance *inst);

#endif
