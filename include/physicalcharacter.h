#ifndef _PHYSICALCHARACTER_
#define _PHYSICALCHARACTER_

#include "model.h"

typedef enum {
    PostureXML_Stand,
} PostureXML;

typedef struct PhysicalCharacter {
    Model_Instance model;

    Instance *Head;
    Instance *UpperBody;
    Instance *LowerBody;
    Instance *LeftLeg;
    Instance *RightLeg;
    Instance *LeftArm;
    Instance *RightArm;
    PostureXML PostureXML;
} PhysicalCharacter;

PhysicalCharacter *PhysicalCharacter_new(Instance *parent);

#endif
