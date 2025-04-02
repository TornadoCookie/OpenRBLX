#ifndef _COREGUI_
#define _COREGUI_

#include "baseplayergui.h"

typedef struct CoreGui {
    BasePlayerGui baseplayergui;
} CoreGui;

CoreGui *CoreGui_new(const char *className, Instance *parent);

#endif

