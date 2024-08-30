#ifndef _TEXTURECONTENTPROVIDER_
#define _TEXTURECONTENTPROVIDER_

#include "cacheablecontentprovider.h"
#include <raylib.h>

typedef struct TextureContentProvider {
    CacheableContentProvider cacheablecontentprovider;

    Texture2D studsTexture;
    Shader tilingShader;
} TextureContentProvider;

TextureContentProvider *TextureContentProvider_new(const char *className, Instance *parent);

#endif
