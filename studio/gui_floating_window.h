#include "raylib.h"

#ifndef GUI_FLOATING_WINDOW_H
#define GUI_FLOATING_WINDOW_H

typedef struct {
    Vector2 position;
    Vector2 size;
    bool minimized;
    bool moving;
    bool resizing;
    Vector2 scroll;
} GuiFloatingWindowState;

#ifdef __cplusplus
extern "C" {
#endif

void GuiWindowFloating(GuiFloatingWindowState *state, void (*draw_content)(Vector2, Vector2), Vector2 content_size, const char* title);

#ifdef __cplusplus
}
#endif

#endif

#ifdef GUI_FLOATING_WINDOW_IMPLEMENTATION

#include "raygui.h"

static void _GuiWindowFloating(Vector2 *position, Vector2 *size, bool *minimized, bool *moving, bool *resizing, void (*draw_content)(Vector2, Vector2), Vector2 content_size, Vector2 *scroll, const char* title) {
    #if !defined(RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT)
        #define RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT 24
    #endif

    #if !defined(RAYGUI_WINDOW_CLOSEBUTTON_SIZE)
        #define RAYGUI_WINDOW_CLOSEBUTTON_SIZE 18
    #endif

    int close_title_size_delta_half = (RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT - RAYGUI_WINDOW_CLOSEBUTTON_SIZE) / 2;

    // window movement and resize input and collision check
    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !*moving && !*resizing) {
        Vector2 mouse_position = GetMousePosition();

        Rectangle title_collision_rect = { position->x, position->y, size->x - (RAYGUI_WINDOW_CLOSEBUTTON_SIZE + close_title_size_delta_half), RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT };
        Rectangle resize_collision_rect = { position->x + size->x - 20, position->y + size->y - 20, 20, 20 };

        if(CheckCollisionPointRec(mouse_position, title_collision_rect)) {
            *moving = true;
        } else if(!*minimized && CheckCollisionPointRec(mouse_position, resize_collision_rect)) {
            *resizing = true;
        }
    }

    // window movement and resize update
    if(*moving) {
        Vector2 mouse_delta = GetMouseDelta();
        position->x += mouse_delta.x;
        position->y += mouse_delta.y;

        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            *moving = false;

            // clamp window position keep it inside the application area
            if(position->x < 0) position->x = 0;
            else if(position->x > GetScreenWidth() - size->x) position->x = GetScreenWidth() - size->x;
            if(position->y < 0) position->y = 0;
            else if(position->y > GetScreenHeight()) position->y = GetScreenHeight() - RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT;
        }

    } else if(*resizing) {
        Vector2 mouse = GetMousePosition();
        if (mouse.x > position->x)
            size->x = mouse.x - position->x;
        if (mouse.y > position->y)
            size->y = mouse.y - position->y;

        // clamp window size to an arbitrary minimum value and the window size as the maximum
        if(size->x < 100) size->x = 100;
        else if(size->x > GetScreenWidth()) size->x = GetScreenWidth();
        if(size->y < 100) size->y = 100;
        else if(size->y > GetScreenHeight()) size->y = GetScreenHeight();

        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            *resizing = false;
        }
    }

    // window and content drawing with scissor and scroll area
    if(*minimized) {
        GuiStatusBar((Rectangle){ position->x, position->y, size->x, RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT }, title);

        if (GuiButton((Rectangle){ position->x + size->x - RAYGUI_WINDOW_CLOSEBUTTON_SIZE - close_title_size_delta_half,
                                   position->y + close_title_size_delta_half,
                                   RAYGUI_WINDOW_CLOSEBUTTON_SIZE,
                                   RAYGUI_WINDOW_CLOSEBUTTON_SIZE },
                                   "#120#")) {
            *minimized = false;
        }

    } else {
        *minimized = GuiWindowBox((Rectangle) { position->x, position->y, size->x, size->y }, title);

        // scissor and draw content within a scroll panel
        if(draw_content != NULL) {
            Rectangle scissor = { 0 };
            GuiScrollPanel((Rectangle) { position->x, position->y + RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT, size->x, size->y - RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT },
                                         NULL,
                                         (Rectangle) { position->x, position->y, content_size.x, content_size.y },
                                         scroll,
                                         &scissor);

            bool require_scissor = size->x < content_size.x || size->y < content_size.y;

            if(require_scissor) {
                BeginScissorMode(scissor.x, scissor.y, scissor.width, scissor.height);
            }

            draw_content(*position, *scroll);

            if(require_scissor) {
                EndScissorMode();
            }
        }

        // draw the resize button/icon
        GuiDrawIcon(71, position->x + size->x - 20, position->y + size->y - 20, 1, WHITE);
    }
}

void GuiWindowFloating(GuiFloatingWindowState *state, void (*draw_content)(Vector2, Vector2), Vector2 content_size, const char* title)
{
    _GuiWindowFloating(&state->position, &state->size, &state->minimized, &state->moving, &state->resizing, draw_content, content_size, &state->scroll, title);
}

#endif
