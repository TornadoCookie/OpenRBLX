#include "scriptruntime.h"
#include "basepart.h"
#include "datamodel.h"

#include <pthread.h>
#include "debug.h"

DEFAULT_DEBUG_CHANNEL(scriptruntime);

ScriptRuntime *ScriptRuntime_new(const char *className, Instance *parent)
{
    ScriptRuntime *newInst = Instance_new(className, parent);

    newInst->instance.DataCost = sizeof(*newInst);
    newInst = realloc(newInst, sizeof(*newInst));

    if (parent) Instance_SetParent(newInst, parent);

    return newInst;
}

#include "rlw32compat.h"
#include "lua.h"
#include "luacode.h"
#include "lualib.h"

//static time_t time_initialized;

static int luau_wait(lua_State *L)
{
    lua_Number towait = 0.03;
    if (lua_gettop(L) == 1)
    {
        towait = lua_tonumber(L, 1);
    }
    lua_Number elapsed = 0.0;

    //time_t current_time = time(NULL);
    //lua_Number elapsedtotal = current_time - time_initialized;

    printf("Sleeping %lf\n", towait);
    usleep(towait*1000000);

    elapsed = towait;

    lua_pushnumber(L, elapsed);
    lua_pushnumber(L, elapsed); //elapsedTotal

    return 2;
}

static int luau_tick(lua_State *L)
{
    lua_pushinteger(L, time(NULL));
    return 1;
}

static int luau_Instance_FindFirstChild(lua_State *L);
static void luau_pushinstance(lua_State *L, Instance *inst);
static void luau_pushvector3(lua_State *L, Vector3 v);

static Vector3 luau_tovector3(lua_State *L, int idx)
{
    Vector3 ret = { 0 };

    lua_pushstring(L, "X");
    lua_gettable(L, idx);
    ret.x = lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_pushstring(L, "Y");
    lua_gettable(L, idx);
    ret.y = lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_pushstring(L, "Z");
    lua_gettable(L, idx);
    ret.z = lua_tonumber(L, -1);
    
    lua_pop(L, 1);

    return ret;
}

static int luau_vector3__mul(lua_State *L)
{
    lua_Number factor = lua_tonumber(L, 2);
    Vector3 vec = luau_tovector3(L, 1);

    Vector3 ret = (Vector3){
        vec.x * factor,
        vec.y * factor,
        vec.z * factor
    };

    luau_pushvector3(L, ret);

    return 1;
}

static int luau_vector3__add(lua_State *L)
{
    Vector3 v2 = luau_tovector3(L, 2);
    Vector3 v1 = luau_tovector3(L, 1);

    Vector3 ret = Vector3Add(v1, v2);

    luau_pushvector3(L, ret);

    return 1;
}

static void luau_pushvector3(lua_State *L, Vector3 v)
{
    lua_newtable(L);

    lua_newtable(L);

    lua_pushstring(L, "__mul");
    lua_pushcfunction(L, luau_vector3__mul, "Vector3:__mul");
    lua_settable(L, -3);

    lua_pushstring(L, "__add");
    lua_pushcfunction(L, luau_vector3__add, "Vector3:__add");
    lua_settable(L, -3);

    lua_setmetatable(L, -2);

    lua_pushstring(L, "X");
    lua_pushnumber(L, v.x);
    lua_settable(L, -3);

    lua_pushstring(L, "Y");
    lua_pushnumber(L, v.y);
    lua_settable(L, -3);

    lua_pushstring(L, "Z");
    lua_pushnumber(L, v.z);
    lua_settable(L, -3);
}

static void luau_pushcframe(lua_State *L, CFrame cf)
{
    lua_newtable(L);

    lua_pushstring(L, "X");
    lua_pushnumber(L, cf.X);
    lua_settable(L, -3);

    lua_pushstring(L, "Y");
    lua_pushnumber(L, cf.Y);
    lua_settable(L, -3);

    lua_pushstring(L, "Z");
    lua_pushnumber(L, cf.Z);
    lua_settable(L, -3);

    lua_pushstring(L, "LookVector");
    luau_pushvector3(L, (Vector3){-cf.R02, -cf.R12, -cf.R22});
    lua_settable(L, -3);

    lua_pushstring(L, "lookVector");
    luau_pushvector3(L, (Vector3){-cf.R02, -cf.R12, -cf.R22});
    lua_settable(L, -3);

    lua_pushstring(L, "RightVector");
    luau_pushvector3(L, (Vector3){cf.R00, cf.R10, cf.R20});
    lua_settable(L, -3);

    lua_pushstring(L, "UpVector");
    luau_pushvector3(L, (Vector3){cf.R01, cf.R11, cf.R21});
    lua_settable(L, -3);
}

