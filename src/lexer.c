#include "lexer.h"

const char* keywords[] = {
    "auto", 
    "break", 
    "case", 
    "char", 
    "const", 
    "continue", 
    "default", 
    "do", 
    "double", 
    "else", 
    "enum", 
    "extern", 
    "float", 
    "for", 
    "goto", 
    "if", 
    "int", 
    "long", 
    "register", 
    "return", 
    "short", 
    "signed", 
    "sizeof", 
    "static", 
    "struct", 
    "switch", 
    "typedef", 
    "union", 
    "unsigned", 
    "void", 
    "volatile", 
    "while"
};

const char* identifiers[] = { 
     NULL,        
};

const char* numbers[] = { 
    NULL,
};

const char* symbols[] = {
    "==", "!=", "<=", ">=",
    "+", "-", "*", "/", "%",
    "=", "<", ">",
    "{", "}", "(", ")", "[", "]",
    NULL,
};

Token* tokenize(const char* file_path) {
    INFO("start tokenizing");
    FILE* rfile = fopen(file_path, "r"); // read-only mode
    if (rfile == NULL) {
        ERROR("Error opening a read-only file");
        return NULL; 
    }

    INFO("Setting parameters"); 
    Token* tokens = (Token*)malloc(2000 * sizeof(Token));
    int token_count = 0;
    int c;
    char* buffer = (char*)malloc(20 * sizeof(char));
    int buffer_count = 0;
    INFO("starting tokenizing");
    while((c = fgetc(rfile)) != EOF) {
        if(isspace(c)) continue;

        buffer_count = 0;
        
        while(c != ' ' && c != '\0')
        { 
            if(isdigit(c)) { 
                // Number
                while(isdigit(c)) {
                    buffer[buffer_count++] = c;
                    c = fgetc(rfile);
                }
                buffer[buffer_count] = '\0';
                ungetc(c, rfile);
                tokens[token_count++] = (Token){
                    .type = TOKEN_NUMBER,
                    .lexeme = strdup(buffer)
                };
                break;
            }

            else if(isalpha(c)) {
                while(!isspace(c) && isalpha(c)) {
                    buffer[buffer_count++] = c;
                    c = fgetc(rfile);
                }
                buffer[buffer_count] = '\0';
                ungetc(c, rfile);
                int is_keyword = 0;
                // Keyword
                for(int i=0; keywords[i]!=NULL; i++) {
                    if(strcmp(buffer, keywords[i]) == 0) {
                        tokens[token_count++] = (Token){
                            .type = TOKEN_KEYWORD,
                            .lexeme = strdup(buffer)
                        };
                        is_keyword = 1;
                        break;
                    }
                }

                // Identifier
                if(is_keyword == 0) { 
                    tokens[token_count++] = (Token){
                        .type = TOKEN_IDENTIFIER,
                        .lexeme = strdup(buffer)
                    };
                    break;
                }
            }
                
            // unknown

            // Symbol
            else {
                if(c == '=' || c == '!' || c == '<' || c == '>') {
                    buffer[buffer_count++] = c;
                    c = fgetc(rfile);
                    if(c == '=') {
                        buffer[buffer_count++] = c;
                        c=fgetc(rfile);
                    }
                }
                else {
                    buffer[buffer_count++] = c;
                    c = fgetc(rfile);
                }
                buffer[buffer_count] = '\0';
                ungetc(c, rfile);
                tokens[token_count++] = (Token){
                    .type = TOKEN_SYMBOL, 
                    .lexeme = strdup(buffer)
                };
                break;
            }
        }
    }
    fclose(rfile);
    tokens[token_count] = (Token){TOKEN_UNKNOWN, NULL};
    return tokens;
}

