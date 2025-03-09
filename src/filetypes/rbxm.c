#include "filetypes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <raylib.h>
#include "lz4.h"
#include "serialize.h"
#include "part.h"
#include "script.h"
//#include <endian.h>
#include "zstd.h"

#include "debug.h"

DEFAULT_DEBUG_CHANNEL(rbxm);

// Resources:
// https://github.com/rojo-rbx/rbx-dom/blob/master/docs/binary.md
// https://github.com/MaximumADHD/Roblox-File-Format/blob/main/BinaryFormat/Chunks/SIGN.cs

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
    bool invalid;
} InstanceChunk;

typedef struct PropertiesChunk {
    int32_t ClassID;
    uint32_t nameLength;
    char *Name;
    uint8_t ValueType;
    void *Values;
} PropertiesChunk;

typedef struct ParentsChunk {
    uint8_t Reserved;
    uint32_t Length;
    int32_t *Children;
    int32_t *Parents;
} ParentsChunk;

typedef struct SharedStringValue {
    uint8_t Hash[16];
    uint32_t Length;
    uint8_t *Bytes;
} SharedStringValue;

typedef struct SharedStringChunk {
    int32_t Version;
    uint32_t Length;
    SharedStringValue *values;
} SharedStringChunk;

typedef struct RbxSignature {
    int32_t SignatureType;
    int64_t PublicKeyId;
    int32_t Length;
    uint8_t *Value;
} RbxSignature;

typedef struct SignChunk {
    int32_t NumSignatures;
    RbxSignature *Signatures;
} SignChunk;

