#include "filetypes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <raylib.h>
#include "lz4.h"
#include "serialize.h"
#include "part.h"

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
    uint32_t nameLength;
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

static uint32_t ReturnUInt32(uint32_t value)
{
    return value;
}

static float RotateFloat(float value)
{
    uint32_t u = *(uint32_t*)&value;
    uint32_t i = (u >> 1) | (u << 31);
    return *(float*)&i;
}

static uint8_t ReturnUInt8(uint8_t value)
{
    return value;
}

#define defreadinterleavedfunc(T) \
static void ReadInterleaved##T(unsigned char *data, int count, T (*transform)(T), T *values) \
{ \
    int blobSize = count * sizeof(T); \
\
    union {\
        unsigned char bytes[sizeof(T)];\
        T val;\
    } work;\
\
    for (int offset = 0; offset < count; offset++)\
    {\
        for (int i = 0; i < sizeof(T); i++)\
        {\
            int index = (i * count) + offset;\
            work.bytes[sizeof(T) - i - 1] = data[index];\
        }\
\
        values[offset] = transform(work.val);\
    }\
}\

defreadinterleavedfunc(int32_t)
defreadinterleavedfunc(uint32_t)
defreadinterleavedfunc(float)
defreadinterleavedfunc(uint8_t)

static void binserialize_token(int *val, unsigned char *prop, int instance, int instanceCount)
{
    //[]uint32b~4
    //32-bit unsigned integer dynamic array, big-endian, encoded by interleaving with a byte size of 4.

    uint32_t *values = malloc(sizeof(uint32_t) * instanceCount);

    ReadInterleaveduint32_t(prop, instanceCount, ReturnUInt32, values);

    *val = values[instance];

}

static void binserialize_float(float *val, unsigned char *prop, int instance, int instanceCount)
{
    float *values = malloc(sizeof(float) * instanceCount);

    ReadInterleavedfloat(prop, instanceCount, RotateFloat, values);

    *val = values[instance];

    printf("Value: %f\n", *val);
}

static void binserialize_vector3(Vector3 *val, unsigned char *prop, int instance, int instanceCount)
{
    float *values = malloc(sizeof(float) * instanceCount * 3);

    ReadInterleavedfloat(prop, instanceCount * 3, RotateFloat, values);

    int offset = instance * 3;

    val->x = values[0 + offset];
    val->y = values[1 + offset];
    val->z = values[2 + offset];

    printf("arr: ");
    for (int i = 0; i < instanceCount * 3; i++)
    {
        printf("%f, ", values[i]);
    }
    puts("");

    printf("Value: %s\n", debugstr_vector3(*val));
}

static float rotationIDList[][9] = {
    [0x02] = {+1, +0, +0, +0, +1, +0, +0, +0, +1},
    [0x03] = {+1, +0, +0, +0, +0, -1, +0, +1, +0},
    [0x05] = {+1, +0, +0, +0, -1, +0, +0, +0, -1},
    [0x06] = {+1, +0, -0, +0, +0, +1, +0, -1, +0},
    [0x07] = {+0, +1, +0, +1, +0, +0, +0, +0, -1},
    [0x09] = {+0, +0, +1, +1, +0, +0, +0, +1, +0},
    [0x0A] = {+0, -1, +0, +1, +0, -0, +0, +0, +1},
    [0x0C] = {+0, +0, -1, +1, +0, +0, +0, -1, +0},
    [0x0D] = {+0, +1, +0, +0, +0, +1, +1, +0, +0},
    [0x0E] = {+0, +0, -1, +0, +1, +0, +1, +0, +0},
    [0x10] = {+0, -1, +0, +0, +0, -1, +1, +0, +0},
    [0x11] = {+0, +0, +1, +0, -1, +0, +1, +0, -0},
    [0x14] = {-1, +0, +0, +0, +1, +0, +0, +0, -1},
    [0x15] = {-1, +0, +0, +0, +0, +1, +0, +1, -0},
    [0x17] = {-1, +0, +0, +0, -1, +0, +0, +0, +1},
    [0x18] = {-1, +0, -0, +0, +0, -1, +0, -1, -0},
    [0x19] = {+0, +1, -0, -1, +0, +0, +0, +0, +1},
    [0x1B] = {+0, +0, -1, -1, +0, +0, +0, +1, +0},
    [0x1C] = {+0, -1, -0, -1, +0, -0, +0, +0, -1},
    [0x1E] = {+0, +0, +1, -1, +0, +0, +0, -1, +0},
    [0x1F] = {+0, +1, +0, +0, +0, -1, -1, +0, +0},
    [0x20] = {+0, +0, +1, +0, +1, -0, -1, +0, +0},
    [0x22] = {+0, -1, +0, +0, +0, +1, -1, +0, +0},
    [0x23] = {+0, +0, -1, +0, -1, -0, -1, +0, -0},
};

