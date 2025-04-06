#ifndef _SERIALIZE_
#define _SERIALIZE_

#include <stdint.h>
#include <stdlib.h>
#include "cframe.h"

typedef enum {
    Serialize_bool,
    Serialize_float,
    Serialize_token,
    Serialize_int,
    Serialize_CoordinateFrame,
    Serialize_string,
    Serialize_Vector3,
    Serialize_Color3,
    Serialize_Ref,
    Serialize_double,
    Serialize_event
} SerializationType;

typedef struct Serialization {
    SerializationType type;
    const char *name;
    void *val;
} Serialization;

typedef struct SerializeInstance {
    Serialization *serializations;
    int serializationCount;
    CFrame modelOffset;
    uint32_t Color3uint8;
    int TopConstraint, BottomConstraint, LeftConstraint, RightConstraint, FrontConstraint, BackConstraint;
} SerializeInstance;

static void _serialize_atomic(SerializeInstance *inst, Serialization serialization)
{
    inst->serializationCount++;
    inst->serializations = realloc(inst->serializations, inst->serializationCount * sizeof(Serialization));
    inst->serializations[inst->serializationCount - 1] = serialization;
}

#define serialize_atomic(type, thing, prop) _serialize_atomic(inst, (Serialization){Serialize_##type, #prop, &thing->prop})

#endif
