#include "kai.h"

Token* tokens = NULL; // tokens = tokenize();
Token* current_token = tokens;

// Tool functions
static Token* peek() {
    return current_token;
}

static Token* peek_next() {
    return current_token + 1;
}

static bool match(TokenType type, const char* lexeme) {
    if(current_token->type != type) return false; 
    if(lexeme && strcmp(current_token->lexeme, lexeme) != 0) return false;
    return true;
}

static void consume(TokenType type, const char* lexeme) {
    if(!match(type, lexeme)) FATAL("Unexpected token");

    current_token++;
}

static bool is_type_keyword(Token* token) {
    if(!token) return false;
    if(token->type != TOKEN_KEYWORD) return false;

    return strcmp(token->lexeme, "int") == 0 ||
           strcmp(token->lexeme, "long") == 0 ||
           strcmp(token->lexeme, "short") == 0 ||
           strcmp(token->lexeme, "float") == 0 ||
           strcmp(token->lexeme, "double") == 0 ||
           strcmp(token->lexeme, "char") == 0 ||
           strcmp(token->lexeme, "void") == 0; 
}

static bool is_type_cast() {
    if(!match(TOKEN_SYMBOL, "(")) return false;

    Token* next = peek_next();
    if(!next) return false;
    if(strcmp((next + 1)->lexeme, ")") != 0) return false;
    return is_type_keyword(next);
    
}

//ASTNode
static ASTNode* new_astnode(ASTNodeKind kind) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if(!node) FATAL("Failed to allocate memory");

    memset(node, 0, sizeof(ASTNode));
    node->kind = kind;
    return node;
}

static ASTNode* new_expr_astnode(Expr* expr) {
    ASTNode* node = new_astnode(ASTNODE_EXPR);
    node->expr = expr;
    return node;
}

static ASTNode* new_stmt_astnode(Stmt* stmt) {
    ASTNode* node = new_astnode(ASTNODE_STMT);
    node->stmt = stmt;
    return node;
}

static ASTNode* new_decl_astnode(Decl* decl) {
    ASTNode* node = new_astnode(ASTNODE_DECL);
    node->decl = decl;
    return node;
}

// Type
static Type* new_type(TypeKind kind) {
    Type* t = malloc(sizeof(Type));
    if(!t) FATAL("Failed to allocate memory");
    
    memset(t, 0, sizeof(Type));
    t->kind = kind;
    return t;
}

static Type* new_ptr_type(Type* base) {
    Type* t = new_type(TYPE_PTR);
    t->size = sizeof(void*);
    t->align = alignof(void*);
    t->ptr_type.base = base;
    return t;
}

static Type* new_arr_type(Type* type, int capacity) {
    Type* t = new_type(TYPE_ARR);
    t->size = capacity * type->size;
    t->align = type->align;
    t->arr_type.type = type;
    t->arr_type.capacity = capacity;
    return t;
}

static Type* new_func_type(Type* ret_type, Type** param_typev, int param_typec) {
    Type* t = new_type(TYPE_FUNC);
    t->size = 0;
    t->align = 1;
    t->func_type.ret_type = ret_type;
    t->func_type.param_typev = param_typev;
    t->func_type.param_typec = param_typec;
    return t;
}

// Expression
static Expr* new_expr(ExprKind kind) {
    Expr* e = malloc(sizeof(Expr));
    if(!e) FATAL("Failed to allocate memory");
    
    memset(e, 0, sizeof(Expr));
    e->kind = kind;
    return e;
}

static Expr* new_literal_expr(int val) {
    Expr* e = new_expr(EXPR_LITERAL);
    e->intval_expr = val;
    return e;
}

static Expr* new_ident_expr(char* ident) {
    Expr* e = new_expr(EXPR_IDENTIFIER);
    e->ident_expr = strdup(ident);
    return e;
}

static Expr* new_unary_expr(UOP uop, Expr* operand) {
    Expr* e = new_expr(EXPR_UNARY); 
    e->unary_expr.uop = uop;
    e->unary_expr.operand = operand;
    return e;
}

