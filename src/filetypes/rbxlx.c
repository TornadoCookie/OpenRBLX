#include "filetypes.h"
#include "datamodel.h"

void LoadPlaceRBXLX(const char *file)
{
    DataModel *dataModel = GetDataModel();
    int mdlCount;
    Instance **instances = LoadModelRBXMX(file, &mdlCount);

    for (int i = 0; i < mdlCount; i++)
    {
        Instance_SetParent(instances[i], dataModel);
    }
}
