#include "vector3value.h"

Vector3Value *Vector3Value_new(const char *className, Instance *parent)
{
    Vector3Value *newInst = ValueBase_new("Vector3Value", parent);

    newInst->valuebase.instance.DataCost = sizeof(Vector3Value);
    newInst = realloc(newInst, sizeof(Vector3Value));

    newInst->Changed = RBXScriptSignal_new();

    return newInst;
}

void Vector3Value_SetValue(Vector3Value *this, Vector3 value)
{
    this->Value = value;
    RBXScriptSignal_Fire(this->Changed, &this->Value);
}

void serialize_Vector3Value(Vector3Value *vector3value, SerializeInstance *inst)
{
    serialize_ValueBase(vector3value, inst);
    
    serialize_atomic(Vector3, vector3value, Value);
}
