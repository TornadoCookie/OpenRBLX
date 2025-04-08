#ifndef _STYLEBASE_
#define _STYLEBASE_

#include "instance.h"

typedef struct StyleBase {
    Instance instance;
} StyleBase;

StyleBase *StyleBase_new(const char *className, Instance *parent);

void serialize_StyleBase(StyleBase *stylebase, SerializeInstance *inst);

#endif

