#include "studio/changehistoryservice.h"

ChangeHistoryService *ChangeHistoryService_new(const char *className, Instance *parent)
{
    INSTANCE_STUB_CONSTRUCTOR(ChangeHistoryService, Instance)
}

void serialize_ChangeHistoryService(ChangeHistoryService *changehistoryservice, SerializeInstance *inst)
{
    serialize_Instance(changehistoryservice, inst);
}

