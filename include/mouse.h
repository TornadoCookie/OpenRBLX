#ifndef _MOUSE_
#define _MOUSE_

#include "instance.h"

typedef struct Mouse {
    Instance instance;
} Mouse;

Mouse *Mouse_new(const char *className, Instance *parent);

void serialize_Mouse(Mouse *mouse, SerializeInstance *inst);

#endif

