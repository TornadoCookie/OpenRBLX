#include "datamodel.h"
#include <stddef.h>
#include "debug.h"
#include <stdio.h>

DataModel *DataModel_new(void)
{
    DataModel *newInst = ServiceProvider_new("DataModel", NULL);

    newInst->serviceprovider.instance.DataCost = sizeof(DataModel);

    newInst->AllowedGearTypeChanged = RBXScriptSignal_new();
    newInst->GraphicsQualityChangeRequest = RBXScriptSignal_new();
    newInst->ItemChanged = RBXScriptSignal_new();

    newInst->JobId = "OpenRBLX";

    return newInst;
}

JobInfo *DataModel_GetJobsInfo(DataModel *this, int *jobsInfoCount)
{
    FIXME("this %p, jobsInfoCount %p stub!\n", this, jobsInfoCount);

    *jobsInfoCount = 0;
    return NULL;
}

bool DataModel_GetRemoteBuildMode(DataModel *this)
{
    FIXME("this %p stub!\n", this);

    return false;
}

const char *DataModel_HttpGetAsync(DataModel *this, const char *url, int httpRequestType)
{
    FIXME("this %p, url \"%s\", httpRequestType %d stub!\n", this, url, httpRequestType);

    return NULL;
}

const char *DataModel_HttpPostAsync(DataModel *this, const char *url, const char *data, const char *contentType, int httpRequestType)
{
    FIXME("this %p, url \"%s\", data \"%s\", contentType \"%s\", httpRequestType %d stub!\n", this, url, data, contentType, httpRequestType);

    return NULL;
}

bool DataModel_IsGearTypeAllowed(DataModel *this, int gearType)
{
    FIXME("this %p, gearType %d stub!\n", this, gearType);

    return true;
}

void DataModel_Load(DataModel *this, const char *url)
{
    FIXME("this %p, url \"%s\" stub!\n", this, url);
}

void DataModel_Shutdown(DataModel *this)
{
    FIXME("this %p semi-stub\n", this);
    RBXScriptSignal_Fire(this->serviceprovider.Close, NULL);
}
