#include <SDL2/SDL.h>
#include "main.h"
#include "chars_gfx.h"

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
static int get_char_pos(char c);
static void render_label(unsigned int *raster, char *string, int s_x, int s_y, unsigned int color, unsigned int bg_color);
static void render_char(unsigned int *raster, int sprite_x, int sprite_y,
                          int screen_x, int screen_y
                          ,unsigned char camera_offset,
                          unsigned int color, unsigned int bg_color);
static void render_panel(unsigned int *raster, int x, int y,
                         int w, int h);
static void init_renderer(void);
static void print_info(void);

static struct MainContext main_context;

static unsigned int palette[16] = {
    0x00000000, /* 0 transparent */
    0x020C7DFF, /* 1 low blue */
    0x0E7E12FF, /* 2 low green */
    0x107F7FFF, /* 3 low cyan */
    0x7E0308FF, /* 4 low red */
    0x7E0f7eFF, /* 5 low magenta */
    0x007F7FFF, /* 6 low yellow */
    0x7F7F7FFF, /* 7 light gray */
    0x000000FF, /* 8 black */
    0x0B24FBFF, /* 9 high blue */
    0x29FD2FFF, /* 10 high green */
    0x2DFEFEFF, /* 11 high cyan */
    0xFC0d1CFF, /* 12 high red */
    0xFD29FCFF, /* 13 high magenta */
    0xFFFD38FF, /* 14 high yellow */
    0xFFFFFFFF  /* 15 white */
};

