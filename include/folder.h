#ifndef _FOLDER_
#define _FOLDER_

#include "instance.h"

typedef struct Folder {
    Instance instance;
} Folder;

Folder *Folder_new(const char *className, Instance *parent);

void serialize_Folder(Folder *folder, SerializeInstance *inst);

#endif
