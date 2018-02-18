#include <SDL2/SDL.h>
#include "main.h"

#ifdef _WIN64
/* define something for Windows (64-bit) */
#define platform_windows
#elif _WIN32
/* define something for Windows (32-bit) */
#define platform_windows
#elif __APPLE__
#include "TargetConditionals.h"
#if TARGET_OS_IPHONE && TARGET_IPHONE_SIMULATOR
/* define something for simulator */
#elif TARGET_OS_IPHONE
/* define something for iphone */
#else
#define TARGET_OS_OSX 1
/* define something for OSX */
#define platform_osx
#include "faerg_mac.h"
#endif
#elif __linux
/* linux */
#elif __unix // all unices not caught above
/* Unix */
#elif __posix
/* POSIX */
#endif

static void init_video(void);
static void init_data(void);
static void update(void);
static void render(void);
static void delay(void);
static void poll_events(void);
static void on_resize_window(void);
static void quit(void);

static struct MainContext main_context;

int main (int argc, char **argv)
{
    /* todo: be able to open application with imagefile */
    main_context.window = NULL;
    main_context.texture = NULL;
    main_context.renderer = NULL;
    main_context.quit = false;
    main_context.width = 640;
    main_context.height = 480;
    init_video();
    update();
    quit();
    return 0;
}

static void init_video(void) {
    
    SDL_Init(SDL_INIT_VIDEO);
    main_context.window = SDL_CreateWindow(
                                           "faerg",
                                           SDL_WINDOWPOS_CENTERED,
                                           SDL_WINDOWPOS_CENTERED,
                                           main_context.width,
                                           main_context.height,
                                           SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
                                           );
    if(main_context.window != NULL) {
        main_context.renderer = SDL_CreateRenderer(
                                                   main_context.window,
                                                   -1,
                                                   SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
                                                   );
        if (main_context.renderer != NULL) {
            main_context.texture = SDL_CreateTexture(
                                                     main_context.renderer,
                                                     SDL_PIXELFORMAT_RGBA8888,
                                                     SDL_TEXTUREACCESS_STREAMING,
                                                     main_context.width,
                                                     main_context.height
                                                     );
            SDL_GL_SetSwapInterval(1);
        }
    }
}

static void init_data(void) {
}

static void update() {
    while (!main_context.quit) {
        poll_events();
        render();
        delay();
    }
}
static void delay(void) {
    SDL_Delay(16);
}

static void render(void) {
    /* todo: update texture from framebuffer if dirty
    SDL_UpdateTexture(main_context.texture, NULL, main_context.framebuffer, main_context.width * sizeof (Uint32));
     */
    SDL_RenderClear(main_context.renderer);
    SDL_RenderCopy(main_context.renderer, main_context.texture, NULL, NULL);
    SDL_RenderPresent(main_context.renderer);
}

static void quit() {
    if(main_context.texture != NULL) {
        SDL_DestroyTexture(main_context.texture);
    }
    if(main_context.window != NULL) {
        SDL_DestroyWindow(main_context.window);
    }
    SDL_Quit();
}

static void on_resize_window(void) {
    /* todo: recreate texture (and maybe also renderer) to the new windowsize */
}

static void poll_events(void) {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_CONTROLLERBUTTONDOWN:
            case SDL_CONTROLLERBUTTONUP:
                break;
            case SDL_QUIT:
                main_context.quit = true;
                break;
            case SDL_MOUSEBUTTONDOWN:
                /* todo: create structure for inputhandling */
                break;
            case SDL_MOUSEMOTION:
                /* todo: create structure for inputhandling */
                break;
            case SDL_MOUSEBUTTONUP:
                /* todo: create structure for inputhandling */
                break;
            case SDL_KEYDOWN:
                /* todo: create structure for inputhandling */
                break;
            case SDL_KEYUP:
                /* todo: create structure for inputhandling */
                break;
                
            case SDL_WINDOWEVENT:
                switch (event.window.event) {
                    case SDL_WINDOWEVENT_SHOWN:
                        SDL_Log("Window %d shown", event.window.windowID);
                        break;
                    case SDL_WINDOWEVENT_HIDDEN:
                        SDL_Log("Window %d hidden", event.window.windowID);
                        break;
                    case SDL_WINDOWEVENT_EXPOSED:
                        SDL_Log("Window %d exposed", event.window.windowID);
                        break;
                    case SDL_WINDOWEVENT_MOVED:
                        SDL_Log("Window %d moved to %d,%d",
                                event.window.windowID, event.window.data1,
                                event.window.data2);
                        break;
                    case SDL_WINDOWEVENT_RESIZED:
                        SDL_Log("Window %d resized to %dx%d",
                                event.window.windowID, event.window.data1,
                                event.window.data2);
                        break;
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                        SDL_Log("Window %d size changed to %dx%d",
                                event.window.windowID, event.window.data1,
                                event.window.data2);
                        break;
                    case SDL_WINDOWEVENT_MINIMIZED:
                        SDL_Log("Window %d minimized", event.window.windowID);
                        break;
                    case SDL_WINDOWEVENT_MAXIMIZED:
                        SDL_Log("Window %d maximized", event.window.windowID);
                        break;
                    case SDL_WINDOWEVENT_RESTORED:
                        SDL_Log("Window %d restored", event.window.windowID);
                        break;
                    case SDL_WINDOWEVENT_ENTER:
                        SDL_Log("Mouse entered window %d",
                                event.window.windowID);
                        break;
                    case SDL_WINDOWEVENT_LEAVE:
                        SDL_Log("Mouse left window %d", event.window.windowID);
                        break;
                    case SDL_WINDOWEVENT_FOCUS_GAINED:
                        SDL_Log("Window %d gained keyboard focus",
                                event.window.windowID);
                        break;
                    case SDL_WINDOWEVENT_FOCUS_LOST:
                        SDL_Log("Window %d lost keyboard focus",
                                event.window.windowID);
                        break;
                    case SDL_WINDOWEVENT_CLOSE:
                        SDL_Log("Window %d closed", event.window.windowID);
                        break;
#if SDL_VERSION_ATLEAST(2, 0, 5)
                    case SDL_WINDOWEVENT_TAKE_FOCUS:
                        SDL_Log("Window %d is offered a focus", event.window.windowID);
                        break;
                    case SDL_WINDOWEVENT_HIT_TEST:
                        SDL_Log("Window %d has a special hit test", event.window.windowID);
                        break;
#endif
                    default:
                        SDL_Log("Window %d got unknown event %d",
                                event.window.windowID, event.window.event);
                        break;
                }
                break;
        }
    }
}



