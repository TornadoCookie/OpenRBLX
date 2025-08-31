#include "testservice.h"
#include "scriptruntime.h"
#include <stdio.h>
#include <string.h>

TestService *TestService_new(const char *className, Instance *parent)
{
    TestService *newInst = Instance_new(className, parent);

    newInst->instance.DataCost = sizeof(*newInst);
    newInst = realloc(newInst, sizeof(*newInst));

    return newInst;
}

static void run_scripts(Instance *inst)
{
    if (!inst) return;

    printf("%s\n", inst->ClassName);

    if (!strcmp(inst->ClassName, "LocalScript"))
    {
        ScriptRuntime *scrRuntime = ScriptRuntime_new("ScriptRuntime", inst);
        ScriptRuntime_RunLocalScript(scrRuntime, inst);
    }
    else if (!strcmp(inst->ClassName, "Script"))
    {
        ScriptRuntime *scrRuntime = ScriptRuntime_new("ScriptRuntime", inst);
        ScriptRuntime_RunScript(scrRuntime, inst);
    }

    for (int i = 0; i < inst->childCount; i++)
    {
        run_scripts(inst->children[i]);
    }
}

void TestService_Run(TestService *this)
{
    run_scripts(this);
}

void serialize_TestService(TestService *testservice, SerializeInstance *inst)
{
    serialize_Instance(testservice, inst);

    serialize_atomic(bool, testservice, AutoRuns);
    serialize_atomic(bool, testservice, ExecuteWithStudioRun);
}



