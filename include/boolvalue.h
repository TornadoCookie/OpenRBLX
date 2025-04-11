#ifndef _BOOLVALUE_
#define _BOOLVALUE_

#include "valuebase.h"

typedef struct BoolValue {
    ValueBase valuebase;
    bool Value;
    RBXScriptSignal *Changed;
} BoolValue;

BoolValue *BoolValue_new(const char *className, Instance *parent);
void BoolValue_SetValue(BoolValue *this, bool value);

void serialize_BoolValue(BoolValue *boolvalue, SerializeInstance *inst);

#endif
