#include "faerg_data_structures.h"

struct F_array *f_array_new(void) {
    return f_array_with_sizes_new(1, 1);
}

struct F_array *f_array_with_sizes_new(unsigned int prealloc_size, unsigned int realloc_size) {
    int i;
    struct F_array *o = NULL;
    if(prealloc_size < 1) {
        prealloc_size = 1;
    }
    o = f_alloc(sizeof(struct F_array), "f_array_new");
    o->data = f_alloc((sizeof(void*) * prealloc_size), "f_array_new prealloc_size");
    o->realloc_increment_size = realloc_size;
    for(i = 0; i < prealloc_size; i++) {
        o->data[i] = NULL;
    }
    o->size = prealloc_size;
    return o;
}

void f_array_insert(struct F_array *o, void *data) {
    int i;
    for (i = 0; i < o->size; i++) {
        if(o->data[i] == NULL) {
            o->data[i] = data;
            return;
        }
    }
    if(o->realloc_increment_size > 0) {
        int new_size = o->size+o->realloc_increment_size;
        void **new_data = f_alloc((sizeof(void*) * new_size), "f_array_new realloc");
        for (i = 0; i < new_size; i++) {
            if(i < o->size) {
                new_data[i] = o->data[i];
            } else {
                new_data[i] = NULL;
            }
        }
        o->data = f_free(o->data);
        o->data = new_data;
        o->data[o->size] = data;
        o->size = new_size;
        f_log("dynamic_array resize:%d", o->size);
    } else {
        f_log("dynamic_array_insert not resizable.");
    }
}

f_bool f_array_remove(struct F_array *o, void *data, f_bool dealloc) {
    int i;
    for (i = 0; i < o->size; i++) {
        if(o->data[i] == data
           && data != NULL) {
            if(dealloc) {
                o->data[i] = f_free(o->data[i]);
            } else {
                o->data[i] = NULL;
            }
            return true;
        }
    }
    return false;
}

void f_array_clear(struct F_array *o, f_bool dealloc) {
    int i;
    for (i = 0; i < o->size; i++) {
        if(o->data[i] != NULL && dealloc) {
            o->data[i] = f_free(o->data[i]);
        } else {
            o->data[i] = NULL;
        }
    }
}

void *f_array_cleanup(struct F_array *o) {
    int i;
    for (i = 0; i < o->size; i++) {
        if(o->data[i] != NULL) {
            o->data[i] = f_free(o->data[i]);
        }
    }
    o->data = f_free(o->data);
    f_free(o);
    return NULL;
}
