#include "contentprovider.h"

ContentProvider *ContentProvider_new(const char *className, Instance *parent)
{
    ContentProvider *newInst = Instance_new(className, parent);

    newInst->instance.DataCost = sizeof(*newInst);
    newInst = realloc(newInst, sizeof(*newInst));

    newInst->BaseUrl = "http://www.roblox.com";

    if (parent)
        Instance_SetParent(newInst, parent);

    return newInst;
}

void serialize_ContentProvider(ContentProvider *contentprovider, SerializeInstance *inst)
{
    serialize_Instance(contentprovider, inst);

    serialize_atomic(string, contentprovider, BaseUrl);
}

