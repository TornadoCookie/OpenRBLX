#include "filetypes.h"
#include "datamodel.h"

#define DEBUG_IMPL
#include "debug.h"

int main()
{
    DataModel *datamodel = DataModel_new();
    Model *mdl = LoadModelRBXMX("res/770 Ball 4x4x4.rbxmx");

    Instance_SetParent(mdl, datamodel);
    
    DataModel_Shutdown(datamodel);
    return 0;
}