static int luau_Instance__index(lua_State *L)
{
    lua_pushstring(L, "__inst_ptr");
    lua_rawget(L, 1);
    Instance *inst = lua_touserdata(L, -1);
    lua_pop(L, 1);

    const char *name = lua_tostring(L, 2);
    Instance *child = Instance_FindFirstChild(inst, name, false);

    if (child)
    {
        luau_pushinstance(L, child);

        return 1;
    }

    lua_rawget(L, 1);
    if (!lua_isnil(L, -1))
        return 1;

    if (!strcmp(name, "Parent"))
    {
        luau_pushinstance(L, inst->Parent);
        return 1;
    }

    if (Instance_IsA(inst, "BasePart"))
    {
        BasePart *basepart = inst;
        if (!strcmp(name, "CFrame"))
        {
            luau_pushcframe(L, basepart->CFrame);
            return 1;
        }
        else if (!strcmp(name, "Position"))
        {
            luau_pushvector3(L, basepart->Position);
            return 1;
        }
    }

    lua_pushstring(L, TextFormat("Instance %s has no child or property %s\n", inst->ClassName, name));
    lua_error(L);

    lua_pushnil(L);
    return 1;
}

static int luau_Instance_FindFirstChild(lua_State *L)
{
    if (lua_gettop(L) < 2)
    {
        lua_pushstring(L, "Expected 2 or more arguments.");
        lua_error(L);
    }

    // Instance is at 1
    const char *name = lua_tostring(L, 2);

    lua_pushstring(L, "__inst_ptr");
    lua_rawget(L, 1);

    Instance *inst = lua_touserdata(L, -1);

    Instance *ret = Instance_FindFirstChild(inst, name, lua_gettop(L) == 4 ? lua_toboolean(L, 3) : false);

    if (!ret)
    {
        return 0;
    }

    lua_newtable(L);

    return 1;
}

static int luau_Instance_GetChildren(lua_State *L)
{
    if (lua_gettop(L) != 1)
    {
        lua_pushstring(L, "Expected 1 argument.\n");
        lua_error(L);
    }

    lua_pushstring(L, "__inst_ptr");
    lua_rawget(L, 1);

    Instance *inst = lua_touserdata(L, -1);

    lua_newtable(L);

    for (int i = 0; i < inst->childCount; i++)
    {
        lua_pushstring(L, inst->children[i]->Name);
        luau_pushinstance(L, inst->children[i]);
        lua_settable(L, -3);
    }

    return 1;
}

static int luau_Instance_Clone(lua_State *L)
{
    if (lua_gettop(L) != 1)
    {
        lua_pushstring(L, "Expected 1 arguments.\n");
        lua_error(L);
    }

    lua_pushstring(L, "__inst_ptr");
    lua_rawget(L, 1);

    Instance *inst = lua_touserdata(L, -1);

    luau_pushinstance(L, Instance_Clone(inst));

    return 1;
}

static int luau_Instance_WaitForChild(lua_State *L)
{
    int nargs = lua_gettop(L);
    if (nargs < 2)
    {
        lua_pushstring(L, "Expected at least 2 arguments.\n");
        lua_error(L);
    }

    lua_pushstring(L, "__inst_ptr");
    lua_rawget(L, 1);

    const char *childName = luaL_checkstring(L, 2);
    double timeOut = 0.0;
    if (nargs > 2)
    {
        timeOut = luaL_checknumber(L, 3);
    }

    Instance *inst = lua_touserdata(L, -1);

    luau_pushinstance(L, Instance_WaitForChild(inst, childName, timeOut));

    return 1;
}

static int luau_Instance_IsDescendantOf(lua_State *L)
{
    int nargs = lua_gettop(L);
    if (nargs != 2)
    {
        lua_pushstring(L, "Expected 2 arguments.\n");
        lua_error(L);
    }

    lua_pushstring(L, "__inst_ptr");
    lua_rawget(L, 1);

    Instance *inst = lua_touserdata(L, -1);

    lua_pushstring(L, "__inst_ptr");
    lua_rawget(L, 2);

    Instance *ancestor = lua_touserdata(L, -1);

    lua_pushboolean(L, Instance_IsDescendantOf(inst, ancestor));

    return 1;
}

static int luau_ServiceProvider_GetService(lua_State *L)
{
    lua_pushstring(L, "__inst_ptr");
    lua_gettable(L, 1);

    ServiceProvider *serviceprovider = lua_touserdata(L, -1);
    const char *serviceName = lua_tostring(L, 2);

    Instance *service = ServiceProvider_GetService(serviceprovider, serviceName);

    luau_pushinstance(L, service);

    return 1;
}

