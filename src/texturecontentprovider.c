#include "texturecontentprovider.h"

TextureContentProvider *TextureContentProvider_new(const char *className, Instance *parent)
{
    TextureContentProvider *newInst = CacheableContentProvider_new(className, parent);

    newInst = realloc(newInst, sizeof(TextureContentProvider));

    newInst->studsTexture = LoadTexture("staticdata/studs.dds");
    newInst->tilingShader = LoadShader("staticdata/default.vs", "staticdata/repeat.fs");

    if (parent) Instance_SetParent(newInst, parent);

    return newInst;
}
