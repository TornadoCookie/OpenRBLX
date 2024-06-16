#ifndef _VALUEBASE_
#define _VALUEBASE_

#include "instance.h"

typedef struct ValueBase {
    Instance instance;
} ValueBase;

ValueBase *ValueBase_new(const char *className, Instance *parent);

void serialize_ValueBase(ValueBase *valuebase, SerializeInstance *inst);

#endif