static void luau_pushinstance(lua_State *L, Instance *inst)
{
    if (!inst)
    {
        lua_pushnil(L);
        return;
    }

    lua_newtable(L);

    lua_newtable(L);

    lua_pushstring(L, "__index");
    lua_pushcfunction(L, luau_Instance__index, "Instance:__index");
    lua_settable(L, -3);

    lua_setmetatable(L, -2);

    lua_pushstring(L, "FindFirstChild");
    lua_pushcfunction(L, luau_Instance_FindFirstChild, "Instance:FindFirstChild");
    lua_settable(L, -3);

    lua_pushstring(L, "findFirstChild");
    lua_pushcfunction(L, luau_Instance_FindFirstChild, "Instance:findFirstChild");
    lua_settable(L, -3);

    lua_pushstring(L, "GetChildren");
    lua_pushcfunction(L, luau_Instance_GetChildren, "Instance:GetChildren");
    lua_settable(L, -3);

    lua_pushstring(L, "children");
    lua_pushcfunction(L, luau_Instance_GetChildren, "Instance:children");
    lua_settable(L, -3);

    lua_pushstring(L, "Clone");
    lua_pushcfunction(L, luau_Instance_Clone, "Instance:Clone");
    lua_settable(L, -3);

    lua_pushstring(L, "WaitForChild");
    lua_pushcfunction(L, luau_Instance_WaitForChild, "Instance:WaitForChild");
    lua_settable(L, -3);

    lua_pushstring(L, "IsDescendantOf");
    lua_pushcfunction(L, luau_Instance_IsDescendantOf, "Instance:IsDescendantOf");
    lua_settable(L, -3);
    
    lua_pushstring(L, "__inst_ptr");
    lua_pushlightuserdata(L, inst);
    lua_settable(L, -3);

    if (Instance_IsA(inst, "ServiceProvider"))
    {
        lua_pushstring(L, "GetService");
        lua_pushcfunction(L, luau_ServiceProvider_GetService, "ServiceProvider:GetService");
        lua_settable(L, -3);
    }
}

static int luau_Vector3_new(lua_State *L)
{
    if (lua_gettop(L) != 3)
    {
        lua_pushstring(L, "Expected 3 arguments.\n");
        lua_error(L);
    }

    lua_Number x = lua_tonumber(L, 1);
    lua_Number y = lua_tonumber(L, 2);
    lua_Number z = lua_tonumber(L, 3);

    luau_pushvector3(L, (Vector3){x, y, z});

    return 1;
}

static void init_lua_state(lua_State *L, Script *script)
{
    //luaL_openlibs(L);

    // Roblox global functions
    lua_pushcfunction(L, luau_wait, "wait");
    lua_setglobal(L, "wait");

    lua_pushcfunction(L, luau_tick, "tick");
    lua_setglobal(L, "tick");

    // Global instances
    luau_pushinstance(L, GetDataModel()->Workspace);
    lua_setglobal(L, "Workspace");

    luau_pushinstance(L, script);
    lua_setglobal(L, "script");

    luau_pushinstance(L, GetDataModel());
    lua_setglobal(L, "game");

    // Data type constructors
    lua_newtable(L);

    lua_pushstring(L, "new");
    lua_pushcfunction(L, luau_Vector3_new, "Vector3.new");
    lua_settable(L, -3);

    lua_setglobal(L, "Vector3");

    lua_newtable(L);

    lua_setglobal(L, "CFrame");
    
}

static void scrt_debugstep(lua_State *L, lua_Debug *ar)
{
    printf("%s:%s:%d\n", ar->what, ar->name, ar->currentline);
}

static void scrt_interrupt(lua_State *L, int gc)
{
    printf("gc %d\n", gc);
}

#include "Luau/Bytecode.h"

static unsigned int readVarInt(char **pData)
{
    unsigned result = 0, shift = 0;
    uint8_t byte;
    char *data = *pData;

    do {
        byte = *(uint8_t*)data;
        data++;
        result |= byte & 127 << shift;
        shift += 7;
    } while (byte & 128);

    *pData = data;

    return result;
}

