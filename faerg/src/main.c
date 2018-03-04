#include "faerg_common.h"
#include "main.h"
#include "faerg_ui.h"

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
static f_bool update_screen(void);
static void redraw_screen(void);
static void render(void);
static void delay(void);
static void poll_events(void);
static void on_resize_window(void);
static void quit(void);
static void init_renderer(void);
static void print_info(void);

static struct F_context f_context;

int main (int argc, char **argv)
{
    /* todo: be able to open application with imagefile, make sure that sandbox entitlements work */
    f_context.window = NULL;
    f_context.texture = NULL;
    f_context.renderer = NULL;
    f_context.quit = false;
    f_context.width = 640;
    f_context.height = 480;
    f_context.framebuffer = NULL;
    f_context.mouse_state_down = false;
    f_context.mouse_event = F_UI_EVENT_MOUSE_NONE;
    f_context.mouse_x = 0;
    f_context.mouse_y = 0;
    f_allocator_init();
    init_video();
    init_data();
    print_info();
    redraw_screen();
    update();
    quit();
    return 0;
}

static void init_video(void) {
    SDL_Init(SDL_INIT_VIDEO);
    f_context.window = SDL_CreateWindow(
                                        "faerg",
                                        SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED,
                                        f_context.width,
                                        f_context.height,
                                        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
                                        );
    init_renderer();
}

static void print_info(void) {
    SDL_version compiled;
    SDL_version linked;
    SDL_VERSION(&compiled);
    SDL_GetVersion(&linked);
    printf("We compiled against SDL version %d.%d.%d ...\n",
           compiled.major, compiled.minor, compiled.patch);
    printf("We are linking against SDL version %d.%d.%d.\n",
           linked.major, linked.minor, linked.patch);
}

static void init_renderer(void) {
    if(f_context.window != NULL) {
        if(f_context.renderer != NULL) {
            SDL_DestroyRenderer(f_context.renderer);
        }
        f_context.renderer = SDL_CreateRenderer(
                                                f_context.window,
                                                -1,
                                                SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
                                                );
        if(f_context.renderer != NULL) {
            if(f_context.texture != NULL) {
                SDL_DestroyTexture(f_context.texture);
            }
            f_context.texture = SDL_CreateTexture(
                                                  f_context.renderer,
                                                  SDL_PIXELFORMAT_RGBA8888,
                                                  SDL_TEXTUREACCESS_STREAMING,
                                                  f_context.width,
                                                  f_context.height
                                                  );
            SDL_GL_SetSwapInterval(1);
        }
    }
}

static void init_data(void) {
    int r;
    if(f_context.framebuffer != NULL) {
        free(f_context.framebuffer);
        f_context.framebuffer = NULL;
    }
    f_context.framebuffer = (unsigned int *) f_alloc((f_context.width * f_context.height) * sizeof(unsigned int), "context framebuffer");
    for(r = 0; r < f_context.width * f_context.height; r++) {
        f_context.framebuffer[r] = 0;
    }
    f_ui_init(f_context);
}

static void update() {
    while (!f_context.quit) {
        f_bool needs_redraw = false;
        poll_events();
        needs_redraw = update_screen();
        if(needs_redraw) {
            redraw_screen();
            f_log("redraw");
        }
        render();
        delay();
    }
}

static void delay(void) {
    /* todo: don't delay if vsync is enabled */
    SDL_Delay(16);
}

static f_bool update_screen(void) {
    f_bool needs_redraw = f_ui_screen_update(f_context);
    return needs_redraw;
}

static void redraw_screen(void) {
    f_ui_screen_draw(f_context);
}

static void render(void) {
    SDL_UpdateTexture(f_context.texture, NULL, f_context.framebuffer, f_context.width * sizeof (Uint32));
    SDL_RenderClear(f_context.renderer);
    SDL_RenderCopy(f_context.renderer, f_context.texture, NULL, NULL);
    SDL_RenderPresent(f_context.renderer);
}

static void quit() {
    if(f_context.texture != NULL) {
        SDL_DestroyTexture(f_context.texture);
    }
    if(f_context.renderer != NULL) {
        SDL_DestroyRenderer(f_context.renderer);
    }
    if(f_context.window != NULL) {
        SDL_DestroyWindow(f_context.window);
    }
    if(f_context.framebuffer != NULL) {
        f_free(f_context.framebuffer);
        f_context.framebuffer = NULL;
    }
    f_ui_cleanup(f_context);
    f_allocator_print_allocations();
    f_allocator_cleanup();
    SDL_Quit();
}

