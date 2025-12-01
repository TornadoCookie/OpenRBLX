#include "luau/enum.h"

#include "debug.h"
DEFAULT_DEBUG_CHANNEL(enum);

static int luau_Enum_GetEnumItems(lua_State *L)
{
    // this is at 1

    lua_newtable(L); // at 2
    int count = 1;

    /* table is in the stack at index 't' */
     lua_pushnil(L);  /* first key */
     while (lua_next(L, 1) != 0) {
       /* uses 'key' (at index -2) and 'value' (at index -1) */
         if (lua_type(L, -1) == LUA_TFUNCTION)
         {
             lua_pop(L, 1);
         }
        else
        {
            lua_pushinteger(L, count);
         lua_pushvalue(L, -2);
         lua_settable(L, 2);
         count++;
        }
         /* removes 'value'; keeps 'key' for next iteration */
       //lua_pop(L, 2);
     }

     return 1;
}

int luau_Enums_GetEnums(lua_State *L)
{

#define enum_start(n) lua_pushstring(L, #n); lua_newtable(L); lua_pushcfunction(L, luau_Enum_GetEnumItems, "Enum:GetEnumItems"); lua_setfield(L, -2, "GetEnumItems");
#define enum_val(n, v) lua_pushinteger(L, v); lua_setfield(L, -2, #n);
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
    enum_val(BuilderSans, 46);
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

    enum_start(PreferredTextSize);
    enum_val(Medium, 1);
    enum_val(Large, 2);
    enum_val(Larger, 3);
    enum_val(Largest, 4);
    enum_end();

    enum_start(FontWeight);
    enum_val(Thin, 100);
    enum_val(ExtraLight, 200);
    enum_val(Light, 300);
    enum_val(Regular, 400);
    enum_val(Medium, 500);
    enum_val(SemiBold, 600);
    enum_val(Bold, 700);
    enum_val(ExtraBold, 800);
    enum_val(Heavy, 900);
    enum_end();

    enum_start(GuiState);
    enum_val(Idle, 0);
    enum_val(Hover, 1);
    enum_val(Press, 2);
    enum_val(NonInteractable, 3);
    enum_end();

    enum_start(UIFlexAlignment);
    enum_val(None, 0);
    enum_val(Fill, 1);
    enum_val(SpaceAround, 2);
    enum_val(SpaceBetween, 3);
    enum_val(SpaceEvenly, 4);
    enum_end();

    enum_start(ItemLineAlignment);
    enum_val(Automatic, 0);
    enum_val(Start, 1);
    enum_val(Center, 2);
    enum_val(End, 3);
    enum_val(Stretch, 4);
    enum_end();

    enum_start(UIFlexMode);
    enum_val(None, 0);
    enum_val(Grow, 1);
    enum_val(Shrink, 2);
    enum_val(Fill, 3);
    enum_val(Custom, 4);
    enum_end();

    enum_start(UserInputType);
    enum_val(MouseButton1, 0);
    enum_val(MouseButton2, 1);
    enum_val(MouseButton3, 2);
    enum_val(MouseWheel, 3);
    enum_val(MouseMovement, 4);
    enum_val(Touch, 7);
    enum_val(Keyboard, 8);
    enum_val(Focus, 9);
    enum_val(Accelerometer, 10);
    enum_val(Gyro, 11);
    enum_val(Gamepad1, 12);
    enum_val(Gamepad2, 13);
    enum_val(Gamepad3, 14);
    enum_val(Gamepad4, 15);
    enum_val(Gamepad5, 16);
    enum_val(Gamepad6, 17);
    enum_val(Gamepad7, 18);
    enum_val(Gamepad8, 19);
    enum_val(TextInput, 20);
    enum_val(InputMethod, 21);
    enum_val(None, 22);
    enum_end();

    enum_start(UIDragDetectorDragStyle);
    enum_val(TranslatePlane, 0);
    enum_val(TranslateLine, 1);
    enum_val(Rotate, 2);
    enum_val(Scriptable, 3);
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


