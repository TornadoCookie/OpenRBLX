#include "stringvalue.h"

StringValue *StringValue_new(const char *className, Instance *parent)
{
    StringValue *newInst = ValueBase_new("StringValue", parent);

    newInst->valuebase.instance.DataCost = sizeof(StringValue);
    newInst = realloc(newInst, sizeof(StringValue));

    newInst->Changed = RBXScriptSignal_new();

    return newInst;
}

void StringValue_SetValue(StringValue *this, const char *value)
{
    this->Value = value;
    RBXScriptSignal_Fire(this->Changed, &this->Value);
}

void serialize_StringValue(StringValue *stringvalue, SerializeInstance *inst)
{
    serialize_ValueBase(stringvalue, inst);
    
    serialize_atomic(string, stringvalue, Value);
}