static void binserialize_cframe(CFrame *val, unsigned char *prop, int instance, int instanceCount)
{
    for (int i = 0; i < instanceCount; i++)
    {
        uint8_t rotId = *(uint8_t*)prop;
        float *rots;
        prop += sizeof(uint8_t);
        if (!rotId)
        {
            rots = (float*)prop;
            prop += sizeof(float) * 9;
        }
        else
        {
            rots = rotationIDList[rotId];
        }

        if (i == instance)
        {
            val->R00 = rots[0];
            val->R01 = rots[1];
            val->R02 = rots[2];
            val->R10 = rots[3];
            val->R11 = rots[4];
            val->R12 = rots[5];
            val->R20 = rots[6];
            val->R21 = rots[7];
            val->R22 = rots[8];
        }
    }

    Vector3 pos = {0};

    binserialize_vector3(&pos, prop, instance, instanceCount);

    val->X = pos.x;
    val->Y = pos.y;
    val->Z = pos.z;
}

static void binserialize_color3uint8(Color3 *val, unsigned char *prop, int instance, int instanceCount)
{
    uint8_t *values = malloc(sizeof(uint8_t) * instanceCount * 
    3);

    ReadInterleaveduint8_t(prop, instanceCount * 3, ReturnUInt8, values);

    val->R = (float)values[instance*3] / 255;
    val->G = (float)values[instance*3+1] / 255;
    val->B = (float)values[instance*3+2] / 255;
}

static void apply_property_chunk_to_instances(PropertiesChunk chunk, InstanceChunk instChunk, unsigned char *chunkData)
{
    int valSize;

    for (int i = 0; i < instChunk.Length; i++)
    {
        SerializeInstance *sInst = &(instChunk.serializeInstances[i]);
        Instance *inst = instChunk.instances[i];
        for (int j = 0; j < sInst->serializationCount; j++)
        {
            Serialization s = sInst->serializations[j];
            if (!strncmp(s.name, chunk.Name, chunk.nameLength) ||
                (!strcmp(s.name, "Color") && !strncmp(chunk.Name, "Color3uint8", chunk.nameLength)))
            {
                switch (chunk.ValueType)
                {
                    case 0x02: // Boolean value
                    {
                        bool *val = s.val;
                        *val = ((uint8_t*)chunkData)[i];
                    } break;
                    case 0x04: // float value
                    {
                        binserialize_float(s.val, chunkData, i, instChunk.Length);
                    } break;
                    case 0x0E:
                    {
                        binserialize_vector3(s.val, chunkData, i, instChunk.Length);
                        if (Instance_IsA(inst, "BasePart") && !strncmp(chunk.Name, "Position", chunk.nameLength))
                        {
                            BasePart_SetPosition(inst, ((BasePart*)inst)->Position);
                        }
                    } break;
                    case 0x10: // CFrame value
                    {
                        binserialize_cframe(s.val, chunkData, i, instChunk.Length);
                        if (Instance_IsA(inst, "BasePart") && !strncmp(chunk.Name, "CFrame", chunk.nameLength))
                        {
                            BasePart_SetCFrame(inst, ((BasePart*)inst)->CFrame);
                        }
                    } break;
                    case 0x12: // Token value
                    {
                        binserialize_token(s.val, chunkData, i, instChunk.Length);
                        if (Instance_IsA(inst, "Part") && !strncmp(chunk.Name, "shape", chunk.nameLength))
                        {
                            Part_SetShape(inst, ((Part*)inst)->shape);
                        }
                    } break;
                    case 0x1A: // Color3uint8 value
                    {
                        binserialize_color3uint8(s.val, chunkData, i, instChunk.Length);
                        if (Instance_IsA(inst, "BasePart") && !strcmp(s.name, "Color"))
                        {
                            BasePart_SetColor(inst, ((BasePart*)inst)->Color);
                        }
                    } break;
                    default:
                    {
                        printf("Error: unknown value type %#lx.\n", chunk.ValueType);
                    } break;
                }
            }
        }
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
            ReadInterleavedint32_t(chunkData, chunk.Length, RotateInt32, chunk.IDs);

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
            chunk.serializeInstances = calloc(chunk.Length, sizeof(SerializeInstance));

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
            chunk.nameLength = *(uint32_t*)chunkData;
            chunkData += sizeof(uint32_t);
            chunk.Name = chunkData;
            chunkData += chunk.nameLength;
            chunk.ValueType = *(uint8_t*)chunkData;
            chunkData += sizeof(uint8_t);

            printf("Property Info:\n");
            printf("ClassID: %d\n", chunk.ClassID);
            printf("Name: ");
            fwrite(chunk.Name, 1, chunk.nameLength, stdout);
            puts("");
            printf("Value type: %#x\n", chunk.ValueType);

            InstanceChunk instChunk;

            for (int i = 0; i < instChunkCount; i++)
            {
                if (chunk.ClassID == instChunks[i].ClassID)
                {
                    printf("Has corresponding INST chunk\n");
                    instChunk = instChunks[i];
                }
            }

            apply_property_chunk_to_instances(chunk, instChunk, chunkData);

        }
        else
        {
            printf("error: Unknown signature.\n");
        }
        data += sizeof(Chunk) + chunk->CompressedLength;
    }

    for (int i = 0; i < instChunkCount; i++)
    {
        for (int j = 0; j < instChunks[i].Length; j++)
        {
            (*mdlCount)++;
            ret = realloc(ret, sizeof(Instance *) * *mdlCount);
            ret[*mdlCount - 1] = instChunks->instances[j];
        }
    }

    return ret;
}
