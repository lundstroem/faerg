#include <stdio.h>
#include <string.h>
#include "faerg_ui.h"
#include "faerg_common.h"
#include "faerg_chars_gfx.h"

static unsigned int ui_palette[16] = {
    0x00000000, /* 0 transparent */
    0x000000FF, /* 1 black */
    0x121212FF, /* 2 dark gray */
    0x7F7F7FFF, /* 3 medium gray */
    0xAFAFAFFF, /* 4 light gray */
    0xFFFFFFFF, /* 5 white */
    0xFFFFFFFF, /* 6  */
    0xFFFFFFFF, /* 7  */
    0xFFFFFFFF, /* 8  */
    0xFFFFFFFF, /* 9  */
    0xFFFFFFFF, /* 10  */
    0xFFFFFFFF, /* 11 */
    0xFFFFFFFF, /* 12 */
    0xFFFFFFFF, /* 13 */
    0xFFFFFFFF, /* 14 */
    0xFFFFFFFF, /* 15 */
};

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

static struct F_UI_element *root_view = NULL;
static f_bool needs_redraw = false;

static void calculate_frame_size_from_str(struct F_UI_element *e);
static f_bool intersects(struct F_UI_rect frame, Uint32 x, Uint32 y);
static void element_render(struct F_context f_context, struct F_UI_element *e);
static void element_recursive_update(struct F_context f_context, struct F_UI_element *e);
static void element_recursive_render(struct F_context f_context, struct F_UI_element *e);
static void element_recursive_cleanup(struct F_context f_context, struct F_UI_element *e);
static struct F_UI_element *element_cleanup(struct F_UI_element *e);
static void clear_screen(struct F_context f_context, unsigned int palette_index);
static int get_char_pos(char c);
static void label_render(struct F_context f_context, char *string, int s_x, int s_y, unsigned int color, unsigned int bg_color);
static void char_render(struct F_context f_context, int sprite_x, int sprite_y,
                        int screen_x, int screen_y
                        ,f_bool camera_offset,
                        unsigned int color, unsigned int bg_color);
static void panel_render(struct F_context f_context, int x, int y,
                         int w, int h);


void f_ui_init(struct F_context f_context) {
    
    struct F_UI_element *e = NULL;
    struct F_UI_element *file_element = NULL;
    
    root_view = f_ui_element_new();
    root_view->subviews = f_array_new();
    
    /* menu */
    e = f_ui_element_new();
    e->type = F_UI_MENU_BUTTON;
    e->str = f_str_print(NULL, "File");
    calculate_frame_size_from_str(e);
    f_array_insert(root_view->subviews, e);
    e->subviews = f_array_new();
    file_element = e;
    
    e = f_ui_element_new();
    e->type = F_UI_MENU_FIELD;
    e->str = f_str_print(NULL, "New CMD+N");
    e->frame.y = f_ui_char_height * 1;
    calculate_frame_size_from_str(e);
    f_array_insert(file_element->subviews, e);
    
    e = f_ui_element_new();
    e->type = F_UI_MENU_FIELD;
    e->str = f_str_print(NULL, "Open.. CMD+O");
    e->frame.y = f_ui_char_height * 2;
    calculate_frame_size_from_str(e);
    f_array_insert(file_element->subviews, e);
    
    e = f_ui_element_new();
    e->type = F_UI_MENU_FIELD;
    e->str = f_str_print(NULL, "Save CMD+S");
    e->frame.y = f_ui_char_height * 3;
    calculate_frame_size_from_str(e);
    f_array_insert(file_element->subviews, e);
    
    e = f_ui_element_new();
    e->type = F_UI_MENU_FIELD;
    e->frame.y = f_ui_char_height * 4;
    e->str = f_str_print(NULL, "Save as..");
    calculate_frame_size_from_str(e);
    f_array_insert(file_element->subviews, e);
}

f_bool f_ui_screen_update(struct F_context f_context) {
    needs_redraw = false;
    element_recursive_update(f_context, root_view);
    return needs_redraw;
}

void f_ui_screen_draw(struct F_context f_context) {
    clear_screen(f_context, 1);
    element_recursive_render(f_context, root_view);
}

void f_ui_cleanup(struct F_context f_context) {
    element_recursive_cleanup(f_context, root_view);
}

static void calculate_frame_size_from_str(struct F_UI_element *e) {
    if(e && e->str && e->str) {
        long length = f_str_length(e->str);
        e->frame.w = f_ui_char_width * (int)length;
        e->frame.h = f_ui_char_height;
    }
}

static f_bool intersects(struct F_UI_rect frame, Uint32 x, Uint32 y) {
    if(x > frame.x && y > frame.y && x < frame.x + frame.w && y < frame.y + frame.h) {
        return true;
    } else {
        return false;
    }
}

static void element_recursive_update(struct F_context f_context, struct F_UI_element *e) {
    int i;
    if(e) {
        switch (e->type) {
            case F_UI_LABEL:
                break;
            case F_UI_LABEL_BUTTON:
                break;
            case F_UI_ICON_BUTTON:
                break;
            case F_UI_MENU_BUTTON:
                if(intersects(e->frame, f_context.mouse_x, f_context.mouse_y)) {
                    if(!e->hovered) {
                        needs_redraw = true;
                    }
                    e->hovered = true;
                } else {
                    if(e->hovered) {
                        needs_redraw = true;
                    }
                    e->hovered = false;
                }
                break;
            case F_UI_MENU_FIELD:
                if(intersects(e->frame, f_context.mouse_x, f_context.mouse_y)) {
                    if(!e->hovered) {
                        needs_redraw = true;
                    }
                    e->hovered = true;
                } else {
                    if(e->hovered) {
                        needs_redraw = true;
                    }
                    e->hovered = false;
                }
                break;
            case F_UI_PANEL:
                break;
            case F_UI_UNDEFINED:
                break;
        }
    }

    if(e && e->subviews) {
        for(i = 0; i < e->subviews->size; i++) {
            struct F_UI_element *f_e = e->subviews->data[i];
            if(f_e) {
                element_recursive_update(f_context, f_e);
            }
        }
    }
}

