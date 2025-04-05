#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <Luau/Bytecode.h>
#include "lua.h"
#include <stdbool.h>

static void scrt_debugstep(lua_State *L, lua_Debug *ar)
{
    if (ar->what)
    {
        printf("%s:%d\n", ar->what, ar->currentline);
    }
}

static void scrt_interrupt(lua_State *L, int gc)
{
    printf("gc %d\n", gc);
}

static unsigned int readVarInt(char **pData)
{
    unsigned result = 0, shift = 0;
    uint8_t byte;
    char *data = *pData;

    do {
        byte = *(uint8_t*)data;
        data++;
        result |= (byte & 127) << shift;
        shift += 7;
    } while (byte & 128);

    *pData = data;

    return result;
}

static void remapUserdataTypes(char *data, size_t size, uint32_t count)
{
    char *initData = data;
    uint32_t typeSize = readVarInt(&data);
    uint32_t upvalCount = readVarInt(&data);
    uint32_t localCount = readVarInt(&data);

    printf("remap Userdata Types:\n");
    printf("Type size: %d\n", typeSize);
    if (typeSize)
    {
        uint8_t* types = data;

        for (int i = 2; i < typeSize; i++)
        {
            uint32_t index = *data;
            data -= LBC_TYPE_TAGGED_USERDATA_BASE;

            printf("type %d: %d\n", i, index);
        }

        data += typeSize;
    }

    printf("upval Count: %d\n", upvalCount);
    if (upvalCount)
    {
        uint8_t *types = data;
        for (int i = 0; i < upvalCount; i++)
        {
            uint32_t index = ((uint32_t)types[i]) - LBC_TYPE_TAGGED_USERDATA_BASE;

            printf("upval %d: %d\n", i, index);
        }
        data += upvalCount;
    }

    printf("local Count: %d\n", localCount);
    if (localCount)
    {
        for (int i = 0; i < localCount; i++)
        {
            uint32_t index = *data;
            index -= LBC_TYPE_TAGGED_USERDATA_BASE;
            printf("local %d: %d\n", i, index);

            data += 2;
            readVarInt(&data);
            readVarInt(&data);

        }
    }

    if (initData + size != data)
    {
        printf("data size mismatch: size %d, read %d\n", size, data - initData);
    }
}

static char *readString(uint8_t **data, char **strings)
{
    int id = readVarInt(data);
    printf("rs %d\n", id);
    if (id == 0) return NULL;
    return strings[id - 1];
}

// sign extend 24 to 32 bits
int32_t se24_32(uint32_t x)
{
    if (x >> 24)
    {
        x |= 0xff000000;
    }
    return x;
}

typedef struct LuauConstant {
    int type;
    void *val;
    int constantCount;
} LuauConstant;

static void print_constant(LuauConstant *constants, unsigned int i, bool putquotes)
{
    if (constants->constantCount <= i)
    {
        printf("KInvalid_%d", i);
        return;
    }

    if (constants[i].type == LBC_CONSTANT_STRING)
    {
        if (putquotes)
        {
            printf("\"%s\"", (char*)(constants[i].val));
        }
        else
        {
            printf("%s", (char*)(constants[i].val));
        }
    }
    else
    {
        printf("K%d", i);
    }
}

