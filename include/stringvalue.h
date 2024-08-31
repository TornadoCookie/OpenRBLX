#ifndef _STRINGVALUE_
#define _STRINGVALUE_

#include "valuebase.h"

typedef struct StringValue {
    ValueBase valuebase;
    const char *Value;
    RBXScriptSignal *Changed;
} StringValue;

StringValue *StringValue_new(const char *className, Instance *parent);
void StringValue_SetValue(StringValue *this, const char *value);

void serialize_StringValue(StringValue *stringvalue, SerializeInstance *inst);

#endif
