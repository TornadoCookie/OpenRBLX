#ifndef _STYLERULE_
#define _STYLERULE_

#include "stylebase.h"

typedef struct StyleRule {
    StyleBase stylebase;
} StyleRule;

StyleRule *StyleRule_new(const char *className, Instance *parent);

#endif
