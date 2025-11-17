#ifndef _CHANGEHISTORYSERVICE_
#define _CHANGEHISTORYSERVICE_

#include "instance.h"

typedef struct ChangeHistoryService {
    Instance instance;
} ChangeHistoryService;

ChangeHistoryService *ChangeHistoryService_new(const char *className, Instance *parent);
void serialize_ChangeHistoryService(ChangeHistoryService *changehistoryservice, SerializeInstance *inst);

#endif

