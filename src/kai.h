#ifndef KAI_H
#define KAI_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <ctype.h>
#include "debug.h"

#ifndef __GNUC__
#define __attribute__(x)
#endif

// Lexer
extern const char* keywords[];
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
    char* lexeme;
} Token;

Token* tokenize(const char* file_path);

// parser.c
extern Token* current_token;

// Expression kind
typedef enum {
    EXPR_LITERAL,
    EXPR_UNARY,
    EXPR_BINARY,
    EXPR_IDENTIFIER,
    EXPR_ASSIGN,
    EXPR_CALLFUNC,
    EXPR_COND,
    EXPR_CAST,
    EXPR_MEMBER,
} ExprKind;

// Statement kind
typedef enum {
    STMT_EXPR,
    STMT_DECL,
    STMT_COND, // If else
    STMT_WHILE,
    STMT_FOR,
    STMT_DOWHILE,
    STMT_SWITCH,
    STMT_CASE,
    STMT_BREAK,
    STMT_CONTINUE,
    STMT_RETURN,
    STMT_BLOCK,
    STMT_EMPTY,
} StmtKind;

typedef enum {
    DECL_VAR,
    DECL_FUNC,
    DECL_PARAM,
    DECL_TYPEDEF,
} DeclKind;

// Unary Operator
typedef enum {
    UOP_PLUS,   // +  Plus       
    UOP_MINUS,  // -  Minus 
    UOP_INCM,   // ++ Increment
    UOP_DECM,   // -- Decrement
    UOP_LNOT,   // !  Logical not
    UOP_BNOT,   // ~  Bitwise not
    UOP_DEREFER,// *  Dereference pointer
    UOP_ADDR,   // &  Address of
    UOP_SIZEOF, // sizeof Size of
    UOP_CAST,   // (int), (float), ... Cast
} UOP;

// Binary operator
typedef enum {
    // Arithmetic
    BOP_ADD,    // +  Addition
    BOP_SUB,    // -  Subtraction
    BOP_MUL,    // *  Multiplication
    BOP_DIV,    // /  Division
    BOP_MOD,    // %  Modulus
    
    // Relational 
    BOP_EQLTO,  // == Equal to 
    BOP_NEQLTO, // != Not equal to 
    BOP_GTHAN,  // >  Greater than
    BOP_LTHAN,  // <  Less than
    BOP_GOREQL, // >= Greater than or equal to
    BOP_LOREQL, // <= Less than or equal to
    
    // Logical 
    BOP_LAND,   // && Logical AND
    BOP_LOR,    // || Logical OR
    // Bitwise
    //...
    
    // Assignment
    BOP_ASN,    // =  Assign
    BOP_ADDASN, // += Add and assign
    BOP_SUBASN, // -= Sub and assign
    BOP_MULASN, // *= mul and assign
    BOP_DIVASN, // /= div and assign
    BOP_MODASN, // %= mod and assign
    // Bitwise ...

    // Miscellaneous 
    BOP_COMMA,  // ,  Comma 
    BOP_MEMACS, // -> Structure pointer member access

} BOP;

// Type
typedef enum {
    TYPE_INT,
    TYPE_LONG,
    TYPE_SHORT,
    TYPE_FLOAT,
    TYPE_DOUBLE,
    TYPE_CHAR,
    TYPE_VOID,
    TYPE_PTR,
    TYPE_ARR,
    TYPE_FUNC,
} TypeKind;

typedef struct Type {
    TypeKind kind;

    size_t size;
    size_t align;
    
    union {
        // Pointer
        struct {
            Type* base;
        } ptr_type;

        // Array
        struct {
            Type* type;
            int capacity;
        } arr_type;

        // Function
        struct {
            Type* ret_type;
            Type** param_typev;
            int param_typec;
        } func_type;
    };
} Type;

// Expression structure
typedef struct Expr{
    ExprKind kind;

    union {
        // Literal
        int intval_expr;
        
        // Identifier
        char* ident_expr;

        // Unary
        struct {
            UOP uop;
            Expr *operand;
        } unary_expr;
        
        // Binary
        struct {
            BOP bop;
            Expr* left;
            Expr* right;
        } binary_expr;
        
        // Call function
        struct {
            Expr* callee;
            Expr** argv;
            int argc;
        } callfunc_expr;
        
        // Conditional
        struct {
            Expr* cond;
            Expr* then_expr;
            Expr* else_expr;
        } cond_expr;
        
        // Type cast 
        struct {
            Type* type;
            Expr* expr;
        } cast_expr;
        
        // Member a.b / a->b
        struct {
            Expr* base;
            char* member;
        } member_expr;

    };
} Expr;

