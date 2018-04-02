#include "faerg_str.h"
#include <assert.h>

void f_str_resize(struct F_str *str, size_t size) {
    if(str != NULL) {
        char *temp_chars = f_alloc((sizeof(char) * size)+1, "f_str resize");
        if(str->chars != NULL) {
            snprintf(temp_chars, size, "%s", str->chars);
            str->chars = f_free(str->chars);
            str->chars = temp_chars;
        } else {
            snprintf(temp_chars, size, "%s", "");
            str->chars = temp_chars;
        }
        str->buffer_size = (int)size;
    } else {
        f_log("cannot resize NULL str");
    }
}

struct F_array *f_str_tokenize(struct F_str *str, const char delimiter) {
    int i = 0;
    struct F_str *token = NULL;
    struct F_array *array = NULL;
    struct F_str *token_copy = NULL;
    f_bool added = false;
    unsigned long length = 0;
    if(str != NULL) {
        array = f_array_new();
        token = NULL;
        added = false;
        length = f_str_length(str);
        for (i = 0; i < length; i++) {
            char c = f_str_get_char_at(str, i);
            if(c != delimiter) {
                if(token != NULL && token->chars != NULL) {
                    token_copy = f_str_print(token_copy, "%s", token->chars);
                    token = f_str_print(token, "%s%c", token_copy->chars, c);
                } else {
                    token = f_str_print(token, "%c", c);
                }
                added = false;
            } else {
                if(token != NULL) {
                    f_array_insert(array, token);
                    token = NULL;
                    added = true;
                }
            }
        }
        if(!added && token != NULL) {
            f_array_insert(array, token);
        }
    } else {
        f_log("f_str_tokenize str is NULL");
    }
    if(token_copy != NULL) {
        f_str_cleanup(token_copy);
    }
    return array;
}

f_bool f_str_compare(struct F_str *a, struct F_str *b) {
    int i = 0;
    if(a != NULL && b != NULL) {
        unsigned long a_length = f_str_length(a);
        unsigned long b_length = f_str_length(b);
        if(a_length == b_length) {
            f_bool is_same = true;
            for (i = 0; i < a_length; i++) {
                if(f_str_get_char_at(a, i) != f_str_get_char_at(b, i)) {
                    is_same = false;
                }
            }
            return is_same;
        }
    } else {
        f_log("f_str_compare input string is null");
    }
    return false;
}

struct F_str *f_str_print(struct F_str *str, const char *fmt,...) {
    int i = 0;
    char *arg = NULL;
    char *temp_chars = NULL;
    if(str == NULL) {
        const size_t default_size = 256;
        str = f_alloc(sizeof(struct F_str), "f_str");
        temp_chars = f_alloc((sizeof(char) * (default_size+1)), "f_str str print");
        for(i = 0; i < default_size+1; i++) {
            temp_chars[i] = 0;
        }
        str->chars = temp_chars;
        str->buffer_size = default_size;
    }
    if(str != NULL) {
        if(str->chars != NULL) {
            size_t count = str->buffer_size;
            va_list ap;
            va_start(ap, fmt);
            if(str->chars == arg) {
                f_log("f_str_print: input and arg is the same, can lead to undefined behaviour.");
                assert(str->chars != arg);
            }
            vsnprintf(str->chars, count, fmt, ap);
            va_end(ap);
        }
    }
    return str;
}

struct F_str *f_str_cleanup(struct F_str *str) {
    if(str != NULL) {
        if(str->chars != NULL) {
            str->chars = f_free(str->chars);
        }
        f_free(str);
    }
    return NULL;
}

unsigned long f_str_length(struct F_str *str) {
    unsigned long length = 0;
    if(str != NULL) {
        if(str->chars != NULL) {
            length = strlen(str->chars);
        }
    }
    return length;
}

char f_str_get_char_at(struct F_str *str, int pos) {
    char ret = 0;
    if(str != NULL) {
        if(pos < str->buffer_size) {
            ret = str->chars[pos];
        }
    }
    return ret;
}

void f_str_set_char_at(struct F_str *str, int pos, char c) {
    if(str != NULL) {
        if(pos < str->buffer_size) {
            str->chars[pos] = c;
        }
    }
}

char *f_str_get_chars(struct F_str *str) {
    if(str != NULL) {
        if(str->chars != NULL) {
            return str->chars;
        }
    }
    f_log("f_str_get_chars returned NULL");
    return NULL;
}

char *f_str_copy_chars(struct F_str *str) {
    int i = 0;
    char *new_buffer = NULL;
    if(str != NULL) {
        if(str->chars != NULL) {
            new_buffer = f_alloc(sizeof(char)*str->buffer_size + 1, "f_str copy char buffer");
            for(i = 0; i < str->buffer_size + 1; i++) {
                new_buffer[i] = str->chars[i];
            }
            return new_buffer;
        }
    }
    f_log("f_str_copy_chars returned NULL");
    return NULL;
}
