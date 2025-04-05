#ifndef _GUISERVICE_
#define _GUISERVICE_

#include "instance.h"

typedef struct GuiService {
    Instance instance;
} GuiService;

GuiService *GuiService_new(const char *className, Instance *parent);

#endif

