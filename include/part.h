#ifndef _PART_
#define _PART_

#include "formfactorpart.h"

typedef struct Part {
    FormFactorPart formfactorpart;
    int shape;
} Part;

Part *Part_new(Instance *parent);

#endif
