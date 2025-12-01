#ifndef _TRANSLATOR_
#define _TRANSLATOR_

#include "instance.h"

typedef struct Translator {
    Instance instance;
} Translator;

Translator *Translator_new(const char *className, Instance *parent);

const char *Translator_FormatByKey(Translator *this, const char *key);

void serialize_Translator(Translator *translator, SerializeInstance *inst);

#endif

