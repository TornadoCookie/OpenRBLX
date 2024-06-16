#ifndef _RUNSERVICE_
#define _RUNSERVICE_

#include "instance.h"

typedef struct RunService {
    Instance instance;

    RBXScriptSignal *Stepped;
    RBXScriptSignal *Heartbeat;

    bool running;
} RunService;

RunService *RunService_new(const char *className, Instance *parent);

void RunService_Run(RunService *this);
void RunService_Reset(RunService *this);
void RunService_Pause(RunService *this);
bool RunService_IsRunning(RunService *this);

#endif
