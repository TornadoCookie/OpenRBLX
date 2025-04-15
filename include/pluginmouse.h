#ifndef _PLUGINMOUSE_
#define _PLUGINMOUSE_

#include "mouse.h"

typedef struct PluginMouse {
    Mouse mouse;
} PluginMouse;

PluginMouse *PluginMouse_new(const char *className, Instance *parent);

void serialize_PluginMouse(PluginMouse *pluginmouse, SerializeInstance *inst);


#endif

