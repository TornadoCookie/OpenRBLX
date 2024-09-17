#ifndef _FORMFACTORPART_
#define _FORMFACTORPART_

#include "basepart.h"

typedef enum {
    FormFactor_Symmetric,
    FormFactor_Brick,
    FormFactor_Plate,
    FormFactor_Custom,
} FormFactor;

typedef struct FormFactorPart {
    BasePart basepart;
    FormFactor FormFactor;
} FormFactorPart;

FormFactorPart *FormFactorPart_new(const char *className, Instance *parent);

void serialize_FormFactorPart(FormFactorPart *formfactorpart, SerializeInstance *inst);

#endif
