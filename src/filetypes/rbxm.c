#include "filetypes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <raylib.h>
#include "lz4.h"
#include "serialize.h"

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
    Instance **instances;
    SerializeInstance *serializeInstances;
} InstanceChunk;

typedef struct PropertiesChunk {
    int32_t ClassID;
    char *Name;
    uint8_t ValueType;
    void *Values;
} PropertiesChunk;

static void DecodeDifferenceInPlace(unsigned char *bytes, int length)
{
    for (int i = 1; i < length; i++)
    {
        bytes[i] += bytes[i-1];
    }
}

static int32_t RotateInt32(int32_t value)
{
    return (int32_t)((uint32_t)value >> 1) ^ (-(value & 1));
}

static void ReadInterleavedInt32(unsigned char *data, int count, int32_t (*transform)(int32_t), int32_t *values)
{
    int blobSize = count * sizeof(int32_t);

    union {
        unsigned char bytes[4];
        int32_t val;
    } work;

    for (int offset = 0; offset < count; offset++)
    {
        for (int i = 0; i < sizeof(int32_t); i++)
        {
            int index = (i * count) + offset;
            work.bytes[sizeof(int32_t) - i - 1] = data[index];
        }

        values[offset] = transform(work.val);
    }
}

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

    InstanceChunk *instChunks = NULL;
    int instChunkCount = 0;

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
        unsigned char *chunkDataCompressed = chunk->Payload;
        printf("\nChunk Info:\n");
        printf("Chunk signature: %.4s\n", chunk->Signature);
        printf("CompressedLength: %d\n", chunk->CompressedLength);
        printf("UncompressedLength: %d\n", chunk->UncompressedLength);
        unsigned char *chunkData = chunkDataCompressed;
        if (chunk->CompressedLength)
        {
            if (chunkDataCompressed[0] == 0x78 || chunkDataCompressed[0] == 0x58)
            {
                int dataSize;
                chunkData = DecompressData(chunkDataCompressed, chunk->CompressedLength, &dataSize);
                if (dataSize != chunk->UncompressedLength)
                {
                    printf("Error: mismatched compressed and uncompressed data sizes.\n");
                }
            }
            else if (!memcmp(chunkDataCompressed, (unsigned char [3]){0xB5, 0x2F, 0xFD}, 3))
            {
                printf("Unsupported compression type zstd.\n");
            }
            else
            {
                chunkData = malloc(chunk->UncompressedLength);
                LZ4_decompress_safe(chunkDataCompressed, chunkData, chunk->CompressedLength, chunk->UncompressedLength);
            }
        }
        if (!strncmp(chunk->Signature, "END\0", 4))
        {
            break;
        }
        else if (!strncmp(chunk->Signature, "INST", 4))
        {
            InstanceChunk chunk = { 0 };
            chunk.ClassID = *(int32_t*)chunkData;
            chunkData += sizeof(uint32_t);
            uint32_t classNameLength = *(uint32_t*)chunkData;
            chunkData += sizeof(uint32_t);
            chunk.ClassName = chunkData;
            chunkData += classNameLength;
            chunk.HasService = *chunkData;
            chunkData += sizeof(uint8_t);
            chunk.Length = *(uint32_t*)chunkData;
            chunkData += sizeof(uint32_t);
            chunk.IDs = malloc(sizeof(int32_t) * chunk.Length);
            ReadInterleavedInt32(chunkData, chunk.Length, RotateInt32, chunk.IDs);

            printf("Instance Info:\n");
            printf("ClassID: %d\n", chunk.ClassID);
            printf("ClassName: %s\n", chunk.ClassName);
            printf("HasService: %d\n", chunk.HasService);
            printf("Length: %d\n", chunk.Length);
            printf("IDs: ");
            for (int i = 0; i < chunk.Length; i++)
            {
                printf("%d ", chunk.IDs[i]);
            }
            puts("");

            chunk.instances = malloc(chunk.Length * sizeof(Instance *));
            chunk.serializeInstances = malloc(chunk.Length * sizeof(SerializeInstance));

            for (int i = 0; i < chunk.Length; i++)
            {
                Instance *inst = Instance_dynNew(chunk.ClassName, NULL);
                chunk.instances[i] = inst;
                Instance_Serialize(inst, &chunk.serializeInstances[i]);
            }

            instChunkCount++;
            instChunks = realloc(instChunks, instChunkCount * sizeof(InstanceChunk));
            instChunks[instChunkCount - 1] = chunk;
        }
        else if (!strncmp(chunk->Signature, "PROP", 4))
        {
            PropertiesChunk chunk = { 0 };

            chunk.ClassID = *(uint32_t*)chunkData;
            chunkData += sizeof(uint32_t);
            uint32_t nameLength = *(uint32_t*)chunkData;
            if (nameLength > 1000)
            {
                break;
            }
            chunkData += sizeof(uint32_t);
            chunk.Name = chunkData;
            chunkData += nameLength;
            chunk.ValueType = *(uint8_t*)chunkData;
            chunkData += sizeof(uint8_t);

            InstanceChunk instChunk;

            for (int i = 0; i < instChunkCount; i++)
            {
                if (chunk.ClassID == instChunks[i].ClassID)
                {
                    instChunk = instChunks[i];
                }
            }

            for (int i = 0; i < instChunk.Length; i++)
            {
                SerializeInstance *sInst = &(instChunk.serializeInstances[i]);
                for (int j = 0; j < sInst->serializationCount; j++)
                {
                    Serialization s = sInst->serializations[j];
                    if (!strcmp(s.name, chunk.Name))
                    {
                        switch (chunk.ValueType)
                        {
                            case 0x02:
                            {
                                bool *val = s.val;
                                *val = ((uint8_t*)chunkData)[i];
                            } break;
                            default:
                            {
                                printf("Error: unknown value type %#lx.\n", chunk.ValueType);
                            } break;
                        }
                    }
                }
            }

            printf("Property Info:\n");
            printf("ClassID: %d\n", chunk.ClassID);
            printf("Name: ");
            fwrite(chunk.Name, 1, nameLength, stdout);
            puts("");
            printf("Value type: %#x\n", chunk.ValueType);
        }
        else
        {
            printf("error: Unknown signature.\n");
        }
        data += sizeof(Chunk) + chunk->CompressedLength;
    }

    return ret;
}
