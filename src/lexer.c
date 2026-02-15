#include "kai.h"

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
    "while",
};

const char* symbols[] = {
    "==", "!=", "<=", ">=",
    "+", "-", "*", "/", "%",
    "=", "<", ">",
    "{", "}", "(", ")", "[", "]",
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
    char* buffer = (char*)malloc(200 * sizeof(char));
    int buffer_count = 0;
    INFO("starting tokenizing");
    while((c = fgetc(rfile)) != EOF) {
        if(isspace(c)) continue;

        buffer_count = 0;
                
        while(c != ' ' && c != '\0')
        { 
            if(isdigit(c)) { 
                // Number
                INFO("A number");
                while(isdigit(c)) {
                    buffer[buffer_count++] = c;
                    c = fgetc(rfile);
                }
                buffer[buffer_count] = '\0';
                ungetc(c, rfile);
                tokens[token_count++] = (Token){
                    .type = TOKEN_NUMBER,
                    .lexeme = _strdup(buffer)
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
                
                INFO("A keyword");
                // Keyword
                for(int i=0; i<sizeof(keywords)/sizeof(keywords[0]); i++) {
                    if(strcmp(buffer, keywords[i]) == 0) {
                        tokens[token_count++] = (Token){
                            .type = TOKEN_KEYWORD,
                            .lexeme = _strdup(buffer)
                        };
                        is_keyword = 1;
                        break;
                    }
                }

                INFO("An identifier");
                // Identifier
                if(is_keyword == 0) { 
                    tokens[token_count++] = (Token){
                        .type = TOKEN_IDENTIFIER,
                        .lexeme = _strdup(buffer)
                    };
                    break;
                }
            }

            // Symbol
            else {
                INFO("A symbol");
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
                    .lexeme = _strdup(buffer)
                };
                break;
            }
        }
    }
    fclose(rfile);
    tokens[token_count] = (Token){TOKEN_UNKNOWN, NULL};
    INFO("Finished tokenizing");
    return tokens;
}