static void remapUserdataTypes(char *data, size_t size, uint32_t count)
{
    uint32_t typeSize = readVarInt(&data);
    uint32_t upvalCount = readVarInt(&data);
    uint32_t localCount = readVarInt(&data);

    printf("remap Userdata Types:\n");
    printf("Type size: %d\n", typeSize);
    if (typeSize)
    {
        printf("TODO typeSize\n");
    }

    printf("upval Count: %d\n", upvalCount);
    if (upvalCount)
    {
        printf("TODO upvalCount\n");
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
}

static char *readString(uint8_t **data, char **strings)
{
    int id = readVarInt(data);
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

// Adapted from Luau/Bytecode.h documentation
static void disassemblecode(uint32_t *code, int codeSize) 
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
                printf("LOADB %d %d %d\n", a, b, c);
            } break;
            case LOP_LOADN: // 4
            {
                printf("LOADN %d %d\n", a, d);
            } break;
            case LOP_LOADK: // 5
            {
                printf("LOADK %d %d\n", a, d);
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
                printf("GETUPVAL %d %d\n", a, b);
            } break;
            case LOP_SETUPVAL: // 10
            {
                printf("SETUPVAL %d %d\n", a, b);
            } break;
            case LOP_CLOSEUPVALS: // 11
            {
                printf("CLOSEUPVALS %d", a);
            } break;
            case LOP_GETIMPORT: // 12
            {
                int pcount = aux >> 30;
                int p3 = aux & 1023;
                int p2 = (aux >> 10) & 1023;
                int p1 = (aux >> 20) & 1023;
        
                printf("R%d = asliteral(", a);

                if (pcount > 0)
                {
                    printf("K%d", p1);
                    if (pcount > 1)
                    {
                        printf(".K%d", p2);
                        if (pcount > 2)
                        {
                            printf(".K%d", p3);
                        }
                    }
                }
                printf(")\n");

                i++;
            } break;
            //LOP_GETTABLE   13
            //LOP_SETTABLE   14
            case LOP_GETTABLEKS: // 15
            {
                printf("R%d = R%d[K%d]\n", a, b, aux);
                i++;
            } break;
            case LOP_SETTABLEKS: // 16
            {
                printf("SETTABLEKS %d %d %d %#x\n", a, b, c, aux);
                i++;
            } break;
            //LOP_GETTABLEN  17
            //LOP_SETTABLEN  18
            //LOP_NEWCLOSURE 19
            case LOP_NAMECALL: // 20
            {
                printf("NAMECALL %d %d %d %#x\n", a, b, c, aux);
                i++;
            } break;
            case LOP_CALL: // 21
            {
                if (b == 2)
                {
                    printf("R%d = ", a);
                }
                printf("R%d(", a);
                for (int j = a+1; j < a+b-1; j++)
                {
                    printf("R%d", j);
                    if (j != a+b-2)
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
                else
                {
                    printf("RETURN %d %d\n", a, b);
                }
            } break;
            //LOP_JUMP       23
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
                printf("JUMPIFEQ %d %d %d\n", a, d, aux);
                i++;
            } break;
            //LOP_JUMPIFLE   28
            //LOP_JUMPIFLT   29
            //LOP_JUMPIFNOTEQ 30
            //LOP_JUMPIFNOTLE 31
            //LOP_JUMPIFNOTLT 32
            //LOP_ADD        33
            //LOP_SUB        34
            //LOP_MUL        35
            //LOP_DIV        36
            //LOP_MOD        37
            //LOP_POW        38
            //LOP_ADDK       39
            //LOP_SUBK       40
            //LOP_MULK       41
            //LOP_DIVK       42
            //LOP_MODK       43
            //LOP_POWK       44
            //LOP_AND        45
            //LOP_OR         46
            //LOP_ANDK       47
            //LOP_ORK        48
            //LOP_CONCAT     49
            //LOP_NOT        50
            //LOP_MINUS      51
            //LOP_LENGTH     52
            //LOP_NEWTABLE   53
            //LOP_DUPTABLE   54
            //LOP_SETLIST    55
            //LOP_FORNPREP   56
            //LOP_FORNLOOP   57
            //LOP_FORGLOOP   58
            //LOP_FORGPREP_INEXT 59
            //LOP_FASTCALL3  60
            //LOP_FORGPREP_NEXT 61
            //LOP_NATIVECALL 62
            //LOP_GETVARARGS 63
            //LOP_DUPCLOSURE 64
            case LOP_PREPVARARGS: // 65
            {
                printf("PREPVARARGS %d\n", a);
            } break;
            //LOP_LOADKX     66
            //LOP_JUMPX      67
            //LOP_FASTCALL   68
            //LOP_COVERAGE   69
            //LOP_CAPTURE    70
            case LOP_SUBRK: // 71
            {
                printf("SUBRK %d %d %d\n", a, b, c);
            } break;
            default:
            {
                printf("UNKNOWN (%d)\n", opcode);
            } break;
        }
    }
}