static Expr* new_binary_expr(BOP bop, Expr* left, Expr* right) {
    Expr* e = new_expr(EXPR_BINARY);
    e->binary_expr.bop = bop;
    e->binary_expr.left = left;
    e->binary_expr.right = right;
    return e;
}

static Expr* new_callfunc_expr(Expr* callee, Expr** argv, int argc) {
    Expr* e = new_expr(EXPR_CALLFUNC);
    e->callfunc_expr.callee = callee;
    e->callfunc_expr.argv = argv;
    e->callfunc_expr.argc = argc;
    return e;
}

static Expr* new_cond_expr(Expr* cond, Expr* then_expr, Expr* else_expr) {
    Expr* e = new_expr(EXPR_COND);
    e->cond_expr.cond = cond;
    e->cond_expr.then_expr = then_expr;
    e->cond_expr.else_expr = else_expr;
    return e;
}

static Expr* new_cast_expr(Type* type, Expr* expr) {
    Expr* e = new_expr(EXPR_CAST);
    e->cast_expr.type = type;
    e->cast_expr.expr = expr;
    return e;
}

static Expr* new_member_expr(Expr* base, char* member) {
    Expr* e = new_expr(EXPR_MEMBER);
    e->member_expr.base = base;
    e->member_expr.member = member;
    return e;
}

static Stmt* new_stmt(StmtKind kind) {
    Stmt* s = malloc(sizeof(Stmt));
    if(!s) FATAL("Failed to allocate memory");
    
    memset(s, 0, sizeof(Stmt));
    s->kind = kind;
    return s;
}

static Stmt* new_expr_stmt(Expr* expr) {
    Stmt* s = new_stmt(STMT_EXPR);
    s->expr_stmt.expr = expr;
    return s;
}

// If else
static Stmt* new_cond_stmt(Expr* cond, Stmt* then_stmt, Stmt* else_stmt) {
    Stmt* s = new_stmt(STMT_COND); 
    s->cond_stmt.cond = cond;
    s->cond_stmt.then_stmt = then_stmt;
    s->cond_stmt.else_stmt = else_stmt;
    return s;
}

static Stmt* new_while_stmt(Expr* cond, Stmt* body) {
    Stmt* s = new_stmt(STMT_WHILE);
    s->while_stmt.cond = cond;
    s->while_stmt.body = body;
    return s;
}

static Stmt* new_for_stmt(Expr* init, Expr* cond, Expr* step, Stmt* body) {
    Stmt* s = new_stmt(STMT_FOR);
    s->for_stmt.init = init;
    s->for_stmt.cond = cond;
    s->for_stmt.step = step;
    s->for_stmt.body = body;
    return s;
}

static Stmt* new_return_stmt(Expr* expr) {
    Stmt* s = new_stmt(STMT_RETURN);
    s->return_stmt.expr = expr;
    return s;
}

static Stmt* new_block_stmt(Stmt** stmtv, int stmtc) {
    Stmt* s = new_stmt(STMT_BLOCK);
    s->block_stmt.stmtv = stmtv;
    s->block_stmt.stmtc = stmtc;
    return s;
}

static Decl* new_decl(DeclKind kind, Type* type, char* name) {
    Decl* d = malloc(sizeof(Decl));
    if(!d) FATAL("Failed to allocate memory");
    
    memset(d, 0, sizeof(Decl));
    d->kind = kind;
    d->type = type;
    d->name = name;
    return d;
}

static Decl* new_var_decl(Expr* init) {
    Decl* d = new_decl(DECL_VAR, NULL, NULL);
    d->var_decl.init = init;
    return d;
}

static Decl* new_func_decl(Decl* param, Stmt* body_stmt) {
    Decl* d = new_decl(DECL_FUNC, NULL, NULL);
    d->func_decl.param = param;
    d->func_decl.body_stmt = body_stmt;
    return d;
}

static Decl* new_param_decl(Decl** paramv, int paramc) {
    Decl* d = new_decl(DECL_PARAM, NULL, NULL);
    d->param_decl.paramv = paramv;
    d->param_decl.paramc = paramc;
    return d;
}

static Decl* new_typedef_decl() {
    Decl* d = new_decl(DECL_TYPEDEF, NULL, NULL);
    return d;
}