// Statement structure
typedef struct Stmt{
    StmtKind kind;

    union {
        // Expression stmt
        struct {
            Expr* expr;
        } expr_stmt;
        
        // if then else
        struct {
            Expr* cond;
            Stmt* then_stmt;
            Stmt* else_stmt;
        } cond_stmt;
        
        // while loop
        struct {
            Expr* cond;
            Stmt* body;
        } while_stmt;

        // for loop
        struct {
            Expr* init;
            Expr* cond;
            Expr* step;
            Stmt* body;
        } for_stmt;

        // return
        struct {
            Expr* expr;
        } return_stmt;

        // block 
        struct {
            Stmt** stmtv;
            int stmtc;
        } block_stmt;
    };
} Stmt;

// Declaration
typedef struct Decl {
    DeclKind kind;

    Type* type;
    char* name;

    union {
        struct {
            Expr* init;  
        } var_decl;

        struct {
            Decl* param;
            Stmt* body_stmt; // block
        } func_decl;

        struct {    
            Decl** paramv;
            int paramc;
        } param_decl;

        struct {
            // No params
        } typedef_decl;
    };
} Decl;

// ASTNode kind
typedef enum {
    ASTNODE_EXPR,
    ASTNODE_STMT,
    ASTNODE_DECL,
} ASTNodeKind;

// ASTNode structure
typedef struct ASTNode {
    ASTNodeKind kind;
    
    union {
        Expr* expr;
        Stmt* stmt;
        Decl* decl;
    };
} ASTNode;

void append_stmt(Stmt*** stmtv, int* stmtc, Stmt* stmt);
void append_expr();
void append_decl(Decl*** declv, int* declc, Decl* decl);

// General entry, program layer
ASTNode* ast_parse(Token* tokens);
ASTNode* parse_program(Token* token);

// Tool functions as follows:
// Declaration
Decl* parse_decl();
Decl* parse_func_decl(Type* type, char* name);
Decl* parse_var_decl(Type* type, char* name);
Decl* parse_param_decl();
Decl* parse_typedef_decl(Type* type, char* name);

// Statement
Stmt* parse_stmt();
Stmt* parse_block_stmt();
Stmt* parse_expr_stmt();
Stmt* parse_return_stmt();
Stmt* parse_cond_stmt();
Stmt* parse_while_stmt();
Stmt* parse_for_stmt();

// Expression
Expr* parse_expr();
Expr* parse_assign_expr();
Expr* parse_conditional_expr();
Expr* parse_logical_or_expr();
Expr* parse_logical_and_expr();
Expr* parse_equal_expr();
Expr* parse_relational_expr();
Expr* parse_add_expr();
Expr* parse_mul_expr();
Expr* parse_cast_expr();
Expr* parse_unary_expr();
Expr* parse_primary_expr();
Expr* parse_postfix_expr();

extern ASTNode* root;

// ir.c
//typedef enum {
//    IR_ADD,
//    IR_SUB,
//    IR_MUL,
//    IR_DIV,
//
//    IR_MOV, 
//    IR_LOAD,
//    IR_STORE,
//
//    IR_LABEL,
//    IR_JMP,
//    IR_JMP_IF,
//    
//    IR_CALL,
//    IR_RET,
//} IROp;
//
//typedef enum {
//    OPERAND_TEMP,
//    OPERAND_CONST,
//    OPERAND_VAR,
//    OPERAND_LABEL,
//} OperandType;
//
//typedef struct {
//    OperandType type;
//
//    int temp_id;
//    long const_val;
//    char* name;
//} Operand;
//
//typedef struct IRInst {
//    IROp op;
//
//    Operand dst;
//    Operand src1;
//    Operand src2;
//
//    struct IRInst* next;
//} IRInst;
//
//typedef struct BasicBlock {
//    char* name;
//
//    IRInst* head;
//    IRInst* tail;
//
//    struct BasicBlock* next;
//} BasicBlock;
//
//typedef struct {
//    char* name;
//
//    BasicBlock* entry;
//    BasicBlock* blocks;
//
//    int temp_count;
//} IRFunction;
//

// symbol.c
typedef enum {
    
} SymbolTable;

typedef struct {
    char* name;
    int offset;
} Symbol;

// codegen.c
typedef struct {
    int label_id;
    int stack_offset;
    int stack_size;

    HashTable* locals;
} CodeGenContext;

void emit(const char* fmt, ...);
void codegen_program(ASTNode* node);
void codegen_func(Decl* func);

// HashTable
typedef struct {
    char* key;
    char* val;
} HashPair;

typedef struct {
    HashPair* kv;
    size_t capacity;
    size_t size;
} HashTable;

void hashinsert(HashTable* hashtable, char* key, char* val);
void* hashget(HashTable* hashtable, char* key);
void hashdelete(HashTable* hashtable, char* key);

#endif
