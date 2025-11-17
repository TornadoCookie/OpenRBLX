#include "userinputservice.h"

UserInputService *UserInputService_new(const char *className, Instance *parent)
{
    INSTANCE_STUB_CONSTRUCTOR(UserInputService, Instance);
}

void serialize_UserInputService(UserInputService *userinputservice, SerializeInstance *inst)
{
    serialize_Instance(userinputservice, inst);
}

