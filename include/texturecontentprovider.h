#ifndef _TEXTURECONTENTPROVIDER_
#define _TEXTURECONTENTPROVIDER_

#include "cacheablecontentprovider.h"
#include <raylib.h>

typedef struct TextureContentProvider {
    CacheableContentProvider cacheablecontentprovider;

    Texture2D studsTexture;
    Shader tilingShader;
    bool texturesEnabled;
} TextureContentProvider;

TextureContentProvider *TextureContentProvider_new(const char *className, Instance *parent);

Texture2D TextureContentProvider_LoadTextureAsset(TextureContentProvider *this, const char *content);

#endif