// Type
static Type* parse_type() {
    if(match(TOKEN_KEYWORD, "int")) {
        Type* type_int = new_type(TYPE_INT);
        type_int->size = sizeof(int);
        type_int->align = alignof(int);
        consume(TOKEN_KEYWORD, "int");
        return type_int;
    }
    
    if(match(TOKEN_KEYWORD, "long")) {
        Type* type_long = new_type(TYPE_LONG);
        type_long->size = sizeof(long);
        type_long->align = alignof(long);
        consume(TOKEN_KEYWORD, "long");
        return type_long;
    }
   
    if(match(TOKEN_KEYWORD, "short")) {
        Type* type_short = new_type(TYPE_SHORT);
        type_short->size = sizeof(short);
        type_short->align = alignof(short);
        consume(TOKEN_KEYWORD, "short");
        return type_short;
    }   
    
    if(match(TOKEN_KEYWORD, "float")) {
        Type* type_float = new_type(TYPE_FLOAT);
        type_float->size = sizeof(float);
        type_float->align = alignof(float);
        consume(TOKEN_KEYWORD, "float");
        return type_float;
    }

    if(match(TOKEN_KEYWORD, "double")) {
        Type* type_double = new_type(TYPE_DOUBLE);
        type_double->size = sizeof(double);
        type_double->align = alignof(double);
        consume(TOKEN_KEYWORD, "double");
        return type_double;
    }

    if(match(TOKEN_KEYWORD, "char")) {
        Type* type_char = new_type(TYPE_CHAR);
        type_char->size = sizeof(char);
        type_char->align = alignof(char);
        consume(TOKEN_KEYWORD, "char");
        return type_char;
    }

    if(match(TOKEN_KEYWORD, "void")) {
        Type* type_void = new_type(TYPE_VOID);
        type_void->size = 0;
        type_void->align = 1;
        consume(TOKEN_KEYWORD, "void");
        return type_void;
    }

    while(match(TOKEN_SYMBOL, "*")) {
        Type* base;
        consume(TOKEN_SYMBOL, "*");
        base = new_ptr_type(base);
    }
    
    return NULL;
}

// Name(Identifier)
static char* parse_name() {
    if(current_token->type != TOKEN_IDENTIFIER) FATAL("Unexpected name");
    char* name = current_token->lexeme;
    consume(TOKEN_IDENTIFIER, NULL);
    return name;
}

// Parser
// General entry, program layer
ASTNode* ast_parse(Token* tokens) {
    current_token = tokens;
    root = parse_program();
    return root;
}

ASTNode* parse_program() {
    Decl** declv = NULL;
    int declc = 0;

    while(!match(TOKEN_EOF, NULL)) {
        Decl* d = parse_decl();
        append_decl(&declv, &declc, d);
    }

    return new_program_astnode(declv, declc);
}

// Tool functions as follows:
// Declaration
Decl* parse_decl() {
    Type* type = parse_type();
    char* name = parse_name();
    
    if(match(TOKEN_SYMBOL, "(")) {
        return parse_func_decl(type, name);
    } 
    else {
        return parse_var_decl(type, name);
    }

}

Decl* parse_func_decl(Type* type, char* name) {
    consume(TOKEN_SYMBOL, "(");
    Decl* param = NULL;
    if(!match(TOKEN_SYMBOL, ")"))
        param = parse_param_decl();
    
    consume(TOKEN_SYMBOL, ")");
    
    Stmt* body = parse_block_stmt();
    Decl* func = new_func_decl(param, body);
    func->type = type;
    func->name = name;

    return func;
}

Decl* parse_var_decl(Type* type, char* name) {
    Expr* init = NULL;

    if(match(TOKEN_SYMBOL, "=")) {
       consume(TOKEN_SYMBOL, "=");
       init = parse_expr();
    }

    consume(TOKEN_SYMBOL, ";");
    Decl* var = new_var_decl(init);
    var->type = type;
    var->name = name;

    return var;
}

Decl* parse_param_decl() {
    Decl** paramv = NULL;
    int paramc = 0;
        
    do {
        Type* type = parse_type();
        char* name = parse_name();
        consume(TOKEN_IDENTIFIER, NULL);
        Decl* param = new_decl(DECL_VAR, type, name);
        append_decl(&paramv, &paramc, param);

        if(!match(TOKEN_SYMBOL, ",")) break;
        consume(TOKEN_SYMBOL, ",");
    } while(1);
    return new_param_decl(paramv, paramc);
}

