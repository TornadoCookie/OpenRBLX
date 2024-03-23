#ifndef _WORLDROOT_
#define _WORLDROOT_

#include "model.h"

typedef struct WorldRoot {
    Model model;
} WorldRoot;

WorldRoot *WorldRoot_new(const char *className, Instance *parent);

#endif
