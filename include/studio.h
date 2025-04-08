#ifndef _STUDIO_
#define _STUDIO_

#include "instance.h"

typedef struct Studio {
    Instance instance;

    RBXScriptSignal *ThemeChanged;
} Studio;

Studio *Studio_new(const char *className, Instance *parent);
void serialize_Studio(Studio *studio, SerializeInstance *inst);

#endif

