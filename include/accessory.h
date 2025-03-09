#ifndef _ACCESSORY_
#define _ACCESSORY_

#include "accoutrement.h"

typedef enum {
    Unknown,
    Hat,
    Hair,
    Face,
    Neck,
    Shoulder,
    Front,
    Back,
    Waist,
    // FUCK LAYERED CLOTHING!!!!
} AccessoryType;

typedef struct Accessory {
    Accoutrement accoutrement;

    AccessoryType AccessoryType;
} Accessory;

Accessory *Accessory_new(const char *className, Instance *parent);
void serialize_Accessory(Accessory *accessory, SerializeInstance *inst);

#endif

