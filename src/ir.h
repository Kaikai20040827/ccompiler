#ifndef IR_H
#define IR_H

#include <stdio.h>
#include "parser.h"

typedef enum {
    IR_FUNC_BEGIN,
    IR_RETURN,
    IR_FUNC_END,
    IR_UNKNOWN,
} IROp;

typedef struct IR {
    IROp op;
    const char* name;
    int value;
} IR;

typedef struct {
    IR* data;
    int size;
    int capacity;
}IRArray;

void IR_array_init(IRArray* ira);
void IR_array_pushback(IRArray* ira, IR ir);
void emit(IROp op, const char* name, int value);
int gen_return_expr(ASTNode* node);
IR* ir_codegen(ASTNode* node); 

extern IRArray glob_ira;
extern IRArray* ira;

#endif
