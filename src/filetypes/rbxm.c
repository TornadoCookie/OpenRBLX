#include "filetypes.h"
#include <stdio.h>

#include "debug.h"

DEFAULT_DEBUG_CHANNEL(rbxm)

Instance **LoadModelRBXM(const char *file, int *mdlCount)
{
    FILE *f = fopen(file, "rb");
    if (!f)
    {
        fprintf(stderr, "an error occurred while parsing file \"%s\": ", file);
        perror("error");
        *mdlCount = 0;
        return NULL;
    }
}