// Adapted from Luau/Bytecode.h documentation
static void disassemblecode(uint32_t *code, int codeSize, LuauConstant *constants) 
{
    for (int i = 0; i < codeSize; i++)
    {
        printf("%05x:", i);
        uint8_t opcode = code[i] & 0xff;
        uint8_t a = (code[i] >>  8) & 0xff;
        uint8_t b = (code[i] >> 16) & 0xff;
        uint8_t c = (code[i] >> 24) & 0xff;
        int16_t d = (code[i] >> 16) & 0xffff;
        int32_t e = se24_32(code[i] >> 8);
        uint32_t aux = 0;

        if (i != codeSize - 1)
        {
            aux = code[i+1];
        }

        switch (opcode)
        {
            case LOP_NOP: // 0
            {
                printf("NOP\n");
            } break;
            case LOP_BREAK: // 1
            {
                printf("BREAK\n");
            } break;
            case LOP_LOADNIL: // 2
            {
                printf("R%d = nil\n", a);
            } break;
            case LOP_LOADB: // 3
            {
                printf("R%d = %s", a, b?"true":"false");
                if (c)
                {
                    printf(", goto %#x\n", i+c);
                }
                else
                {
                    printf("\n");
                }
            } break;
            case LOP_LOADN: // 4
            {
                printf("R%d = %d\n", a, d);
            } break;
            case LOP_LOADK: // 5
            {
                printf("R%d = ", a);
                print_constant(constants, d, true);
                printf("\n");
            } break;
            case LOP_MOVE: // 6
            {
                printf("R%d = R%d\n", a, b);
            } break;
            case LOP_GETGLOBAL: // 7
            {
                printf("GETGLOBAL %d %d %#x\n", a, c, aux);
                i++;
            } break;
            case LOP_SETGLOBAL: // 8
            {
                printf("SETGLOBAL %d %d %#x\n", a, c, aux);
                i++;
            } break;
            case LOP_GETUPVAL: // 9
            {
                printf("R%d = upval %d\n", a, b);
            } break;
            case LOP_SETUPVAL: // 10
            {
                printf("SETUPVAL %d %d\n", a, b);
            } break;
            case LOP_CLOSEUPVALS: // 11
            {
                printf("CLOSEUPVALS %d\n", a);
            } break;
            case LOP_GETIMPORT: // 12
            {
                int pcount = aux >> 30;
                int p3 = aux & 1023;
                int p2 = (aux >> 10) & 1023;
                int p1 = (aux >> 20) & 1023;
        
                printf("R%d = ", a);

                if (pcount > 0)
                {
                    print_constant(constants, p1, false);
                    if (pcount > 1)
                    {
                        printf(".");
                        print_constant(constants, p2, false);
                        if (pcount > 2)
                        {
                            printf(".");
                            print_constant(constants, p3, false);
                        }
                    }
                }
                printf("\n");

                i++;
            } break;
            case LOP_GETTABLE: // 13
            {
                printf("R%d = R%d[R%d]\n", a, b, c);
            } break;
            case LOP_SETTABLE: // 14
            {
                printf("R%d[R%d] = R%d\n", b, c, a);
            } break;
            case LOP_GETTABLEKS: // 15
            {
                printf("R%d = R%d[", a, b);
                print_constant(constants, aux, true);
                printf("]\n");
                i++;
            } break;
            case LOP_SETTABLEKS: // 16
            {
                printf("R%d[", b);
                print_constant(constants, aux, true);
                printf("] = R%d\n", a);
                i++;
            } break;
            //LOP_GETTABLEN  17
            //LOP_SETTABLEN  18
            case LOP_NEWCLOSURE: // 19
            {
                printf("R%d = new closure from %d\n", a, d);
            } break;
            case LOP_NAMECALL: // 20
            {
                printf("R%d = R%d, R%d = R%d[", a+1, b, a, b);
                print_constant(constants, aux, true);
                printf("]\n");
                i++;
            } break;
            case LOP_CALL: // 21
            {
                if (c == 0)
                {
                    printf("... = ");
                }
                else if (c == 2)
                {
                    printf("R%d = ", a);
                }
                printf("R%d(", a);
                if (b == 0)
                {
                    printf("...");
                }
                for (int j = a+1; j < a+b; j++)
                {
                    printf("R%d", j);
                    if (j != a+b-1)
                    {
                        printf(", ");
                    }
                }
                printf(") -> %d rets\n", c-1);
            } break;
            case LOP_RETURN: // 22
            {
                if (b == 1)
                {
                    printf("return\n");
                }
                else if (b > 0)
                {
                    printf("return ");
                    for (int j = a; j < a+b-1; j++)
                    {
                        printf("R%d", j);
                        if (j != a+b-2) printf(", ");
                    }
                    printf("\n");
                }
                else
                {
                    printf("RETURN %d %d\n", a, b);
                }
            } break;
            case LOP_JUMP: // 23
            {
                printf("go to %#x\n", i+d);
            } break;
            //LOP_JUMPBACK   24
            case LOP_JUMPIF: // 25
            {
                printf("if R%d: go to %#x\n", a, i+d+1);
            } break;
            case LOP_JUMPIFNOT: // 26
            {
                printf("if not R%d: go to %#x\n", a, i+d+1);
            } break;
            case LOP_JUMPIFEQ: // 27
            {
                printf("if R%d == R%d: go to %#x\n", a, aux, i+d+1);
                i++;
            } break;
            //LOP_JUMPIFLE   28
            //LOP_JUMPIFLT   29
            case LOP_JUMPIFNOTEQ: // 30
            {
                printf("if R%d ~= R%d: go to %#x\n", a, aux, d);
                i++;
            } break;
            //LOP_JUMPIFNOTLE 31
            //LOP_JUMPIFNOTLT 32
            //LOP_ADD        33
            case LOP_SUB: // 34
            {
                printf("R%d = R%d - R%d\n", a, b, c);
            } break;
            //LOP_MUL        35
            //LOP_DIV        36
            //LOP_MOD        37
            //LOP_POW        38
            //LOP_ADDK       39
            case LOP_SUBK: // 40
            {
                printf("R%d = R%d - ", a, b);
                print_constant(constants, c, true);
                printf("\n");
            } break;
            //LOP_MULK       41
            //LOP_DIVK       42
            //LOP_MODK       43
            //LOP_POWK       44
            //LOP_AND        45
            //LOP_OR         46
            //LOP_ANDK       47
            case LOP_ORK: // 48
            {
                printf("ORK %d %d %d\n", a, b, c);
            } break;
            case LOP_CONCAT: // 49
            {
                printf("R%d = ", a);
                for (int j = b; j <= c; j++)
                {
                    printf("R%d", j);
                    if (j != c)
                        printf(" .. ");
                }
                printf("\n");
            } break;
            //LOP_NOT        50
            //LOP_MINUS      51
            //LOP_LENGTH     52
            case LOP_NEWTABLE: // 53
            {
                printf("R%d = {}, arrsz=%d, tsz=%d\n", a, aux, b);
                i++;
            } break;
            case LOP_DUPTABLE: // 54
            {
                printf("R%d = ", a);
                print_constant(constants, d, true);
                printf("(duptable)\n");
            } break;
            //LOP_SETLIST    55
            //LOP_FORNPREP   56
            //LOP_FORNLOOP   57
            case LOP_FORGLOOP: // 58
            {
                printf("(FORGLOOP) ");
                for (int j = a+3; j < a+3+(aux&0xff); j++)
                {
                    printf("R%d", j);
                    if (j != a+2+(aux&0xff))
                        printf(", ");
                }
                printf(" = R%d(R%d, R%d), goto %#x\n", a, a+1, a+2, i+d+1);
                i++;
            } break;
            //LOP_FORGPREP_INEXT 59
            //LOP_FASTCALL3  60
            case LOP_FORGPREP_NEXT: // 61
            {
                printf("R%d = nil, R%d = nil, go to FORGLOOP\n", a+3, a+4);
            } break;
            //LOP_NATIVECALL 62
            case LOP_GETVARARGS: // 63
            {
                printf("GETVARARGS %d %d\n", a, b);
            } break;
            case LOP_DUPCLOSURE: // 64
            {
                printf("R%d = K%d (closure)\n", a, d);
            } break;
            case LOP_PREPVARARGS: // 65
            {
                printf("PREPVARARGS %d\n", a);
            } break;
            //LOP_LOADKX     66
            //LOP_JUMPX      67
            case LOP_FASTCALL: // 68
            {
                printf("FASTCALL %d %d\n", a, c);
            } break;
            //LOP_COVERAGE   69
            case LOP_CAPTURE: // 70
            {
                if (a == LCT_VAL)
                {
                    printf("capture val R%d\n", b);
                }
                else if (a == LCT_REF)
                {
                    printf("capture ref R%d\n", b);
                }
                else if (a == LCT_UPVAL)
                {
                    printf("capture upval %d\n", b);
                }
            } break;
            case LOP_SUBRK: // 71
            {
                printf("SUBRK %d %d %d\n", a, b, c);
            } break;
            //LOP_DIVRK       72
            //LOP_FASTCALL1   73
            case LOP_FASTCALL2: // 74
            {
                printf("FASTCALL2 %d %d %d %d\n", a, b, c, aux);
                i++;
            } break;
            case LOP_FASTCALL2K: // 75
            {
                printf("FASTCALL2K %d %d %d %d\n", a, b, c, aux);
                i++;
            } break;
            //LOP_FORGPREP    76
            case LOP_JUMPXEQKNIL: // 77
            {
                printf("JUMPXEQKNIL %d %d %d\n", a, d, aux);
                i++;
            } break;
            //LOP_JUMPXEQKB   78
            case LOP_JUMPXEQKN: // 79
            {
                printf("JUMPXEQKN %d %d %d\n", a, d, aux);
                i++;
            } break;
            case LOP_JUMPXEQKS: // 80
            {
                printf("if R%d ", a);
                if (aux & 0xffff0000)
                {
                    printf("~= ");
                }
                else
                {
                    printf("== ");
                }
                print_constant(constants, aux & 0x0000ffff, true);
                printf(": go to %#x\n", i+d);
                i++;
            } break;
            default:
            {
                printf("UNKNOWN (%d)\n", opcode);
            } break;
        }
    }
}

