#ifndef _SERVERSCRIPTSERVICE_
#define _SERVERSCRIPTSERVICE_

#include "instance.h"

typedef struct ServerScriptService {
    Instance instance;
    
    bool LoadStringEnabled;
} ServerScriptService;

ServerScriptService *ServerScriptService_new(const char *className, Instance *parent);

void ServerScriptService_RunScripts(ServerScriptService *this);

#endif