Decl* parse_typedef_decl() {
    return NULL;
}

// Statement
Stmt* parse_stmt() {
    if(match(TOKEN_SYMBOL, "{")) 
        return parse_block_stmt();
    if(match(TOKEN_KEYWORD, "while")) 
        return parse_while_stmt();
    if(match(TOKEN_KEYWORD, "for"))
        return parse_for_stmt();
    if(match(TOKEN_KEYWORD, "return"))
        return parse_return_stmt();
    if(match(TOKEN_KEYWORD, "if")) 
        return parse_cond_stmt();

    return parse_expr_stmt();
}

Stmt* parse_block_stmt() {
    consume(TOKEN_SYMBOL, "{");

    Stmt** stmtv = NULL;
    int stmtc = 0;
    while(!match(TOKEN_SYMBOL, "}")) {
        Stmt* stmt = NULL;
        if(is_type_keyword(peek()))
            stmt = parse_decl_stmt();
        else 
            stmt = parse_stmt();
        
        append_stmt(&stmtv, &stmtc, stmt);
    }
    consume(TOKEN_SYMBOL, "}");
    return new_block_stmt(stmtv, stmtc);
}

Stmt* parse_return_stmt() {
    consume(TOKEN_KEYWORD, "return");

    Expr* expr = NULL;
    if(!match(TOKEN_SYMBOL, ";")) 
        expr = parse_expr();
    consume(TOKEN_SYMBOL, ";");
    return new_return_stmt(expr);
}

Stmt* parse_cond_stmt() {
    consume(TOKEN_KEYWORD, "if");
    consume(TOKEN_SYMBOL, "(");
    Expr* cond = parse_expr();
    consume(TOKEN_SYMBOL ,")");
    Stmt* then_stmt = parse_stmt();
    Stmt* else_stmt = NULL;

    if(match(TOKEN_KEYWORD, "else")) {
        consume(TOKEN_KEYWORD, "else");
        else_stmt = parse_stmt();
    }
    return new_cond_stmt(cond, then_stmt, else_stmt);
        
}

Stmt* parse_while_stmt() {
    consume(TOKEN_KEYWORD, "while");
    consume(TOKEN_SYMBOL, "(");

    Expr* cond = parse_expr();
    consume(TOKEN_SYMBOL, ")");
    Stmt* body = parse_stmt();
    return new_while_stmt(cond, body);
}

Stmt* parse_for_stmt() {
    consume(TOKEN_KEYWORD, "for");
    consume(TOKEN_SYMBOL, "(");

    Expr* init = NULL;
    Expr* cond = NULL;
    Expr* step = NULL;
    if(!match(TOKEN_SYMBOL, ";"))
        init = parse_expr();
    consume(TOKEN_SYMBOL, ";");
    if(!match(TOKEN_SYMBOL, ";"))
        cond = parse_expr();
    consume(TOKEN_SYMBOL, ";");
    if(!match(TOKEN_SYMBOL, ")"))
        step = parse_expr();
    consume(TOKEN_SYMBOL, ")");

    Stmt* body = parse_stmt();

    return new_for_stmt(init, cond, step, body);
}

Stmt* parse_expr_stmt() {
    if(match(TOKEN_SYMBOL, ";")) {
        consume(TOKEN_SYMBOL, ";");
        return new_expr_stmt(NULL);
    }
    Expr* expr = parse_expr();
    consume(TOKEN_SYMBOL, ";");

    return new_expr_stmt(expr);
}

// Expression
Expr* parse_expr() {
    return parse_assign_expr();
}

Expr* parse_assign_expr() {
    Expr* left = parse_cond_expr();

    if(match(TOKEN_SYMBOL, "=")) {
        consume(TOKEN_SYMBOL, "=");
        return new_binary_expr(BOP_ASN, left, parse_assign_expr());
    }

    return left;
}

