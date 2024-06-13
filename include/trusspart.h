#ifndef _TRUSSPART_
#define _TRUSSPART_

#include "basepart.h"

typedef struct TrussPart {
    BasePart basepart;
    int Style;
} TrussPart;

TrussPart *TrussPart_new(const char *className, Instance *parent);

#endif
