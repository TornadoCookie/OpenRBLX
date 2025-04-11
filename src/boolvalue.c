#include "boolvalue.h"

BoolValue *BoolValue_new(const char *className, Instance *parent)
{
    BoolValue *newInst = ValueBase_new("BoolValue", parent);

    newInst->valuebase.instance.DataCost = sizeof(BoolValue);
    newInst = realloc(newInst, sizeof(BoolValue));

    newInst->Changed = RBXScriptSignal_new();

    return newInst;
}

void BoolValue_SetValue(BoolValue *this, bool value)
{
    this->Value = value;
    RBXScriptSignal_Fire(this->Changed, &this->Value);
}

void serialize_BoolValue(BoolValue *boolvalue, SerializeInstance *inst)
{
    serialize_ValueBase(boolvalue, inst);
    
    serialize_atomic(bool, boolvalue, Value);
}