static void on_resize_window(void) {
    init_data();
    init_renderer();
}

static void poll_events(void) {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_CONTROLLERBUTTONDOWN:
            case SDL_CONTROLLERBUTTONUP:
                break;
            case SDL_QUIT:
                f_context.quit = true;
                break;
            case SDL_MOUSEBUTTONDOWN:
                f_context.mouse_x = event.motion.x;
                f_context.mouse_y = event.motion.y;
                f_context.mouse_state_down = true;
                f_context.mouse_event = F_UI_EVENT_MOUSE_DOWN;
                if(event.button.button == SDL_BUTTON_LEFT) {
                    /* todo: action for left/right mouse */
                }
                if(event.button.button == SDL_BUTTON_RIGHT) {
                    /* todo: action for left/right mouse */
                }
                break;
            case SDL_MOUSEMOTION:
                f_context.mouse_x = event.motion.x;
                f_context.mouse_y = event.motion.y;
                break;
            case SDL_MOUSEBUTTONUP:
                f_context.mouse_x = event.motion.x;
                f_context.mouse_y = event.motion.y;
                f_context.mouse_state_down = false;
                f_context.mouse_event = F_UI_EVENT_MOUSE_UP;
                if(event.button.button == SDL_BUTTON_LEFT) {
                    /* todo: action for left/right mouse */
                }
                if(event.button.button == SDL_BUTTON_RIGHT) {
                    /* todo: action for left/right mouse */
                }
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
                        /*SDL_Log("Window %d shown", event.window.windowID);*/
                        break;
                    case SDL_WINDOWEVENT_HIDDEN:
                        /*SDL_Log("Window %d hidden", event.window.windowID);*/
                        break;
                    case SDL_WINDOWEVENT_EXPOSED:
                        /*SDL_Log("Window %d exposed", event.window.windowID);*/
                        break;
                    case SDL_WINDOWEVENT_MOVED:
                        /*SDL_Log("Window %d moved to %d,%d",
                                event.window.windowID, event.window.data1,
                                event.window.data2);*/
                        break;
                    case SDL_WINDOWEVENT_RESIZED:
                        /*SDL_Log("Window %d resized to %dx%d",
                                event.window.windowID, event.window.data1,
                                event.window.data2);*/
                        f_context.width = event.window.data1;
                        f_context.height = event.window.data2;
                        on_resize_window();
                        break;
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                        /*SDL_Log("Window %d size changed to %dx%d",
                                event.window.windowID, event.window.data1,
                                event.window.data2);*/
                        break;
                    case SDL_WINDOWEVENT_MINIMIZED:
                        /*SDL_Log("Window %d minimized", event.window.windowID);*/
                        break;
                    case SDL_WINDOWEVENT_MAXIMIZED:
                        /*SDL_Log("Window %d maximized", event.window.windowID);*/
                        break;
                    case SDL_WINDOWEVENT_RESTORED:
                        /*SDL_Log("Window %d restored", event.window.windowID);*/
                        break;
                    case SDL_WINDOWEVENT_ENTER:
                        SDL_Log("Mouse entered window %d",
                                event.window.windowID);
                        break;
                    case SDL_WINDOWEVENT_LEAVE:
                        /*SDL_Log("Mouse left window %d", event.window.windowID);*/
                        break;
                    case SDL_WINDOWEVENT_FOCUS_GAINED:
                        /*SDL_Log("Window %d gained keyboard focus",
                                event.window.windowID);*/
                        break;
                    case SDL_WINDOWEVENT_FOCUS_LOST:
                        /*SDL_Log("Window %d lost keyboard focus",
                                event.window.windowID);*/
                        break;
                    case SDL_WINDOWEVENT_CLOSE:
                        /*SDL_Log("Window %d closed", event.window.windowID);*/
                        break;
#if SDL_VERSION_ATLEAST(2, 0, 5)
                    case SDL_WINDOWEVENT_TAKE_FOCUS:
                        /*SDL_Log("Window %d is offered a focus", event.window.windowID);*/
                        break;
                    case SDL_WINDOWEVENT_HIT_TEST:
                        /*SDL_Log("Window %d has a special hit test", event.window.windowID);*/
                        break;
#endif
                    default:
                        /*SDL_Log("Window %d got unknown event %d",
                                event.window.windowID, event.window.event);*/
                        break;
                }
                break;
        }
    }
}

