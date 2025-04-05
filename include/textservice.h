#ifndef _TEXTSERVICE_
#define _TEXTSERVICE_

#include "instance.h"
typedef struct TextService {
    Instance instance;
} TextService;

TextService *TextService_new(const char *className, Instance *parent);

#endif