Expr* parse_cond_expr() {
    Expr* cond = parse_logical_or_expr();

    if(match(TOKEN_SYMBOL, "?")) {
        consume(TOKEN_SYMBOL, "?");
        Expr* then_expr = parse_expr();

        consume(TOKEN_SYMBOL, ":");
        Expr* else_expr = parse_cond_expr();

        return new_cond_expr(cond, then_expr, else_expr);
    }
    return cond;
}

Expr* parse_cast_expr() {
    if(is_type_cast()) {
        consume(TOKEN_SYMBOL, "(");
        Type* type = parse_type();
        consume(TOKEN_SYMBOL, ")");
        
        Expr* expr = parse_cast_expr();
        
        return new_cast_expr(type, expr);
    }

    return parse_postfix_expr(); 
}

Expr* parse_logical_or_expr() {
    Expr* left = parse_logical_and_expr();

    while(match(TOKEN_SYMBOL, "||")) {
        consume(TOKEN_SYMBOL, "||");
        Expr* right = parse_logical_and_expr();
        left = new_binary_expr(BOP_LOR, left, right);
    }

    return left;
}

Expr* parse_logical_and_expr() {
    Expr* left = parse_equal_expr();
    
    while(match(TOKEN_SYMBOL, "&&")) {
        consume(TOKEN_SYMBOL, "&&");
        Expr* right = parse_equal_expr();
        left = new_binary_expr(BOP_LAND, left, right);
    }
    return left;
}

Expr* parse_equal_expr() {
    Expr* left = parse_relational_expr();

    while(match(TOKEN_SYMBOL, "==") ||
          match(TOKEN_SYMBOL, "!=")) {
        if(match(TOKEN_SYMBOL, "==")) {
            consume(TOKEN_SYMBOL, "==");
            Expr* right = parse_relational_expr();
            left = new_binary_expr(BOP_EQLTO, left, right);
        }

        else {
            consume(TOKEN_SYMBOL, "!=");
            Expr* right = parse_relational_expr();
            left = new_binary_expr(BOP_NEQLTO, left, right);
        }
    }
    return left;
}

Expr* parse_relational_expr() {
    Expr* left = parse_add_expr();

    while(match(TOKEN_SYMBOL, "<") || 
          match(TOKEN_SYMBOL, ">") ||
          match(TOKEN_SYMBOL, ">=") ||
          match(TOKEN_SYMBOL, "<=") ) {
        if(match(TOKEN_SYMBOL, "<")) {
            consume(TOKEN_SYMBOL, "<");
            Expr* right = parse_add_expr();
            left = new_binary_expr(BOP_LTHAN, left, right);
        }
        else if(match(TOKEN_SYMBOL, ">")) {
            consume(TOKEN_SYMBOL, ">");
            Expr* right = parse_add_expr();
            left = new_binary_expr(BOP_GTHAN, left, right);
        }
        else if(match(TOKEN_SYMBOL, "<=")) {
            consume(TOKEN_SYMBOL, "<=");
            Expr* right = parse_add_expr();
            left = new_binary_expr(BOP_LOREQL, left, right);
        }
        else {
            consume(TOKEN_SYMBOL, ">=");
            Expr* right = parse_add_expr();
            left = new_binary_expr(BOP_GOREQL, left, right);
        }
    }
    return left;
}

Expr* parse_add_expr() {
    Expr* left = parse_mul_expr();

    while(match(TOKEN_SYMBOL, "+") ||
          match(TOKEN_SYMBOL, "-")) {

        if(match(TOKEN_SYMBOL, "+")) {
            consume(TOKEN_SYMBOL, "+");
            left = new_binary_expr(BOP_ADD, left, parse_mul_expr());
        }
        if(match(TOKEN_SYMBOL, "-")) {
            consume(TOKEN_SYMBOL, "-");
            left = new_binary_expr(BOP_SUB, left, parse_mul_expr());
        }
    }
    return left;
}

