#include "luau/enum.h"

#include "debug.h"
DEFAULT_DEBUG_CHANNEL(enum);

static int luau_Enum_GetEnumItems(lua_State *L)
{
    // the enum obj is at 1. since this is a table, we can just 'return self'
    // TODO this includes member functions
    return 1;
}

int luau_Enums_GetEnums(lua_State *L)
{

#define enum_start(n) lua_pushstring(L, #n); lua_newtable(L); lua_pushcfunction(L, luau_Enum_GetEnumItems, "Enum:GetEnumItems"); lua_setfield(L, -2, "GetEnumItems");
#define enum_val(n, v) lua_pushstring(L, #n); lua_pushinteger(L, v); lua_settable(L, -3);
#define enum_end() lua_settable(L, -3);
    lua_newtable(L);

    enum_start(ZIndexBehavior);
    enum_val(Global, 0);
    enum_val(Sibling, 1);
    enum_end();

    enum_start(InitialDockState);
    enum_val(Top, 0);
    enum_val(Bottom, 1);
    enum_val(Left, 2);
    enum_val(Right, 3);
    enum_val(Float, 4);
    enum_end();

    enum_start(Font);
    enum_val(Legacy, 0);
    enum_val(Arial, 1);
    enum_val(ArialBold, 2);
    enum_val(SourceSans, 3);
    enum_val(SourceSansBold, 4);
    enum_end();

    enum_start(HorizontalAlignment);
    enum_val(Center, 0);
    enum_val(Left, 1);
    enum_val(Right, 2);
    enum_end();

    enum_start(VerticalAlignment);
    enum_val(Center, 0);
    enum_val(Top, 1);
    enum_val(Bottom, 2);
    enum_end();

    enum_start(ScrollBarInset);
    enum_val(None, 0);
    enum_val(ScrollBar, 1);
    enum_val(Always, 2);
    enum_end();

    enum_start(TextXAlignment);
    enum_val(Left, 0);
    enum_val(Right, 1);
    enum_val(Center, 2);
    enum_end();

    enum_start(TextYAlignment);
    enum_val(Top, 0);
    enum_val(Center, 1);
    enum_val(Bottom, 2);
    enum_end();

    enum_start(ScaleType);
    enum_val(Stretch, 0);
    enum_val(Slice, 1);
    enum_val(Tile, 2);
    enum_val(Fit, 3);
    enum_val(Crop, 4);
    enum_end();

    enum_start(FillDirection);
    enum_val(Horizontal, 0);
    enum_val(Vertical, 1);
    enum_end();

    enum_start(TextTruncate);
    enum_val(None, 0);
    enum_val(AtEnd, 1);
    enum_val(SplitWord, 2);
    enum_end();

    enum_start(ScrollingDirection);
    enum_val(X, 1);
    enum_val(Y, 2);
    enum_val(XY, 4);
    enum_end();

    enum_start(AutomaticSize);
    enum_val(None, 0);
    enum_val(X, 1);
    enum_val(Y, 2);
    enum_val(XY, 3);
    enum_end();

    enum_start(SortOrder);
    enum_val(Name, 0);
    enum_val(Custom, 1);
    enum_val(LayoutOrder, 2);
    enum_end();

    enum_start(ApplyStrokeMode);
    enum_val(Contextual, 0);
    enum_val(Border, 1);
    enum_end();

    enum_start(EasingStyle);
    enum_val(Linear, 0);
    enum_val(Sine, 1);
    enum_val(Back, 2);
    enum_val(Quad, 3);
    enum_val(Quart, 4);
    enum_val(Quint, 5);
    enum_val(Bounce, 6);
    enum_val(Elastic, 7);
    enum_val(Exponential, 8);
    enum_val(Circular, 9);
    enum_val(Cubic, 10);
    enum_end();

    enum_start(SurfaceType);
    enum_val(Smooth, 0);
    enum_val(Glue, 1);
    enum_val(Weld, 2);
    enum_val(Studs, 3);
    enum_val(Inlet, 4);
    enum_val(Universal, 5);
    enum_val(Hinge, 6);
    enum_val(Motor, 7);
    enum_val(SteppingMotor, 8);
    enum_val(SmoothNoOutlines, 10);
    enum_end();

    enum_start(NormalId);
    enum_val(Right, 0);
    enum_val(Top, 1);
    enum_val(Back, 2);
    enum_val(Left, 3);
    enum_val(Bottom, 4);
    enum_val(Front, 5);
    enum_end();

#undef enum_end
#undef enum_val
#undef enum_start

    return 1;
}

int luau_Enums__index(lua_State *L)
{
    const char *key = lua_tostring(L, 2);

    luau_Enums_GetEnums(L);
    lua_pushstring(L, key);
    lua_gettable(L, -2);

    if (lua_isnil(L, -1))
    {
        FIXME("key %s\n", key);
    }

    return 1;
}


