#include "luau/serialize.h"
#include "luau/instance.h"
#include "luau/event.h"
#include "luau/vector3.h"

#include "color3.h"

#include "debug.h"
DEFAULT_DEBUG_CHANNEL(luau)

#include "lualib.h"

int luau_pushserialization(lua_State *L, Serialization sz)
{
    int type = sz.type;
    void *val = sz.val;

    if (!val || !sz.name) return 0;

    switch (type)
    {
        case Serialize_float:
        {
            lua_pushnumber(L, *(float*)val);
            return 1;
        } break;
        case Serialize_string:
        {
            lua_pushstring(L, *(char**)val);
            return 1;
        } break;
        case Serialize_Ref:
        {
            luau_pushinstance(L, *(Instance**)val);
            return 1;
        } break;
        case Serialize_event:
        {
            luau_pushevent(L, *(RBXScriptSignal**)val);
            return 1;
        } break;
        default:
        {
            FIXME("No lua type serializer for %d\n", type);
            luaL_error(L, "No lua type serializer for %d\n", type);
        } break;
    }
    return 0;
}


int sztypesiz(int t)
{
    switch (t)
    {
        case Serialize_bool: return sizeof(bool);
        case Serialize_float: return sizeof(float);
        case Serialize_token: return sizeof(int);
        case Serialize_int: return sizeof(int);
        case Serialize_CoordinateFrame: return sizeof(CFrame);
        case Serialize_string: return sizeof(char*);
        case Serialize_Vector3: return sizeof(Vector3);
        case Serialize_Color3: return sizeof(Color3);
        case Serialize_Ref: return sizeof(Instance*);
        case Serialize_double: return sizeof(double);
        case Serialize_event: return sizeof(RBXScriptSignal*);
    }
}

Serialization luau_toserialization(lua_State *L, int idx)
{
    Serialization ret = { 0 };

    if (lua_isboolean(L, idx))
    {
        ret.type = Serialize_bool;
        ret.val = malloc(sizeof(bool));
        *(bool*)ret.val = lua_toboolean(L, idx);
    }
    else if (lua_isnumber(L, idx))
    {
        ret.type = Serialize_float;
        ret.val = malloc(sizeof(float));
        *(float*)ret.val = lua_tonumber(L, idx);
    }
    else if (lua_isstring(L, idx))
    {
        ret.type = Serialize_string;
        ret.val = malloc(sizeof(char**));
        *(char**)ret.val = lua_tostring(L, idx);
    }
    else if (lua_istable(L, idx))
    {
        lua_getfield(L, idx, "__inst_ptr");
        if (!lua_isnil(L, idx))
        {
            lua_pop(L, 1);
            ret.type = Serialize_Ref;
            ret.val = malloc(sizeof(Instance**));
            *(Instance**)ret.val = luau_toinstance(L, idx);
            return ret;
        }
        lua_pop(L, 1);

        lua_getfield(L, idx, "X");
        if (!lua_isnil(L, idx))
        {
            if (lua_istable(L, -1))
            {
                FIXME("type is %s\n", "UDim2");
                luaL_error(L, "unable to set UDim2 attr\n");
            }

            lua_pop(L, 1);
            lua_getfield(L, idx, "Z");

            if (!lua_isnil(L, -1))
            {
                lua_pop(L, 1);

                ret.type = Serialize_Vector3;
                ret.val = malloc(sizeof(Vector3*));
                *(Vector3*)ret.val = luau_tovector3(L, idx);
                return ret;
            }
            else
            {
                FIXME("type is %s\n", "Vector2");
                luaL_error(L, "unable to set Vector2 attr\n");
            }
        }

        lua_pop(L, 1);
        
        lua_getfield(L, idx, "Scale");
        if (!lua_isnil(L, idx))
        {
            FIXME("type is %s\n", "UDim");
            luaL_error(L, "unable to set UDim attr\n");
        }
    }
    else
    {
        ret.val = NULL;
    }

    return ret;
}

