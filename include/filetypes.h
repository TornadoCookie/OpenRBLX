#ifndef _FILETYPES_
#define _FILETYPES_

#include "instance.h"

Instance **LoadModelRBXMX(const char *file, int *mdlCount);
void LoadPlaceRBXLX(const char *file);

Instance **LoadModelRBXM(const char *file, int *mdlCount);
void LoadPlaceRBXL(const char *file);

#endif
