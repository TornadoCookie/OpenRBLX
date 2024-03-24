#ifndef _FORMFACTORPART_
#define _FORMFACTORPART_

#include "basepart.h"

typedef struct FormFactorPart {
    BasePart basepart;
} FormFactorPart;

FormFactorPart *FormFactorPart_new(const char *className, Instance *parent);

#endif