static void DecodeDifferenceInPlace(int32_t *data, int length)
{
    for (int i = 1; i < length; i++)
    {
        data[i] += data[i-1];
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

static float ReturnFloat(float value)
{
    return value;
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

static void *parse_values(PropertiesChunk chunk, InstanceChunk instChunk, unsigned char *chunkData, int *vsize, SharedStringChunk sstrChunk)
{
    void *values;
    int valueSize;

    switch (chunk.ValueType)
    {
        case 0x01: // String value
        {
            return (void*)-1;
        } break;
        case 0x02: // Boolean value
        {
            values = chunkData;
            valueSize = sizeof(uint8_t);
        } break;
        case 0x04: // float value
        {
            values = malloc(sizeof(uint32_t) * instChunk.Length);
            ReadInterleavedfloat(chunkData, instChunk.Length, RotateFloat, values);
            valueSize = sizeof(float);
        } break;
        case 0x0B: // BrickColor value
        {
            values = malloc(sizeof(uint32_t) * instChunk.Length);
            valueSize = sizeof(uint32_t);
            ReadInterleaveduint32_t(chunkData, instChunk.Length, ReturnUInt32, values);
        } break;
        case 0x0C: // Color3 value
        {
            values = malloc(sizeof(Color3) * instChunk.Length);
            valueSize = sizeof(Color3);
            float *floatValues = malloc(sizeof(float) * instChunk.Length * 3);
            ReadInterleavedfloat(chunkData, instChunk.Length, RotateFloat, floatValues);
            ReadInterleavedfloat(chunkData + instChunk.Length*sizeof(float), instChunk.Length, RotateFloat, floatValues + instChunk.Length);
            ReadInterleavedfloat(chunkData + instChunk.Length*2*sizeof(float), instChunk.Length, RotateFloat, floatValues + instChunk.Length*2);
            for (int i = 0; i < instChunk.Length; i++)
            {
                ((Color3*)values)[i].R = floatValues[i];
                ((Color3*)values)[i].G = floatValues[i+instChunk.Length];
                ((Color3*)values)[i].B = floatValues[i+instChunk.Length*2];
            }
            free(floatValues);
        } break;
        case 0x0E: // Vector3 value
        {
            values = malloc(sizeof(Vector3) * instChunk.Length);
            valueSize = sizeof(Vector3);
            float *xValues = malloc(sizeof(float) * instChunk.Length);
            float *yValues = malloc(sizeof(float) * instChunk.Length);
            float *zValues = malloc(sizeof(float) * instChunk.Length);
            ReadInterleavedfloat(chunkData, instChunk.Length, RotateFloat, xValues);
            ReadInterleavedfloat(chunkData + instChunk.Length*sizeof(float), instChunk.Length, RotateFloat, yValues);
            ReadInterleavedfloat(chunkData + instChunk.Length*2*sizeof(float), instChunk.Length, RotateFloat, zValues);
            for (int i = 0; i < instChunk.Length; i++)
            {
                ((Vector3*)values)[i] = (Vector3){
                    xValues[i],
                    yValues[i],
                    zValues[i]
                };
            }
            free(xValues);
            free(yValues);
            free(zValues);
        } break;
        case 0x10: // CFrame value
        {
            values = malloc(sizeof(CFrame) * instChunk.Length);
            valueSize = sizeof(CFrame);
            float (**rotations) = malloc(sizeof(float[9]) * instChunk.Length);
            for (int i = 0; i < instChunk.Length; i++)
            {
                uint8_t rotId = *(uint8_t*)chunkData;
                chunkData += sizeof(uint8_t);
                if (!rotId)
                {
                    rotations[i] = chunkData;
                    chunkData += sizeof(float[9]);
                }
                else
                {
                    rotations[i] = rotationIDList[rotId];
                }
                ((CFrame*)values)[i].R00 = rotations[i][0];
                ((CFrame*)values)[i].R01 = rotations[i][1];
                ((CFrame*)values)[i].R02 = rotations[i][2];
                ((CFrame*)values)[i].R10 = rotations[i][3];
                ((CFrame*)values)[i].R11 = rotations[i][4];
                ((CFrame*)values)[i].R12 = rotations[i][5];
                ((CFrame*)values)[i].R20 = rotations[i][6];
                ((CFrame*)values)[i].R21 = rotations[i][7];
                ((CFrame*)values)[i].R22 = rotations[i][8];
            }
            float *positions = malloc(sizeof(float) * instChunk.Length * 3);
            ReadInterleavedfloat(chunkData, instChunk.Length, RotateFloat, positions);
            ReadInterleavedfloat(chunkData + instChunk.Length*sizeof(float), instChunk.Length, RotateFloat, positions + instChunk.Length);
            ReadInterleavedfloat(chunkData + instChunk.Length*2*sizeof(float), instChunk.Length, RotateFloat, positions + instChunk.Length*2);
            for (int i = 0; i < instChunk.Length; i++)
            {
                ((CFrame*)values)[i].X = positions[i];
                ((CFrame*)values)[i].Y = positions[i+instChunk.Length];
                ((CFrame*)values)[i].Z = positions[i+instChunk.Length*2];
            }
            free(rotations);
            free(positions);
        } break;
        case 0x12: // Token value
        {
            valueSize = sizeof(int);
            values = malloc(sizeof(int) * instChunk.Length);
            ReadInterleaveduint32_t(chunkData, instChunk.Length, ReturnUInt32, values);
            
        } break;
        case 0x1A: // Color3uint8 value
        {
            valueSize = sizeof(Color3);
            values = malloc(sizeof(Color3) * instChunk.Length);
            uint8_t *uint8Values = malloc(3 * instChunk.Length);
            ReadInterleaveduint8_t(chunkData, instChunk.Length, ReturnUInt8, uint8Values);
            ReadInterleaveduint8_t(chunkData + instChunk.Length, instChunk.Length, ReturnUInt8, uint8Values + instChunk.Length);
            ReadInterleaveduint8_t(chunkData + instChunk.Length*2, instChunk.Length, ReturnUInt8, uint8Values + instChunk.Length*2);
            for (int i = 0; i < instChunk.Length; i++)
            {
                ((Color3*)values)[i] = (Color3){
                    (float)uint8Values[i] / 255,
                    (float)uint8Values[i+instChunk.Length] / 255,
                    (float)uint8Values[i+instChunk.Length*2] / 255,
                };
            }
            free(uint8Values);
        } break;
        case 0x1C: // SharedString value
        {
            valueSize = sizeof(char*);
            values = malloc(sizeof(char*) * instChunk.Length);
            uint32_t *indices = malloc(sizeof(uint32_t) * instChunk.Length);
            ReadInterleaveduint32_t(chunkData, instChunk.Length, ReturnUInt32, indices);
            for (int i = 0; i < instChunk.Length; i++)
            {
                char *sstr = sstrChunk.values[indices[i]].Bytes;
                ((char**)values)[i] = calloc(sstrChunk.values[indices[i]].Length, 1);
                printf("string %d: copy %d bytes\n", i, sstrChunk.values[indices[i]].Length);
                memcpy(((char**)values)[i], sstr, sstrChunk.values[indices[i]].Length);
            }
            free(indices);
        } break;
        default:
        {
            printf("Error: unknown value type %#x.\n", chunk.ValueType);
            return NULL;
        } break;
    }

    *vsize = valueSize;
    return values;
}

static void apply_property_chunk_to_instances(PropertiesChunk chunk, InstanceChunk instChunk, unsigned char *chunkData, SharedStringChunk sstrChunk)
{
    int valueSize;
    void *values = parse_values(chunk, instChunk, chunkData, &valueSize, sstrChunk);

    if (!values) return;

    for (int i = 0; i < instChunk.Length; i++)
    {
        SerializeInstance *sInst = &(instChunk.serializeInstances[i]);
        Instance *inst = instChunk.instances[i];
        if (!inst) continue;
        bool serialized = false;
        for (int j = 0; j < sInst->serializationCount; j++)
        {
            Serialization s = sInst->serializations[j];
            if (!strncmp(s.name, chunk.Name, chunk.nameLength) ||
                (!strcmp(s.name, "Color") && !strncmp(chunk.Name, "Color3uint8", chunk.nameLength)))
            {
                serialized = true;
                if (chunk.ValueType == 0x01)
                {
                    char **val = s.val;
                    unsigned char *strData;
                    unsigned char *cDataCopy = chunkData;
                    uint32_t length;
                    for (int j = 0; j < instChunk.Length; j++)
                    {
                        length = *(uint32_t*)cDataCopy;
                        cDataCopy += sizeof(uint32_t);
                        if (j == i)
                        {
                            strData = cDataCopy;
                            break;
                        }
                        cDataCopy += length;
                    }
                    *val = malloc(length + 1);
                    memcpy(*val, strData, length);
                    (*val)[length] = 0;
                    if (Instance_IsA(inst, "Script") && !strcmp(s.name, "Source"))
                    {
                        ((Script*)inst)->sourceLength = length;
                    }
                }
                else
                {
                    memcpy(s.val, values + valueSize * i, valueSize);
                }
                if (Instance_IsA(inst,"BasePart"))
                {
                    if (!strcmp(s.name, "Position"))
                    {
                        BasePart_SetPosition(inst, ((BasePart*)inst)->Position);
                    }
                    else if (!strcmp(s.name, "CFrame"))
                    {
                        BasePart_SetCFrame(inst, ((BasePart*)inst)->CFrame);
                    }
                    else if (!strcmp(s.name, "Color"))
                    {
                        BasePart_SetColor(inst, ((BasePart*)inst)->Color);
                    }
                    else if (!strcmp(s.name, "BrickColor"))
                    {
                        BasePart_SetBrickColor(inst, ((BasePart*)inst)->BrickColor);
                    }
                }
                if (Instance_IsA(inst, "Part") && !strncmp(chunk.Name, "shape", chunk.nameLength))
                {
                    Part_SetShape(inst, ((Part*)inst)->shape);
                }
            }
        }
        if (!serialized)
        {
            FIXME("Not serialized: ClassName %s, Property %.*s\n", inst->ClassName, chunk.nameLength, chunk.Name);
        }
    }

    if (values != -1 && values != chunkData) free(values);
}

static int32_t *LoadReferences(int length, unsigned char *data)
{
    int32_t *references = malloc(sizeof(int32_t) * length);
    ReadInterleavedint32_t(data, length, RotateInt32, references);
    DecodeDifferenceInPlace(references, length);
    return references;
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
    void *initData = data;
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

    SharedStringChunk sstrChunk = { 0 };

    while (1)
    {
        Chunk *chunk = data;
        unsigned char *chunkDataCompressed = chunk->Payload;
        printf("\nChunk Info:\n");
        printf("Chunk signature: %.4s\n", chunk->Signature);
        printf("CompressedLength: %d\n", chunk->CompressedLength);
        printf("UncompressedLength: %d\n", chunk->UncompressedLength);
        unsigned char *chunkData = chunkDataCompressed;
        void *chunkDataOrig = chunkData;
        if (chunk->CompressedLength)
        {
            if (chunkDataCompressed[0] == 0x78 || chunkDataCompressed[0] == 0x58)
            {
                int dataSize;
                chunkData = DecompressData(chunkDataCompressed, chunk->CompressedLength, &dataSize);
                chunkDataOrig = chunkData;
                if (dataSize != chunk->UncompressedLength)
                {
                    printf("Error: mismatched compressed and uncompressed data sizes.\n");
                }
            }
            else if (*(uint32_t*)chunkDataCompressed == 0xfd2fb528)
            {
                chunkData = malloc(chunk->UncompressedLength);
                chunkDataOrig = chunkData;
                ZSTD_decompress(chunkData, chunk->UncompressedLength, chunkDataCompressed, chunk->CompressedLength);
            }
            else
            {
                chunkData = malloc(chunk->UncompressedLength);
                chunkDataOrig = chunkData;
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
            chunk.ClassName = malloc(classNameLength+1);
            memcpy(chunk.ClassName, chunkData, classNameLength);
            chunk.ClassName[classNameLength] = 0;
            chunkData += classNameLength;
            chunk.HasService = *chunkData;
            chunkData += sizeof(uint8_t);
            chunk.Length = *(uint32_t*)chunkData;
            chunkData += sizeof(uint32_t);
            chunk.IDs = LoadReferences(chunk.Length, chunkData);

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
                if (!inst)
                {
                    printf("Unknown ClassName %s.\n", chunk.ClassName);
                    chunk.invalid = true;
                }
                chunk.instances[i] = inst;
                if (chunk.invalid) continue;
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

            apply_property_chunk_to_instances(chunk, instChunk, chunkData, sstrChunk);

        }
        else if (!strncmp(chunk->Signature, "PRNT", 4))
        {
            ParentsChunk chunk = { 0 };

            chunkData += sizeof(uint8_t);
            chunk.Length = *(uint32_t*)chunkData;
            chunkData += sizeof(uint32_t);

            chunk.Children = LoadReferences(chunk.Length, chunkData);
            chunkData += sizeof(int32_t) * chunk.Length;
            chunk.Parents = LoadReferences(chunk.Length, chunkData);

            printf("Parents info:\n");
            printf("Length: %d.\n", chunk.Length);
            for (int i = 0; i < chunk.Length; i++)
            {
                Instance *child = NULL;
                Instance *parent = NULL;
                printf("%d is parented to %d.\n", chunk.Children[i], chunk.Parents[i]);

                for (int j = 0; j < instChunkCount; j++)
                {
                    InstanceChunk instChunk = instChunks[j];
                    if (instChunk.invalid) continue;
                    for (int k = 0; k < instChunk.Length; k++)
                    {
                        if (instChunk.IDs[k] == chunk.Children[i]) child = instChunk.instances[k];
                        if (instChunk.IDs[k] == chunk.Parents[i]) parent = instChunk.instances[k];
                    }
                }

                if (chunk.Parents[i] != -1 && (!child || !parent))
                {
                    printf("Unable to find instance. Instance will be parented to datamodel.\n");
                }

                if (!child) continue;
                if (Instance_IsAncestorOf(child, parent) || Instance_IsDescendantOf(parent, child))
                {
                    printf("This will cause an infinite loop. Parenting to datamodel.\n");
                    parent = NULL;
                }

                if (chunk.Parents[i] == -1 || !parent)
                {
                    (*mdlCount)++;
                    ret = realloc(ret, sizeof(Instance *) * *mdlCount);
                    ret[*mdlCount - 1] = child;
                }
                else
                {
                    Instance_SetParent(child, parent);
                }
            }

            free(chunk.Children);
            free(chunk.Parents);
        }
        else if (!strncmp(chunk->Signature, "SSTR", 4))
        {
            SharedStringChunk chunk = { 0 };

            chunk.Version = *(int32_t*)chunkData;
            chunkData += sizeof(int32_t);

            chunk.Length = *(uint32_t*)chunkData;
            chunkData += sizeof(uint32_t);

            chunk.values = malloc(sizeof(SharedStringValue) * chunk.Length);
            for (int i = 0; i < chunk.Length; i++)
            {
                SharedStringValue val = { 0 };
                chunkData += 16; // hash
                val.Length = *(uint32_t*)chunkData;
                chunkData += sizeof(uint32_t);
                val.Bytes = malloc(val.Length);
                memcpy(val.Bytes, chunkData, val.Length);
                chunkData += val.Length;
                chunk.values[i] = val;
            }

            sstrChunk = chunk;
        }
        else if (!strncmp(chunk->Signature, "SIGN", 4))
        {
            SignChunk chunk = { 0 };

            chunk.NumSignatures = *(int32_t*)chunkData;
            chunkData += sizeof(int32_t);

            chunk.Signatures = malloc(sizeof(RbxSignature) * chunk.NumSignatures);

            printf("%d Signatures\n", chunk.NumSignatures);

            for (int i = 0; i < chunk.NumSignatures; i++)
            {

                printf("Signature %d:\n", i);
                chunk.Signatures[i].SignatureType = *(int32_t*)chunkData;
                chunkData += sizeof(int32_t);

                chunk.Signatures[i].PublicKeyId = *(int64_t*)chunkData;
                chunkData += sizeof(int64_t);

                chunk.Signatures[i].Length = *(int32_t*)chunkData;
                chunkData += sizeof(int32_t);

                printf("SignatureType: %#x\n", chunk.Signatures[i].SignatureType);
                printf("PublicKeyId: %#x\n", chunk.Signatures[i].PublicKeyId);
                printf("Length: %#x\n", chunk.Signatures[i].Length);

                chunk.Signatures[i].Value = malloc(chunk.Signatures[i].Length);
                memcpy(chunk.Signatures[i].Value, chunkData, chunk.Signatures[i].Length);
                chunkData += chunk.Signatures[i].Length;
            }
        }
        else
        {
            printf("error: Unknown signature.\n");
        }
        data += sizeof(Chunk) + chunk->CompressedLength;
        if (!chunk->CompressedLength)
        {
            data += chunk->UncompressedLength;
        }
        if (chunkDataOrig != chunkDataCompressed) free(chunkDataOrig);
    }

    // unload...
    for (int i = 0; i < instChunkCount; i++)
    {
        for (int j = 0; j < instChunks[i].Length; j++)
        {
            free(instChunks[i].serializeInstances[j].serializations);
        }
        free(instChunks[i].serializeInstances);
        free(instChunks[i].IDs);
        free(instChunks[i].instances);
    }
    free(instChunks);
    free(initData);

    return ret;
}
