#ifndef faerg_data_structures_h
#define faerg_data_structures_h

#include "faerg_common.h"

struct F_array {
    void **data;
    unsigned int size;
    unsigned int realloc_increment_size;
};

struct F_array *f_array_new(void);
struct F_array *f_array_with_sizes_new(unsigned int prealloc_size, unsigned int realloc_size);
void f_array_insert(struct F_array *o, void *data);
f_bool f_array_remove(struct F_array *o, void *data, f_bool dealloc);
void f_array_clear(struct F_array *o, f_bool dealloc);
void *f_array_cleanup(struct F_array *o);

#endif /* faerg_data_structures_h */
