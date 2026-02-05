#include "parser.h"

Token* current_token = NULL;

ASTNode* new_ast_node(ASTType type, const char* value) {
    ASTNode* node = memnew(1 * sizeof(ASTNode));
    node->type = type;
    node->value = value ? strdup(value) : NULL;
    node->child = NULL;
    return node;
}

Token* peek() {
    return current_token;
}

Token* advance() {
    return current_token++;
}

int expect(TokenType type, const char* lexeme) {
    if(current_token->type != type) {
        printf("\n");
        return 0;
    }
    if(lexeme && strcmp(current_token->lexeme, lexeme) != 0) {
        printf("\n");
        return 0;
    }
    
    current_token++;
    return 1;
}

ASTNode* AST_parse(Token* tokens) {
    INFO("AST structure: ");
    printf("Program\n");
    current_token = tokens; // point at the first address of token array
    return AST_parse_function();
}

ASTNode* AST_parse_function() {
    //printf("└── Function\n");
    
    Token* ret_type = advance();
    if(ret_type->type != TOKEN_KEYWORD) FATAL("Return type error");
    //printf("    ├── return_type: %s\n", ret_type->lexeme);

    Token* func_name = advance();
    if(func_name->type != TOKEN_IDENTIFIER) FATAL("Function name error");
    //printf("    ├── name: %s\n", func_name->lexeme);

    if(!expect(TOKEN_SYMBOL, "(")) FATAL("Symbol error");
    if(!expect(TOKEN_SYMBOL, ")")) FATAL("Symbol error");
    if(!expect(TOKEN_SYMBOL, "{")) FATAL("Symbol error");
    
    printf("    └── Body\n");
    ASTNode* body = AST_parse_return_stmt();

    if(!expect(TOKEN_SYMBOL, "}")) FATAL("Token error");

    ASTNode* func = new_ast_node(AST_FUNCTION, func_name->lexeme);
    func->child = body;
    return func;
}

ASTNode* AST_parse_return_stmt() {
    Token* ret_ident = advance(); // identifier "return"
    if(ret_ident->type != TOKEN_KEYWORD) FATAL("keyword error");
    printf("        └── return\n");
    
    Token* num = advance();
    printf("            └── number: %s\n", num->lexeme);
    
    if(!expect(TOKEN_SYMBOL, ";")) FATAL("Token error");

    ASTNode* ret = new_ast_node(AST_RETURN, NULL);
    ret->child = new_ast_node(AST_NUMBER, num->lexeme);
    return ret;
}

void AST_destroy(ASTNode* root) {
    if(root == NULL) {
        return;
    }
    AST_destroy(root->child);
    
    memdel(root->value);
    memdel(root);
}
