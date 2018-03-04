#ifndef faerg_str_h
#define faerg_str_h

#include <stdio.h>
#include "faerg_common.h"

struct F_str {
    char *chars;
    int buffer_size;
};

void f_str_resize(struct F_str *str, size_t size);
struct F_array *f_str_tokenize(struct F_str *str, const char delimiter);
f_bool f_str_compare(struct F_str *a, struct F_str *b);
struct F_str *f_str_print(struct F_str *str, const char *fmt,...);
unsigned long f_str_length(struct F_str *str);
char f_str_get_char_at(struct F_str *str, int pos);
void f_str_set_char_at(struct F_str *str, int pos, char c);
char *f_str_get_chars(struct F_str *str);
char *f_str_copy_chars(struct F_str *str);
struct F_str *f_str_cleanup(struct F_str *str);

#endif /* faerg_str_h */