// Adapted from luau_load() at luau/VM/src/lvmload.cpp:245
static void disassemble(uint8_t *data, int dataSize)
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

        disassemblecode(data, sizecode);

        data += 4*sizecode;
    
        int sizek = readVarInt(&data);
        printf("Sizek: %d\n", sizek);

        for (int j = 0; j < sizek; j++)
        {
            uint8_t type = *(uint8_t*)data;
            data++;

            printf("Constant %d: ", j);

            switch (type)
            {
                case LBC_CONSTANT_NIL:
                {
                    printf("nil\n");
                } break;
                case LBC_CONSTANT_STRING:
                {
                    int strId = readVarInt(&data) - 1;
                    printf("str %d ", strId);
                    printf("\"%s\"\n", strings[strId]);
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
                    for (int i = 0; i < keys; i++)
                    {
                        int key = readVarInt(&data);
                        printf("[%d] = [constant %d]\n", j, key);
                    }
                } break;
                default:
                {
                    printf("Unknown constant type %d\n", type);
                    return;
                } break;
            }
        }

        int sizep = readVarInt(&data);
        printf("Sizep: %d\n", sizep);

        if (sizep)
        {
            printf("TODO Sizep\n");
            return;
        }

        int linedefined = readVarInt(&data);
        printf("linedefined: %d\n", linedefined);

        char *debugname = readString(&data, strings);
        printf("debugname: %s\n", debugname);

        uint8_t lineinfo = *(uint8_t*)data;
        data++;

        if (lineinfo)
        {
            printf("TODO lineinfo\n");
            return;
        }

        uint8_t debuginfo = *(uint8_t*)data;
        data++;

        if (debuginfo)
        {
            printf("TODO debuginfo\n");
        }
    }

    uint32_t mainid = *(uint32_t*)data;
    data += sizeof(uint32_t);

    printf("Entrypoint proto: %d\n", mainid);
}

static void run_script(Script *script, const char *source, int sourceLength, bool client)
{
    lua_State *L = luaL_newstate();

    init_lua_state(L, script);

    printf("Running script source [%s]\n", ((Instance*)script)->Name);

    size_t bytecodesize = 0;
    char *bytecode;
    if (script->isBytecode)
    {
        bytecode = source;
        bytecodesize = sourceLength;

        SaveFileData("bytecode", source, sourceLength);
    }
    else
    {
        struct lua_CompileOptions compopts = { 0 };

        compopts.debugLevel = 2;

        luau_compile(source, sourceLength, &compopts, &bytecodesize);
    }
    int result = luau_load(L, ((Instance*)script)->Name, bytecode, bytecodesize, 0);
    
    if (!script->isBytecode)
        free(bytecode);
    
    if (result == 0)
    {
        goto end;
    }
    luaL_sandbox(L);
    lua_singlestep(L, 1);
    lua_callbacks(L)->debugstep = scrt_debugstep;
    lua_callbacks(L)->interrupt = scrt_interrupt;
    lua_call(L, 0, 0);
    //if (lua_pcall(L, 0, 0, 0))
    //{
    //    printf("Error: %s\n", lua_tostring(L, -1));
    //    goto end;
    //}
    
    printf("worked.\n");

end:
    printf("Source:  ");

    if (script->isBytecode)
    {
        printf("[%d bytes of bytecode]\n", sourceLength);
        disassemble(source, sourceLength);
    }
    else
    {
        printf("\"%s\"\n", source);
    }
    lua_close(L);
}

typedef struct ScriptRunnerArgs {
    Script *script;
    const char *source;
    int sourceLength;
    bool client;
    ScriptRuntime *this;
} ScriptRunnerArgs;

void *run_script_runner(void *args)
{
    ScriptRunnerArgs *srargs = args;

    run_script(srargs->script, srargs->source, srargs->sourceLength, srargs->client);

    Instance_Destroy(srargs->this);

    return NULL;
}

void ScriptRuntime_RunScript(ScriptRuntime *this, Script *script)
{
    ScriptRunnerArgs *args = malloc(sizeof(ScriptRunnerArgs));
    pthread_t thread;

    args->script = script;
    args->source = script->Source;
    args->sourceLength = script->sourceLength;
    args->client = false;
    args->this = this;

    pthread_create(&thread, NULL, run_script_runner, args);
}

void ScriptRuntime_RunLocalScript(ScriptRuntime *this, LocalScript *localScript)
{
    ScriptRunnerArgs *args = malloc(sizeof(ScriptRunnerArgs));
    pthread_t thread;

    args->script = localScript;
    args->source = localScript->script.Source;
    args->sourceLength = localScript->script.sourceLength;
    args->client = true;
    args->this = this;

    pthread_create(&thread, NULL, run_script_runner, args);
}

