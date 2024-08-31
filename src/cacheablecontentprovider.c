#include "cacheablecontentprovider.h"
#include <stdlib.h>
#include "httpservice.h"
#include <raylib.h>
#include <string.h>
#include "datamodel.h"

CacheableContentProvider *CacheableContentProvider_new(const char *className, Instance *parent)
{
    CacheableContentProvider *newInst = Instance_new(className, parent);

    newInst->instance.DataCost = sizeof(CacheableContentProvider);
    newInst = realloc(newInst, sizeof(CacheableContentProvider));

    return newInst;
}

long CacheableContentProvider_GetAssetIdFromContent(CacheableContentProvider *this, const char *content)
{
    long assetid;
    if (!strncmp(content, "http://www.roblox.com/asset", 27))
    {
        sscanf(content, "http://www.roblox.com/asset/?id=%ld", &assetid);   
    }
    else if (!strncmp(content, "rbxassetid://", 13))
    {
        sscanf(content, "rbxassetid://%ld", &assetid);
    }
    else
    {
        printf("Don't know how to handle this\n");
        return 0;
    }

    return assetid;
}

const char *CacheableContentProvider_LoadAssetDelivery(CacheableContentProvider *this, long assetId, int *dataSize)
{
    const char *url = TextFormat("https://assetdelivery.roblox.com/v2/assetId/%ld", assetId);
    printf("Our thing is: %s\n", url);

    HttpService *httpService = ServiceProvider_GetService(GetDataModel(), "HttpService");
    const char *jsonundecoded = HttpService_GetAsync(httpService, url, NULL);
    cJSON *json = HttpService_JSONDecode(httpService, jsonundecoded);

    printf("%s\n", jsonundecoded);

    cJSON *locations = cJSON_GetObjectItem(json, "locations");
    cJSON *location = cJSON_GetArrayItem(locations, 0);
    const char *newUrl = cJSON_GetStringValue(cJSON_GetObjectItem(location, "location"));
    printf("Redirecting to %s.\n", newUrl);

    return HttpService_GetAsync(httpService, newUrl, dataSize);
}