// Adapted from luau_load() at luau/VM/src/lvmload.cpp:245
void disassemble(uint8_t *data, int dataSize)
{
    printf("Disassembly:\n");

    uint8_t version = *(uint8_t*)data;
    data++;

    printf("Version: %d\n", version);

    uint8_t typesversion = 0;

    if (version >= 4)
    {
        typesversion = *(uint8_t*)data;
        data++;
    }

    printf("Types version: %d\n", typesversion);

    unsigned stringCount = readVarInt(&data);
    printf("String count: %d\n", stringCount);

    char **strings = malloc(stringCount * sizeof(char*));
    for (int i = 0; i < stringCount; i++)
    {
        int length = readVarInt(&data);
        strings[i] = malloc(length+1);
        strings[i][length] = 0;
        memcpy(strings[i], data, length);
        printf("String %d: %.*s\n", i, length, strings[i]);
        data += length;
    }

    if (typesversion == 3)
    {
        uint8_t index = *(uint8_t*)data;
        data++;
        printf("index: %d\n", index);

        if (index != 0)
        {
            printf("TODO index != 0\n");
            return;
        }
    }

    int protoCount = readVarInt(&data);
    printf("Proto count: %d\n", protoCount);

    for (int i = 0; i < protoCount; i++)
    {
        uint8_t maxstacksize = *(uint8_t*)data;
        data++;
        uint8_t numparams = *(uint8_t*)data;
        data++;
        uint8_t nups = *(uint8_t*)data;
        data++;
        uint8_t is_vararg = *(uint8_t*)data;
        data++;

        printf("Proto %d:\n", i);
        printf("Max stack size: %d\n", maxstacksize);
        printf("Numparams: %d\n", numparams);
        printf("Nups: %d\n", nups);
        printf("IsVararg? %d\n", is_vararg);

        if (version >= 4)
        {
            uint8_t flags = *(uint8_t*)data;
            data++;

            printf("Flags: %#x\n", flags);

            if (flags == 0x4)
            {
                printf("[Native]\n");
                //return;
            }

            if (typesversion == 1)
            {
                printf("Invalid typesversion. (%d)\n", typesversion);
                return;
            }
            else if (typesversion == 2 || typesversion == 3)
            {
                uint32_t typesize = readVarInt(&data);

                printf("Type size: %d\n", typesize);

                if (typesize)
                {
                    if (typesversion == 3)
                        remapUserdataTypes(data, typesize, LBC_TYPE_TAGGED_USERDATA_END - LBC_TYPE_TAGGED_USERDATA_BASE);
                    data += typesize;
                }
            }
        }

        int sizecode = readVarInt(&data);
        printf("Sizecode: %d\n", sizecode);

        void *code = data;

        

        data += 4*sizecode;
    
        int sizek = readVarInt(&data);
        printf("Sizek: %d\n", sizek);

        LuauConstant *constants = malloc(sizek * sizeof(LuauConstant));
        if (sizek)
        {
            constants->constantCount = sizek;
        }
        else
        {
            constants = malloc(sizeof(LuauConstant));
            constants->constantCount = 0;
        }

        for (int j = 0; j < sizek; j++)
        {
            uint8_t type = *(uint8_t*)data;
            data++;

            printf("Constant %d: ", j);

            constants[j].type = type;

            switch (type)
            {
                case LBC_CONSTANT_NIL:
                {
                    printf("nil\n");
                } break;
                case LBC_CONSTANT_BOOLEAN:
                {
                    bool b = *(uint8_t*)data;
                    data++;
                    printf("%s\n", b?"true":"false");
                } break;
                case LBC_CONSTANT_NUMBER:
                {
                    double d = *(double*)data;
                    data += sizeof(double);
                    printf("%lf\n", d);
                } break;
                case LBC_CONSTANT_STRING:
                {
                    int strId = readVarInt(&data) - 1;
                    printf("str %d ", strId);
                    if (strId >= 0 && strId < stringCount)
                    {
                        printf("\"%s\"\n", strings[strId]);
                        constants[j].val = strings[strId];
                    }
                    else
                    {
                        printf("\n");
                        constants[j].val = NULL;
                    }
                } break;
                case LBC_CONSTANT_IMPORT:
                {
                    uint32_t iid = *(uint32_t*)data;
                    data += sizeof(uint32_t);

                    printf("import count %d, id0 %d, id1 %d, id2 %d\n", (iid >> 30), (iid >> 20) & 1023, (iid >> 10) & 1023, iid & 1023);
                } break;
                case LBC_CONSTANT_TABLE:
                {
                    int keys = readVarInt(&data);
                    printf("tbl, len %d\n", keys);
                    for (int k = 0; k < keys; k++)
                    {
                        int key = readVarInt(&data);
                        printf("[%d] = [constant %d]\n", k, key);
                    }
                } break;
                case LBC_CONSTANT_CLOSURE:
                {
                    uint32_t fid = readVarInt(&data);
                    printf("closure %d\n", fid);
                } break;
                case LBC_CONSTANT_VECTOR:
                {
                    float x = *(float*)data;
                    data += sizeof(float);
                    float y = *(float*)data;
                    data += sizeof(float);
                    float z = *(float*)data;
                    data += sizeof(float);
                    float w = *(float*)data;
                    data += sizeof(float);
                    printf("{%f, %f, %f, %f}\n", x, y, z, w);
                } break;
                default:
                {
                    printf("Unknown constant type %d\n", type);
                    
                } break;
            }
        }

        int sizep = readVarInt(&data);
        printf("Sizep: %d\n", sizep);

        if (sizep)
        {
            for (int j = 0; j < sizep; j++)
            {
                int fid = readVarInt(&data);
                printf("fid[%d] = %d\n", j, fid);
            }
            printf("TODO Sizep\n");
        }

        disassemblecode(code, sizecode, constants);

        int linedefined = readVarInt(&data);
        printf("linedefined: %d\n", linedefined);

        char *debugname = readString(&data, strings);
        //printf("debugname: %s\n", debugname);

        uint8_t lineinfo = *(uint8_t*)data;
        data++;

        if (lineinfo)
        {
            uint8_t linegaplog2 = *(uint8_t*)data;
            data++;

            printf("lineinfo:\n");
            printf("linegaplog2: %d\n", linegaplog2);

            int intervals = ((sizecode - 1) >> linegaplog2) + 1;
            int absoffset = (sizecode + 3) & ~3;

            const int sizelineinfo = absoffset + intervals * sizeof(int);

            uint8_t lastoffset = 0;
            for (int j = 0; j < sizecode; j++)
            {
                lastoffset += *(uint8_t*)data;
                data++;
                printf("lastoffset %05x: %d\n", j, lastoffset+1);
            }

            int lastline = 0;
            for (int j = 0; j < intervals; j++)
            {
                lastline += *(uint32_t*)data;
                data += sizeof(uint32_t);
                printf("lastline %d: %d\n", j, lastline);
            }

        }

        uint8_t debuginfo = *(uint8_t*)data;
        data++;

        if (debuginfo)
        {
            printf("debuginfo:\n");
            int sizelocvars = readVarInt(&data);
            
            printf("sizelocvars: %d\n", sizelocvars);

            for (int j = 0; j < sizelocvars; j++)
            {
                readVarInt(&data);//char *varname = readString(&data, strings);
                int startpc = readVarInt(&data);
                int endpc = readVarInt(&data);
                uint8_t reg = *(uint8_t*)data;
                data++;

                //printf("locvar %d:\n", j);
                //printf("varname: %s\n", varname);
                //printf("startpc: %#x\n", startpc);
                //printf("endpc: %#x\n", endpc);
                //printf("reg: %d\n", reg);
            }

            int sizeupvalues = readVarInt(&data);

            printf("sizeupvalues: %d\n", sizeupvalues);
            for (int j = 0; j < sizeupvalues; j++)
            {
                char *upvalue = readString(&data, strings);

                //printf("upvalue %d: %s\n", j, upvalue);
            }
        }
    }

    uint32_t mainid = readVarInt(&data);

    printf("Entrypoint proto: %d\n", mainid);
}