int main (int argc, char **argv)
{
    /* todo: be able to open application with imagefile, make sure that sandbox entitlements work */
    main_context.window = NULL;
    main_context.texture = NULL;
    main_context.renderer = NULL;
    main_context.quit = false;
    main_context.width = 640;
    main_context.height = 480;
    main_context.framebuffer = NULL;
    init_video();
    init_data();
    print_info();
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
    if(main_context.window != NULL) {
        if(main_context.renderer != NULL) {
            SDL_DestroyRenderer(main_context.renderer);
        }
        main_context.renderer = SDL_CreateRenderer(
                                                   main_context.window,
                                                   -1,
                                                   SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
                                                   );
        if (main_context.renderer != NULL) {
            if(main_context.texture != NULL) {
                SDL_DestroyTexture(main_context.texture);
            }
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
    int r;
    if(main_context.framebuffer != NULL) {
        free(main_context.framebuffer);
        main_context.framebuffer = NULL;
    }
    main_context.framebuffer = (unsigned int *) malloc((main_context.width*main_context.height) * sizeof(unsigned int));
    for(r = 0; r < main_context.width*main_context.height; r++) {
        main_context.framebuffer[r] = 0;
    }
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
    int i;
    for(i = 0; i < main_context.width * main_context.height; i++) {
        main_context.framebuffer[i] = palette[1];
    }
    render_label(main_context.framebuffer, "test string", 0, 0, palette[15], palette[0]);
    render_panel(main_context.framebuffer, 100, 100, 100, 100);
    /* todo: only update texture from framebuffer if dirty to save cycles, passive rendering. Need object graph for easier detection? */
    SDL_UpdateTexture(main_context.texture, NULL, main_context.framebuffer, main_context.width * sizeof (Uint32));
    SDL_RenderClear(main_context.renderer);
    SDL_RenderCopy(main_context.renderer, main_context.texture, NULL, NULL);
    SDL_RenderPresent(main_context.renderer);
}

static void quit() {
    if(main_context.texture != NULL) {
        SDL_DestroyTexture(main_context.texture);
    }
    if(main_context.renderer != NULL) {
        SDL_DestroyRenderer(main_context.renderer);
    }
    if(main_context.window != NULL) {
        SDL_DestroyWindow(main_context.window);
    }
    if(main_context.framebuffer != NULL) {
        free(main_context.framebuffer);
        main_context.framebuffer = NULL;
    }
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
                        main_context.width = event.window.data1;
                        main_context.height = event.window.data2;
                        on_resize_window();
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

static void render_label(unsigned int *raster, char *string, int s_x, int s_y, unsigned int color, unsigned int bg_color) {
    int i;
    int len = (int)strlen(string);
    for(i = 0; i < len; i++) {
        char c = string[i];
        int x_char_pos = get_char_pos(c);
        render_char(raster, (s_x*8)+(i*8), s_y*12, x_char_pos, 0, 1, color, bg_color);
    }
}

static void render_panel(unsigned int *raster, int x, int y,
                         int w, int h) {
    int s_x, s_y;
    int frame_buffer_size = main_context.width * main_context.height;
    for (s_x = 0; s_x < w; s_x++) {
        for (s_y = 0; s_y < h; s_y++) {
            int s_x_p = x+s_x;
            int s_y_p = y+s_y;
            unsigned int pos = s_x_p + s_y_p * main_context.width;
            if(pos < frame_buffer_size) {
                main_context.framebuffer[pos] = palette[7];
            }
        }
    }
}

static void render_char(unsigned int *raster, int sprite_x, int sprite_y,
                                int screen_x, int screen_y
                                ,unsigned char camera_offset,
                                unsigned int color, unsigned int bg_color) {
    int sheet_width = 1024;
    int sheet_height = 16;
    int char_width = 8;
    int char_height = 12;
    int width = main_context.width;
    int height = main_context.height;
    int s_x, s_y;
    screen_x *= char_width;
    screen_y *= char_height;
    
    for (s_x = 0; s_x < char_width; s_x++) {
        for (s_y = 0; s_y < char_height; s_y++) {
            /* screen offsets */
            int s_x_p = sprite_x+s_x;
            int s_y_p = sprite_y+s_y;
            /* sheet offsets */
            int s_x_sheet_p = screen_x+s_x;
            int s_y_sheet_p = screen_y+s_y;
            /*
            if(camera_offset) {
                s_x_p -= camera_x;
                s_y_p -= camera_y;
            }
            */
            if(s_x_p > -1 && s_y_p > -1 && s_x_p < width && s_y_p < height
               && s_x_sheet_p > -1 && s_y_sheet_p > -1 && s_x_sheet_p < sheet_width && s_y_sheet_p < sheet_height) {
                unsigned int alpha = chars_gfx[s_x_sheet_p + s_y_sheet_p * sheet_width] & 0xFF;
                unsigned int bg_alpha = chars_gfx[s_x_sheet_p + s_y_sheet_p * sheet_width] & 0xFF;
                if(alpha > 0) {
                    raster[s_x_p + s_y_p * width] = chars_gfx[s_x_sheet_p + s_y_sheet_p * sheet_width];
                } else if(bg_alpha > 0) {
                    raster[s_x_p + s_y_p * width] = bg_color;
                }
            }
        }
    }
}

static int get_char_pos(char c) {
    switch (c) {
        case 'A': return 0; break;
        case 'B': return 1; break;
        case 'C': return 2; break;
        case 'D': return 3; break;
        case 'E': return 4; break;
        case 'F': return 5; break;
        case 'G': return 6; break;
        case 'H': return 7; break;
        case 'I': return 8; break;
        case 'J': return 9; break;
        case 'K': return 10; break;
        case 'L': return 11; break;
        case 'M': return 12; break;
        case 'N': return 13; break;
        case 'O': return 14; break;
        case 'P': return 15; break;
        case 'Q': return 16; break;
        case 'R': return 17; break;
        case 'S': return 18; break;
        case 'T': return 19; break;
        case 'U': return 20; break;
        case 'V': return 21; break;
        case 'W': return 22; break;
        case 'X': return 23; break;
        case 'Y': return 24; break;
        case 'Z': return 25; break;
            /*
             case '\x8F': return 26; break;
             case '\x8E': return 27; break;
             case '\x99': return 28; break;
             */
        case 'a': return 29; break;
        case 'b': return 30; break;
        case 'c': return 31; break;
        case 'd': return 32; break;
        case 'e': return 33; break;
        case 'f': return 34; break;
        case 'g': return 35; break;
        case 'h': return 36; break;
        case 'i': return 37; break;
        case 'j': return 38; break;
        case 'k': return 39; break;
        case 'l': return 40; break;
        case 'm': return 41; break;
        case 'n': return 42; break;
        case 'o': return 43; break;
        case 'p': return 44; break;
        case 'q': return 45; break;
        case 'r': return 46; break;
        case 's': return 47; break;
        case 't': return 48; break;
        case 'u': return 49; break;
        case 'v': return 50; break;
        case 'w': return 51; break;
        case 'x': return 52; break;
        case 'y': return 53; break;
        case 'z': return 54; break;
            /*
             case '\x86': return 55; break;
             case '\x84': return 56; break;
             case '\x94': return 57; break;
             */
        case ' ': return 58; break;
        case '0': return 59; break;
        case '1': return 60; break;
        case '2': return 61; break;
        case '3': return 62; break;
        case '4': return 63; break;
        case '5': return 64; break;
        case '6': return 65; break;
        case '7': return 66; break;
        case '8': return 67; break;
        case '9': return 68; break;
        case '.': return 69; break;
        case ',': return 70; break;
        case ':': return 71; break;
        case '-': return 72; break;
        case '\'': return 73; break;
        case '!': return 74; break;
        case '"': return 75; break;
        case '#': return 76; break;
        case '?': return 77; break;
        case '/': return 78; break;
        case '\\': return 79; break;
        case '[': return 80; break;
        case ']': return 81; break;
        case '(': return 82; break;
        case ')': return 83; break;
        case '%': return 84; break;
        case '_': return 85; break;
        case '+': return 86; break;
        case '|': return 87; break;
        case '=': return 88; break;
        case '^': return 89; break;
        case '`': return 90; break;
        case '<': return 91; break;
        case '>': return 92; break;
        default: return 77;
            break;
    }
}
