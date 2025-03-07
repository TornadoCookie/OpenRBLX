// RBXS shader format

#include <raylib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "debug.h"

DEFAULT_DEBUG_CHANNEL(rbxs);

typedef struct RBXSHeader {
    uint16_t unknown1[2];
    uint16_t names3Count;
    uint16_t names4Count;
    uint32_t unknown2[2];
} RBXSHeader;

typedef struct RBXSShaderData {
    uint32_t unknown1[4];
    uint32_t offset;
    uint32_t length;
    uint32_t unknown2[10];
} RBXSShaderData;

Shader *LoadShadersRBXS(char *filename, int *shaderCount)
{
    int dataSize;
    unsigned char *data = LoadFileData(filename, &dataSize);
    unsigned char *initData = data;

    if (strncmp(data, "RBXS", 4))
    {
        ERR("invalid signature %4s\n", data);
        return;
    }

    data += 4;

    RBXSHeader *header = data;
    printf("Unknown 1.1: %d\n", header->unknown1[0]);
    printf("Unknown 1.2: %d\n", header->unknown1[1]);
    printf("Names 3 count: %d\n", header->names3Count);
    printf("Names 4 count: %d\n", header->names4Count);
    printf("Unknown 2.1: %#x\n", header->unknown2[0]);
    printf("Unknown 2.2: %#x\n", header->unknown2[1]);

    data += sizeof(RBXSHeader);

    for (int i = 0; i < 4; i++)
    {
        printf("Name 1.%d: %s\n", i, data);
        data += 0x40;
    }

    for (int i = 0; i < 406; i++)
    {
        printf("Name 2.%d: %#x %s\n", i, *(uint32_t*)data, data+4);
        data += 0x44;
    }

    printf("Unknown: %#x\n", *(uint32_t*)data);
    data += 4;

    for (int i = 0; i < header->names3Count; i++)
    {
        printf("Name 3.%d: %s\n", i, data);
        data += 0x40;
    }

    for (int i = 0; i < header->names4Count; i++)
    {
        printf("Name 4.%d: %#x %s\n", i, data[0x40], data);
        data += 0x41;
    }

    RBXSShaderData *shaderData = data;
    int count = (410 + header->names3Count + header->names4Count + 5260);

    data += 0x40 * count;

    for (int i = 0; i < count; i++)
    {
        RBXSShaderData *shader = shaderData+i;
        printf("Unknown 2.0.%d.0:", i);
        for (int j = 0; j < 0x40; j++)
        {
            printf(" %02X", ((unsigned char *)(shaderData+i))[j]);
        }
        printf("\n");
        printf("Shader code for %d (len %d):\n", i, shader->length);// shader->length, data);
        data = initData + shader->offset;
        for (int j = 0; j < shader->length; j++) putc(data[j], stdout);
    }

    printf("Stopped reading file @ %#x\n", data - initData);

    *shaderCount = 0;
    return NULL;
}

