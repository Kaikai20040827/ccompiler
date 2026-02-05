#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>
#include <stdlib.h>

#define COLOR_RESET     "\033[0m"
#define COLOR_GREEN     "\033[32m"
#define COLOR_PURPLE    "\033[35m"
#define COLOR_RED       "\033[31m"
#define COLOR_DARK_RED  "\033[91m"

#define EMOJI_INFO      "\u2139\uFE0F"  // ℹ️ Checkmark for "info"
#define EMOJI_WARNING   "\u26A0\uFE0F"  // ⚠️ Exclamation mark for "warning"
#define EMOJI_ERROR     "\u274C"        // ❌Clash for "error"
#define EMOJI_FATAL     "\U0001F4A5"    // 💥Explosion "fatal"

typedef enum {
    INFO,
    WARNING,
    ERROR,
    FATAL,

} DebugLevel; 

typedef enum {
    INVALID_FUNCTION_NAME,
    
    TOKEN_TYPE_ERR,
    LEXEME_ERR
} ErrorType; // Only for throwing and processing error

#define INFO(x) \
    do{\
        printf("%s  %sinfo%s: %s\n", EMOJI_INFO, COLOR_GREEN, COLOR_RESET, x);\
    } while(0)

#define WARNING(x) \
    do{\
        printf("%s  %s %s:%d%s warning%s: %s\n", EMOJI_WARNING, __func__, __FILE__, __LINE__, COLOR_PURPLE, COLOR_RESET, x);\
    } while(0)

#define ERROR(x) \
    do{\
        printf("%s in \"%s\" %s:%d%s error%s: %s\n", EMOJI_ERROR, __func__, __FILE__, __LINE__, COLOR_RED, COLOR_RESET, x);\
    } while(0)

#define FATAL(x) \
    do{\
        printf("%s in \"%s\" %s:%d%s fatal%s: %s\n", EMOJI_FATAL, __func__, __FILE__, __LINE__, COLOR_DARK_RED, COLOR_RESET, x);\
        exit(EXIT_FAILURE);\
    } while(0)

// Memory management
typedef struct {
    void* data;
    size_t elem_size;
    size_t size;
    size_t capacity;
} MemInfo;

void* mem_new(size_t t);
void mem_del(void *x);

#define memnew(size) mem_new(size);

#define memdel(ptr) do {\
        mem_del(ptr);\
        (ptr) = NULL;\
    } while(0)

#endif


