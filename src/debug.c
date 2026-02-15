#include "debug.h"

void* mem_new(size_t size) {
    if (size == 0) {
        FATAL("Byte size cannot be 0");
    }
    void* p = malloc(size);
    if(!p) {
        FATAL("malloc failed");
    }
    return p;
}

void mem_del(void *p, const char* file, int line) {
    if(p == NULL) {
        WARNING("Free null pointer: %s:%d", file, line);    
    }

    free(p);
}

