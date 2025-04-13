#ifndef _STUDIOTHEME_
#define _STUDIOTHEME_

#include "instance.h"

typedef struct StudioTheme {
    Instance instance;
} StudioTheme;

StudioTheme *StudioTheme_new(const char *className, Instance *parent);
void serialize_StudioTheme(StudioTheme *studiotheme, SerializeInstance *inst);

#endif

