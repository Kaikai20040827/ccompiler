#include "kai.h"
#include "debug.h"

static char* i_file_str;
static char* o_file_str;

static bool executable = true;

typedef enum {
    FILE_NONE,
    FILE_C,
    FILE_ASM,
} FileType;

const char* valid_options[] = {
    "-o", "-I", "-S", "-Wall", "-Wextra",
};

// exit and print instruction
static void help_output(int status) {
    fprintf(stderr, "kcc [ -o <path> ] <file>\n");
    exit(status);
}

static void help_asm(int status) {
    fprintf(stderr, "kcc [ -S <path> ]\n");
    exit(status);
}

static bool check_args(char* arg) {
    size_t vo_count = sizeof(valid_options)/sizeof(valid_options[0]);
    for(size_t i=0; i<vo_count; i++) {
        if(strcmp(arg, valid_options[i]) != 0)
            continue;
        else
            return true;
    }
    return false;
}

static bool parse_args(int argc, char** argv) {
    
    for(int i=1; i<argc; i++) {
        if(strcmp(argv[i], "-o") == 0) {
            if(argv[++i]) {
                o_file_str = argv[i];
                execuable = true;
            }
            else 
                help_output(1);
        }

        // -o (output file)
        if(strncmp(argv[i], "-I", 2) == 0) {
            continue;
        }

        // -S (asm file)
        if(strcmp(argv[i], "-S")) {
            if(argv[++i]) {
                o_file_str = argv[i];
            }
            else
                help_asm(1);
        }
        
        // -Wall (Warning all)
        if(strcmp(argv[i], "-Wall")) {
            continue;
        }

        // -Wextra (Extra warnings)
        if(strcmp(argv[i], "-Wextra")) {
            continue;
        }
        
    }
        
    return executable;
}

static void clean_trash() {
    
}

int main(int argc, char** argv) {
    const Token* tokens = tokenize("test/example.c"); 
   
    INFO("Printing tokens");
    for(size_t i=0; tokens[i].lexeme != NULL; i++) {
        printf("{TokenType: , value: %s}\n", tokens[i].lexeme);
    }
    INFO("Finished printing tokens");
    
    if(parse_args(argc, argv)) {
        INFO("Finished parsing arguments");

    }
       
    return 0;
}
