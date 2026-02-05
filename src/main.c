#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "parser.h"
#include "ir.h"
#include "debug.h"

int main() {
    INFO("Compiler is running");
    const Token* tokens = tokenize("test/example.c"); 
    
    for(size_t i=0; tokens[i].lexeme != NULL; i++) {
        printf("{TokenType: , value: %s}\n", tokens[i].lexeme);
    }

    ASTNode* root = AST_parse(tokens);
    
    ir_codegen(root);
    dump_ira(ira);

    for(size_t i=0; tokens[i].lexeme != NULL; i++) {
        free(tokens[i].lexeme);
    }
    
    free(tokens);
    return 0;
        
}