Expr* parse_mul_expr() {
    Expr* left = parse_unary_expr();

    while(match(TOKEN_SYMBOL, "*") || 
          match(TOKEN_SYMBOL, "/") ||
          match(TOKEN_SYMBOL, "%")) {
        if(match(TOKEN_SYMBOL, "*")) {
            consume(TOKEN_SYMBOL, "*");
            left = new_binary_expr(BOP_MUL, left, parse_unary_expr());
        }
        else if(match(TOKEN_SYMBOL, "/")) {
            consume(TOKEN_SYMBOL, "/");
            left = new_binary_expr(BOP_DIV, left, parse_unary_expr());
        }   
        else {
            consume(TOKEN_SYMBOL, "%");
            left = new_binary_expr(BOP_MOD, left, parse_unary_expr());
        }
    }
    return left;
}

Expr* parse_unary_expr() {
    if(match(TOKEN_SYMBOL, "+")) {
        consume(TOKEN_SYMBOL, "+");
        return new_unary_expr(UOP_PLUS, parse_unary_expr());
    }
    if(match(TOKEN_SYMBOL, "-")) {
        consume(TOKEN_SYMBOL, "-");
        return new_unary_expr(UOP_MINUS, parse_unary_expr());
    }
    if(match(TOKEN_SYMBOL, "++")) {
        consume(TOKEN_SYMBOL, "++");
        return new_unary_expr(UOP_INCM, parse_unary_expr());
    }
    if(match(TOKEN_SYMBOL, "--")) {
        consume(TOKEN_SYMBOL, "--");
        return new_unary_expr(UOP_DECM, parse_unary_expr());
    }
     if(match(TOKEN_SYMBOL, "!")) {
        consume(TOKEN_SYMBOL, "!");
        return new_unary_expr(UOP_LNOT, parse_unary_expr());
    }
      if(match(TOKEN_SYMBOL, "~")) {
        consume(TOKEN_SYMBOL, "~");
        return new_unary_expr(UOP_BNOT, parse_unary_expr());
    }
     if(match(TOKEN_SYMBOL, "*")) {
        consume(TOKEN_SYMBOL, "*");
        return new_unary_expr(UOP_DEREFER, parse_unary_expr());
    }
    if(match(TOKEN_SYMBOL, "&")) {
        consume(TOKEN_SYMBOL, "&");
        return new_unary_expr(UOP_ADDR, parse_unary_expr());
    }
    if(match(TOKEN_KEYWORD, "sizeof")) {
        consume(TOKEN_KEYWORD, "sizeof");
        return new_unary_expr(UOP_SIZEOF, parse_unary_expr());
    }
    // cast
    return parse_cast_expr();
}

Expr* parse_primary_expr() {
    // Number
    if(match(TOKEN_NUMBER, NULL)) {
        int val = atoi(current_token->lexeme);
        consume(TOKEN_NUMBER, NULL);
        return new_literal_expr(val);
    }
    // Identifier 
    if(match(TOKEN_IDENTIFIER, NULL)) {
        char* name = current_token->lexeme;
        consume(TOKEN_IDENTIFIER, NULL);
        return new_ident_expr(name);
    }
    // Expression 
    if(match(TOKEN_SYMBOL, "(")) {
        consume(TOKEN_SYMBOL, "(");
        Expr* expr = parse_expr();
        consume(TOKEN_SYMBOL, ")");
        return expr;
    }
    FATAL("Unexpected token in primary expression");
}

Expr* parse_postfix_expr() {

    Expr* expr = parse_primary_expr();

    while (1) {

        // function call
        if(match(TOKEN_SYMBOL, "(")) {

            consume(TOKEN_SYMBOL, "(");

            Expr** argv = NULL;
            int argc = 0;

            if(!match(TOKEN_SYMBOL, ")")) {

                do {
                    Expr* arg = parse_expr();
                    append_expr(&argv, &argc, arg);

                    if(!match(TOKEN_SYMBOL, ",")) break;
                    consume(TOKEN_SYMBOL, ",");
                } while(1);
            }

            consume(TOKEN_SYMBOL, ")");

            expr = new_callfunc_expr(expr, argv, argc);
            continue;
        }

        // member access
        if(match(TOKEN_SYMBOL, ".")) {

            consume(TOKEN_SYMBOL, ".");

            char* name = current_token->lexeme;
            consume(TOKEN_IDENTIFIER, NULL);

            expr = new_member_expr(expr, name);
            continue;
        }

        break;
    }

    return expr;
}

