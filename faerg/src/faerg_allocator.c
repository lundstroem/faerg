#include "faerg_allocator.h"

static struct F_allocator_context p_f_allocator;
static f_bool vlk_track_allocations = true; /* todo: set as flag in common */

void f_allocator_init(void) {
    p_f_allocator.alloc_count = 0;
    p_f_allocator.alloc_track_size = 1000;
    p_f_allocator.alloc_track_cursor = 0;
    p_f_allocator.print_allocs = false;
    p_f_allocator.alloc_ids = NULL;
}

void f_allocator_print_allocation_count(void) {
    f_log("f_allocator alloc count:%d", p_f_allocator.alloc_count);
}

void f_allocator_print_allocations(void) {
    int i;
    if(vlk_track_allocations) {
        for(i = 0; i < p_f_allocator.alloc_track_size; i++) {
            if(p_f_allocator.alloc_ids[i] != NULL) {
                if(!p_f_allocator.alloc_ids[i]->deallocated) {
                    f_log("alloc name: %s size:%zu pointer:%p i:%d", p_f_allocator.alloc_ids[i]->name, p_f_allocator.alloc_ids[i]->size, p_f_allocator.alloc_ids[i]->pointer, i);
                }
            }
        }
    } else {
        f_log("f_allocator - allocation tracking is disabled.");
    }
}

void f_allocator_cleanup(void) {
    int i;
    f_allocator_print_allocations();
    f_allocator_print_allocation_count();
    if(vlk_track_allocations) {
        if(p_f_allocator.alloc_ids != NULL) {
            for (i = 0; i < p_f_allocator.alloc_track_size; i++) {
                if(p_f_allocator.alloc_ids[i] != NULL) {
                    free(p_f_allocator.alloc_ids[i]);
                    p_f_allocator.alloc_ids[i] = NULL;
                }
            }
            free(p_f_allocator.alloc_ids);
            p_f_allocator.alloc_ids = NULL;
        }
    }
}

struct F_allocator_node *f_allocator_node_new(size_t size, void *pointer, char *name) {
    int i;
    struct F_allocator_node *n = NULL;
    if(p_f_allocator.alloc_track_cursor < p_f_allocator.alloc_track_size) {
        if(p_f_allocator.alloc_ids == NULL) {
            p_f_allocator.alloc_ids = malloc(sizeof(struct F_allocator_node*) * p_f_allocator.alloc_track_size);
            for (i = 0; i < p_f_allocator.alloc_track_size; i++) {
                p_f_allocator.alloc_ids[i] = NULL;
            }
        }
        n = malloc(sizeof(struct F_allocator_node));
        n->size = size;
        n->pointer = pointer;
        n->name = name;
        n->deallocated = false;
        n->duplicate_address = false;
        f_allocator_check_duplicates(pointer);
        if(p_f_allocator.print_allocs) {
            f_log("name:%s pointer:%p i:%d", name, pointer, p_f_allocator.alloc_track_cursor);
        }
        p_f_allocator.alloc_ids[p_f_allocator.alloc_track_cursor] = n;
        p_f_allocator.alloc_track_cursor++;
        return n;
    } else {
        return NULL;
    }
}

void f_allocator_expand_track_size(void) {
    int i = 0;
    int new_track_size = p_f_allocator.alloc_track_size + 10000;
    struct F_allocator_node **alloc_ids = malloc(sizeof(struct F_allocator_node*) * new_track_size);
    for(i = 0; i < new_track_size; i++) {
        if(i < p_f_allocator.alloc_track_size) {
            alloc_ids[i] = p_f_allocator.alloc_ids[i];
        } else {
            alloc_ids[i] = NULL;
        }
    }
    p_f_allocator.alloc_track_size = new_track_size;
    free(p_f_allocator.alloc_ids);
    p_f_allocator.alloc_ids = alloc_ids;
    f_log("f_allocator_alloc new size:%d", p_f_allocator.alloc_track_size);
}

void f_allocator_check_duplicates(void *pointer) {
    /* if a tracked and freed alloc has the same address as a new one, prevent it
     from generating errors by setting the duplicate flag.*/
    int i = 0;
    for(i = 0; i < p_f_allocator.alloc_track_size; i++) {
        if(p_f_allocator.alloc_ids[i] != NULL) {
            if(p_f_allocator.alloc_ids[i]->pointer == pointer &&
               p_f_allocator.alloc_ids[i]->deallocated) {
                p_f_allocator.alloc_ids[i]->duplicate_address = true;
            }
        }
    }
}

f_bool f_allocator_remove_tracked_allocation(void *pointer) {
    int i = 0;
    for(i = 0; i < p_f_allocator.alloc_track_size; i++) {
        if(p_f_allocator.alloc_ids[i] != NULL) {
            if(p_f_allocator.alloc_ids[i]->pointer == pointer) {
                if(!p_f_allocator.alloc_ids[i]->deallocated) {
                    p_f_allocator.alloc_ids[i]->deallocated = true;
                    return true;
                } else if(!p_f_allocator.alloc_ids[i]->duplicate_address) {
                    f_log("f_allocator warning: trying to free deallocated pointer. name: %s size:%zu pointer:%p alloc_node:%p", p_f_allocator.alloc_ids[i]->name, p_f_allocator.alloc_ids[i]->size, p_f_allocator.alloc_ids[i]->pointer, p_f_allocator.alloc_ids[i]);
                    return false;
                }
            }
        }
    }
    f_log("f_allocator warning: Could not find pointer:%p to dealloc", pointer);
    return false;
}

void *f_alloc(size_t size, char *name) {
    void *ptr = NULL;
    if(size > 0) {
        ptr = malloc(size);
    }
    if(ptr == NULL) {
        unsigned long ul_size = size;
        f_log("f_alloc: malloc with size %lu returned NULL. name:%s", ul_size, name);
    } else {
        if(vlk_track_allocations) {
            struct F_allocator_node *node = f_allocator_node_new(size, ptr, name);
            if(node == NULL) {
                f_allocator_expand_track_size();
                f_allocator_node_new(size, ptr, name);
            }
        }
        p_f_allocator.alloc_count++;
    }
    return ptr;
}

void *f_free(void *ptr) {
    if(ptr != NULL) {
        f_bool dealloc = true;
        if(vlk_track_allocations) {
            dealloc = f_allocator_remove_tracked_allocation(ptr);
        }
        if(dealloc) {
            free(ptr);
            p_f_allocator.alloc_count--;
        }
    } else {
        if(vlk_track_allocations) {
            f_log("f_allocator_alloc warning: pointer cannot be deallocated because it's NULL.");
        }
    }
    return NULL;
}
