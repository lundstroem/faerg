#ifndef faerg_ui_h
#define faerg_ui_h

#include "faerg_common.h"

enum F_UI_element_type {
    F_UI_LABEL,
    F_UI_LABEL_BUTTON,
    F_UI_ICON_BUTTON,
    F_UI_MENU_BUTTON,
    F_UI_MENU_FIELD,
    F_UI_PANEL,
    F_UI_UNDEFINED
};

struct F_UI_rect {
    int x;
    int y;
    int w;
    int h;
};

struct F_UI_element {
    f_bool pressed;
    f_bool hovered;
    f_bool toggleable;
    f_bool visible;
    struct F_UI_rect frame;
    enum F_UI_element_type type;
    struct F_array *subviews;
    struct F_str *str;
    struct F_UI_element *parent;
};

void f_ui_init(struct F_context f_context);
f_bool f_ui_screen_update(struct F_context f_context);
void f_ui_screen_draw(struct F_context f_context);
void f_ui_cleanup(struct F_context f_context);
struct F_UI_rect f_ui_rect_make(int x, int y, int w, int h);
struct F_UI_element *f_ui_element_new(void);
void f_ui_subview_add(struct F_UI_element *parent, struct F_UI_element *child);

#endif /* faerg_ui_h */
