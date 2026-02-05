#include "ir.h"

IRArray glob_ira;
IRArray* ira = &glob_ira;

void IR_array_init(IRArray* ira) {
    ira->data = malloc(4 * sizeof(IR));
    ira->size = 0;
    ira->capacity = 4;

    if(ira->data == NULL) FATAL("IR array failed");
}

void IR_array_pushback(IRArray* ira, IR ir) {
    if(ira->size == ira->capacity) {
        IR* new_data = malloc(ira->capacity * 2 * sizeof(IR));
        for(int i=0; i<ira->size; i++) {
            new_data[i] = ira->data[i];
        }
        free(ira->data);
        ira->data = new_data;
        ira->capacity *= 2;
    }
    ira->data[ira->size] = ir;
    ira->size++;
}

void emit(IROp op, const char* name, int value) {
    IR ir = (IR) {
        .op = op,
        .name = name,
        .value = value
    };

    IR_array_pushback(ira, ir);
}

int gen_return_expr(ASTNode* node) {
    switch (node->type) {
        case AST_NUMBER:
            return node->value;
    }
}

IR* ir_codegen(ASTNode* node) {
    if(!node) return NULL;

    switch(node->type) {
        case AST_RETURN:
            int ret_expr = gen_return_expr(node->child);
            emit(IR_RETURN, NULL, ret_expr);

        default:
            emit(IR_UNKNOWN, NULL, 0);
            break;
    }
    return ira->data;
}

void dump_ira(IRArray* ira) {
    FILE* fasm = fopen("main.s", "w");

    if(!ira || !fasm) return;

    fprintf(fasm, "global main\n");
    fprintf(fasm, "main:\n");
    
    for(int i=0; i<ira->size; i++) {
        IR ir = ira->data[i];
        switch(ir.op) {
                case IR_RETURN:
                fprintf(fasm, "  mov rax, 0\n");
                fprintf(fasm, "  ret\n");
                break;
            default:
                fprintf(fasm, "  ; unknown IR\n");
                break;
        }
    }
}
