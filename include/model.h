#ifndef _MODEL_
#define _MODEL_

#include "pvinstance.h"

typedef struct Model {
    PVInstance pvinstance;
} Model;

Model *Model_new(const char *className, Instance *parent);

#endif
