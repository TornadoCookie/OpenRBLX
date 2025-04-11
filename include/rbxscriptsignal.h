#ifndef _RBXSCRIPTSIGNAL_
#define _RBXSCRIPTSIGNAL_

#include <stdbool.h>

typedef struct _RBXScriptSignal_Internal _RBXScriptSignal_Internal;

typedef struct RBXScriptConnection {
    bool connected;
    void (*func)(void *arg, void *ud);
    void *userdata;
    bool do_once;
    bool did_once;
} RBXScriptConnection;

void RBXScriptConnection_Disconnect(RBXScriptConnection *this);

typedef struct RBXScriptSignal {
    RBXScriptConnection **connections;
    int connectionCount;
    _RBXScriptSignal_Internal *private;
} RBXScriptSignal;

RBXScriptSignal *RBXScriptSignal_new();
RBXScriptConnection *RBXScriptSignal_Connect(RBXScriptSignal *this, void *func, void *ud);
RBXScriptConnection *RBXScriptSignal_Once(RBXScriptSignal *this, void *func, void *ud);
void RBXScriptSignal_Wait(RBXScriptSignal *this);
void RBXScriptSignal_Fire(RBXScriptSignal *this, void *arg);

#endif
