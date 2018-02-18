#ifndef main_h
#define main_h

#ifndef vlk_bool_h
#define vlk_bool_h
typedef int fbool;
#define true 1
#define false 0
#endif

struct MainContext {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    fbool quit;
    Uint32 *framebuffer;
    Uint32 width;
    Uint32 height;
};

#endif /* main_h */
