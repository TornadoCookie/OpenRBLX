#include "texturecontentprovider.h"
#include <stdio.h>

TextureContentProvider *TextureContentProvider_new(const char *className, Instance *parent)
{
    TextureContentProvider *newInst = CacheableContentProvider_new(className, parent);

    newInst = realloc(newInst, sizeof(TextureContentProvider));

    newInst->studsTexture = LoadTexture("staticdata/studs.dds");
    newInst->tilingShader = LoadShader("staticdata/default.vs", "staticdata/repeat.fs");

    if (parent) Instance_SetParent(newInst, parent);

    return newInst;
}

Texture2D TextureContentProvider_LoadTextureAsset(TextureContentProvider *this, const char *content)
{
    long assetid = CacheableContentProvider_GetAssetIdFromContent(this, content);

    if (assetid == 0) return (Texture2D){0};

    printf("LoadTextureAsset %s\n", content);
    printf("Get AssetId %ld\n", assetid);

    if (FileExists(TextFormat("cache/%ld.png", assetid)))
    {
        printf("Using cached\n");
        return LoadTexture(TextFormat("cache/%ld.png", assetid));
    }

    int dataSize;
    const char *data = CacheableContentProvider_LoadAssetDelivery(this, assetid, &dataSize);

    Image img = LoadImageFromMemory(".png", data, dataSize);
    ExportImage(img, TextFormat("cache/%ld.png", assetid));

    return LoadTextureFromImage(img);
}
