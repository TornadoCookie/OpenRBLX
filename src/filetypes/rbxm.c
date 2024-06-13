#include "filetypes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "debug.h"

DEFAULT_DEBUG_CHANNEL(rbxm)

const unsigned char signature[] = {
    0x3C, 0x72, 0x6F, 0x62, 0x6C, 0x6F, 0x78, 0x21, 0x89, 0xFF, 0x0D, 0x0A, 0x1A, 0x0A,
};

typedef struct Header {
    uint32_t ClassCount;
    uint32_t InstanceCount;
    uint8_t Reserved[8];
} Header;

typedef struct Chunk {
    char Signature[4];
    uint32_t CompressedLength;
    uint32_t UncompressedLength;
    uint8_t Reserved[4];
    uint8_t Payload[];
} Chunk;

typedef struct InstanceChunk {
    int32_t ClassID;
    char *ClassName;
    uint8_t HasService;
    uint32_t Length;
    int32_t *IDs;
    bool *IsService;
} InstanceChunk;

typedef struct PropertiesChunk {
    int32_t ClassID;
    char *Name;
} PropertiesChunk;

Instance **LoadModelRBXM(const char *file, int *mdlCount)
{
    FILE *f = fopen(file, "rb");
    if (!f)
    {
        fprintf(stderr, "an error occurred while parsing file \"%s\": ", file);
        perror("error");
        *mdlCount = 0;
        return NULL;
    }

    Instance **ret = NULL;
    *mdlCount = 0;

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    unsigned char *data = malloc(size);
    fread(data, 1, size, f);

    if (memcmp(data, signature, sizeof(signature)))
    {
        free(data);
        printf("error: %s has invalid signature.\n", file);
        return ret;
    }

    data += sizeof(signature);

    uint16_t *version = data;
    printf("Version: %d\n", *version);
    data += 2;

    Header *header = data;
    printf("ClassCount: %d\n", header->ClassCount);
    printf("InstanceCount: %d\n", header->InstanceCount);

    data += sizeof(Header);

    while (1)
    {
        Chunk *chunk = data;
        printf("\nChunk Info:\n");
        printf("Chunk signature: %.4s\n", chunk->Signature);
        printf("CompressedLength: %d\n", chunk->CompressedLength);
        printf("UncompressedLength: %d\n", chunk->UncompressedLength);
        if (!strncmp(chunk->Signature, "END\0", 4))
        {
            break;
        }
        else if (!strncmp(chunk->Signature, "INST", 4))
        {
            unsigned char *chunkData = data + 17;
            InstanceChunk chunk = { 0 };
            chunk.ClassID = *(uint32_t*)chunkData;
            chunkData += sizeof(uint32_t);
            uint32_t classNameLength = *(uint32_t*)chunkData;
            printf("class name length: %d\n", classNameLength);
            chunkData += sizeof(uint32_t);
            chunk.ClassName = chunkData;
            chunkData += classNameLength;
            chunk.HasService = *chunkData;
            chunkData++;
            chunk.Length = *(uint32_t*)chunkData;
            chunkData += sizeof(uint32_t);
            unsigned char *IDs = chunkData;

            printf("Instance Info:\n");
            printf("ClassID: %d\n", chunk.ClassID);
            printf("ClassName: %s\n", chunk.ClassName);
            printf("HasService: %d\n", chunk.HasService);
            printf("Length: %d\n", chunk.Length);
        }
        else if (!strncmp(chunk->Signature, "PROP", 4))
        {
            unsigned char *chunkData = data + 18;
            PropertiesChunk chunk = { 0 };

            chunk.ClassID = *(uint32_t*)chunkData;
            chunkData += sizeof(uint32_t);
            uint32_t nameLength = *(uint32_t*)chunkData;
            if (nameLength > 1000)
            {
                break;
            }
            printf("name length: %d\n", nameLength);
            chunkData += sizeof(uint32_t);
            chunk.Name = chunkData;

            printf("Property Info:\n");
            printf("ClassID: %d\n", chunk.ClassID);
            printf("Name: ");
            fwrite(chunk.Name, 1, nameLength, stdout);
            puts("");
        }
        else
        {
            printf("error: Unknown signature.\n");
        }
        data += sizeof(Chunk) + chunk->CompressedLength;
    }

    return ret;
}
