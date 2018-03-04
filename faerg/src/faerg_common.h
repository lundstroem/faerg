#ifndef faerg_common_h
#define faerg_common_h

#ifndef f_bool_h
#define f_bool_h
typedef int f_bool;
#define true 1
#define false 0
#endif

#define f_ui_char_width 8
#define f_ui_char_height 12

#define f_log_enabled 1

#include <SDL2/SDL.h>
#include "faerg_allocator.h"
#include "faerg_data_structures.h"
#include "faerg_str.h"

enum F_UI_mouse_event {
    F_UI_EVENT_MOUSE_UP,
    F_UI_EVENT_MOUSE_DOWN,
    F_UI_EVENT_MOUSE_NONE
};

struct F_context {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    f_bool quit;
    Uint32 *framebuffer;
    Uint32 width;
    Uint32 height;
    Uint32 mouse_x;
    Uint32 mouse_y;
    f_bool mouse_state_down;
    enum F_UI_mouse_event mouse_event;
};

void f_log(const char *fmt,...);


#endif /* common_h */
