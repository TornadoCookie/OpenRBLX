#ifndef _DATAMODEL_
#define _DATAMODEL_

#include "serviceprovider.h"
#include "workspace.h"

typedef struct JobInfo {
    const char *name;
    int averageDutyCycle;
    int averageStepsPerSecond;
    int averageStepTime;
    int averageError;
    bool isRunning;
} JobInfo;

typedef struct DataModel {
    ServiceProvider serviceprovider;

    int CreatorType;
    int Genre;
    const char *JobId;
    Workspace *Workspace;

    RBXScriptSignal *AllowedGearTypeChanged;
    RBXScriptSignal *GraphicsQualityChangeRequest;
    RBXScriptSignal *ItemChanged;
} DataModel;

DataModel *DataModel_new(void);

JobInfo *DataModel_GetJobsInfo(DataModel *this, int *jobsInfoCount);
bool DataModel_GetRemoteBuildMode(DataModel *this);
const char *DataModel_HttpGetAsync(DataModel *this, const char *url, int httpRequestType);
const char *DataModel_HttpPostAsync(DataModel *this, const char *url, const char *data, const char *contentType, int httpRequestType);
bool DataModel_IsGearTypeAllowed(DataModel *this, int gearType);
void DataModel_Load(DataModel *this, const char *url);
void DataModel_Shutdown(DataModel *this);

void DataModel_Draw(DataModel *this);

DataModel *GetDataModel(void);

#endif
