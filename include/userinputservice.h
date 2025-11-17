#ifndef _USERINPUTSERVICE_
#define _USERINPUTSERVICE_

#include "instance.h"

typedef struct UserInputService {
    Instance instance;
} UserInputService;

UserInputService *UserInputService_new(const char *className, Instance *parent);
void serialize_UserInputService(UserInputService *userinputservice, SerializeInstance *inst);

#endif

