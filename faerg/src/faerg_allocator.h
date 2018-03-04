#ifndef faerg_allocator_h
#define faerg_allocator_h

#include <stdio.h>
#include "faerg_common.h"

struct F_allocator_node {
    size_t size;
    void *pointer;
    char *name;
    f_bool deallocated;
    f_bool duplicate_address;
};

struct F_allocator_context {
    int alloc_count;
    int alloc_track_size;
    int alloc_track_cursor;
    f_bool print_allocs;
    struct F_allocator_node **alloc_ids;
};

void f_allocator_init(void);
void f_allocator_set_debuglog(f_bool debuglog);
void f_allocator_print_allocation_count(void);
void f_allocator_print_allocations(void);
void f_allocator_cleanup(void);
void *f_alloc(size_t size, char *name);
void *f_free(void *ptr);
struct F_allocator_node *f_allocator_node_new(size_t size, void *pointer, char *name);
void f_allocator_expand_track_size(void);
void f_allocator_check_duplicates(void *pointer);
f_bool f_allocator_remove_tracked_allocation(void *pointer);

#endif /* faerg_allocator_h */
