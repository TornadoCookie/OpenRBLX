#ifndef _TWEENSERVICE_
#define _TWEENSERVICE_

#include "instance.h"
typedef struct TweenService {
    Instance instance;
} TweenService;

TweenService *TweenService_new(const char *className, Instance *parent);

#endif

