#ifndef _BASEPLAYERGUI_
#define _BASEPLAYERGUI_

#include "instance.h"

typedef struct BasePlayerGui {
    Instance instance;
} BasePlayerGui;

BasePlayerGui *BasePlayerGui_new(const char *className, Instance *parent);

#endif

