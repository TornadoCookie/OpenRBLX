#ifndef _SKY_
#define _SKY_

#include "instance.h"

typedef struct Sky {
    Instance instance;

    bool CelestialBodiesShown;
    char *SkyboxBk;
    char *SkyboxDn;
    char *SkyboxFt;
    char *SkyboxLf;
    char *SkyboxRt;
    char *SkyboxUp;
    int StarCount;
} Sky;

Sky *Sky_new(Instance *parent);

#endif
