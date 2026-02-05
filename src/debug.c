#include "debug.h"

void* mem_new(size_t size) {
    void* p = malloc(size);
    if(!p) {
        FATAL("malloc failed");
    }
    return p;
}

void mem_del(void *p) {
    if(p == NULL) {
        FATAL("Null pointer");
    }

    free(p);
}