static void element_recursive_render(struct F_context f_context, struct F_UI_element *e) {
    int i;
    
    if(e && e->visible) {
        element_render(f_context, e);
    }
    
    if(e && e->subviews) {
        for(i = 0; i < e->subviews->size; i++) {
            struct F_UI_element *f_e = e->subviews->data[i];
            if(f_e) {
                element_recursive_render(f_context, f_e);
            }
        }
    }
}

static struct F_UI_element *element_cleanup(struct F_UI_element *e) {
    if(e) {
        if(e->subviews) {
            e->subviews = f_array_cleanup(e->subviews);
        }
        if(e->str) {
            e->str = f_str_cleanup(e->str);
        }
        e->parent = NULL;
        f_free(e);
    }
    return NULL;
}

static void element_recursive_cleanup(struct F_context f_context, struct F_UI_element *e) {
    int i;
    
    if(e && e->subviews) {
        for(i = 0; i < e->subviews->size; i++) {
            struct F_UI_element *f_e = e->subviews->data[i];
            if(f_e) {
                element_recursive_cleanup(f_context, f_e);
                e->subviews->data[i] = NULL;
            }
        }
    }
    
    if(e) {
        element_cleanup(e);
    }
}

static void element_render(struct F_context f_context, struct F_UI_element *e) {
    if(e) {
        switch (e->type) {
            case F_UI_LABEL:
                break;
            case F_UI_LABEL_BUTTON:
                break;
            case F_UI_ICON_BUTTON:
                break;
            case F_UI_MENU_BUTTON:
                if(e->hovered) {
                    label_render(f_context, e->str->chars, e->frame.x, e->frame.y, palette[4], palette[0]);
                } else {
                    label_render(f_context, e->str->chars, e->frame.x, e->frame.y, palette[6], palette[0]);
                }
                break;
            case F_UI_MENU_FIELD:
                if(e->hovered) {
                    label_render(f_context, e->str->chars, e->frame.x, e->frame.y, palette[4], palette[0]);
                } else {
                    label_render(f_context, e->str->chars, e->frame.x, e->frame.y, palette[6], palette[0]);
                }
                break;
            case F_UI_PANEL:
                break;
            case F_UI_UNDEFINED:
                break;
        }
    }
}

struct F_UI_rect f_ui_rect_make(int x, int y, int w, int h) {
    struct F_UI_rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
    return rect;
}

struct F_UI_element *f_ui_element_new(void) {
    struct F_UI_element *e = f_alloc(sizeof(struct F_UI_element), "F_UI_element");
    e->pressed = false;
    e->hovered = false;
    e->toggleable = false;
    e->visible = true;
    e->frame = f_ui_rect_make(0, 0, 0, 0);
    e->type = F_UI_UNDEFINED;
    e->subviews = NULL;
    e->parent = NULL;
    return e;
}

void f_ui_subview_add(struct F_UI_element *parent, struct F_UI_element *child) {
    if(parent) {
        if(parent->subviews == NULL) {
            parent->subviews = f_array_new();
        }
        f_array_insert(parent->subviews, child);
        child->parent = parent;
    }
}

static void clear_screen(struct F_context f_context, unsigned int palette_index) {
    int i;
    for(i = 0; i < f_context.width * f_context.height; i++) {
        f_context.framebuffer[i] = palette[palette_index];
    }
}

static void label_render(struct F_context f_context, char *string, int s_x, int s_y, unsigned int color, unsigned int bg_color) {
    int i;
    int len = (int)strlen(string);
    for(i = 0; i < len; i++) {
        char c = string[i];
        int x_char_pos = get_char_pos(c);
        char_render(f_context, s_x + (i*f_ui_char_width), s_y, x_char_pos, 0, true, color, bg_color);
    }
}

static void panel_render(struct F_context f_context, int x, int y,
                         int w, int h) {
    int s_x, s_y;
    int frame_buffer_size = f_context.width * f_context.height;
    for (s_x = 0; s_x < w; s_x++) {
        for (s_y = 0; s_y < h; s_y++) {
            int s_x_p = x+s_x;
            int s_y_p = y+s_y;
            unsigned int pos = s_x_p + s_y_p * f_context.width;
            if(pos < frame_buffer_size) {
                f_context.framebuffer[pos] = palette[7];
            }
        }
    }
}

static void char_render(struct F_context f_context, int sprite_x, int sprite_y,
                        int screen_x, int screen_y
                        ,f_bool camera_offset,
                        unsigned int color, unsigned int bg_color) {
    int sheet_width = 1024;
    int sheet_height = 16;
    int char_width = f_ui_char_width;
    int char_height = f_ui_char_height;
    int width = f_context.width;
    int height = f_context.height;
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
                unsigned int alpha = faerg_chars_gfx[s_x_sheet_p + s_y_sheet_p * sheet_width] & 0xFF;
                unsigned int bg_alpha = bg_color & 0xFF;
                if(alpha > 0) {
                    f_context.framebuffer[s_x_p + s_y_p * width] = color;
                } else if(bg_alpha > 0) {
                    f_context.framebuffer[s_x_p + s_y_p * width] = bg_color;
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
