#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "debug.h"

#define KEYWORD_COUNT 3
extern const char* keywords[];

#define IDENTIFIER_COUNT 3
extern const char* identifiers[]; 
    
#define NUMBER_COUNT 3
extern const char* numbers[];

#define SYMBOL_COUNT 3
extern const char* symbols[];

typedef enum {
    TOKEN_KEYWORD,
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_SYMBOL,
    TOKEN_UNKNOWN,
} TokenType;

typedef struct {
    TokenType type;
    const char* lexeme; // a string
} Token;

Token* tokenize(const char* file_path); 

#endif
