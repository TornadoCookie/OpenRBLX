#include "numbervalue.h"

NumberValue *NumberValue_new(const char *className, Instance *parent)
{
    NumberValue *newInst = ValueBase_new("NumberValue", parent);

    newInst->valuebase.instance.DataCost = sizeof(NumberValue);
    newInst = realloc(newInst, sizeof(NumberValue));

    newInst->Changed = RBXScriptSignal_new();

    return newInst;
}

void NumberValue_SetValue(NumberValue *this, double value)
{
    this->Value = value;
    RBXScriptSignal_Fire(this->Changed, &this->Value);
}

void serialize_NumberValue(NumberValue *numbervalue, SerializeInstance *inst)
{
    serialize_ValueBase(numbervalue, inst);
    
    serialize_atomic(double, numbervalue, Value);
}