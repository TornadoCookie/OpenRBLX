#ifndef _TESTSERVICE_
#define _TESTSERVICE_

#include "instance.h"

typedef struct TestService {
    Instance instance;

    bool AutoRuns;
    bool ExecuteWithStudioRun;
} TestService;

TestService *TestService_new(const char *className, Instance *parent);

void TestService_Run(TestService *this);

void serialize_TestService(TestService *testservice, SerializeInstance *inst);

#endif

