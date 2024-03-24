#include "filetypes.h"
#include "datamodel.h"

#define DEBUG_IMPL
#include "debug.h"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("usage: test_rbxmx <mdl.rbxmx>\n");
        return 0;
    }
    DataModel *datamodel = DataModel_new();
    Model *mdl = LoadModelRBXMX(argv[1]);

    Instance_SetParent(mdl, datamodel);
    
    DataModel_Shutdown(datamodel);
    return 0;
}
