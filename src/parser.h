#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lexer.h"
#include "debug.h"

extern Token* current_token;

typedef enum {
    AST_FUNCTION,
    AST_RETURN,
    AST_NUMBER,
} ASTType;

typedef struct AST{
    ASTType type;
    char* value;
    struct AST* child;
} ASTNode;

ASTNode* new_ast_node(ASTType type, const char* value);
Token* peek();
Token* advance();
int expect(TokenType type, const char* lexeme);

ASTNode* AST_parse(Token* tokens);
ASTNode* AST_parse_function();
ASTNode* AST_parse_return_stmt();
void AST_print();

#endif

