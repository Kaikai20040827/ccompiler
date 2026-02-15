#include "kai.h"

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
        .name = name ? _strdup(name) : NULL,
        .value = value
    };

    IR_array_pushback(ira, ir);
}

int gen_return_expr(ASTNode* node) {
    switch (node->type) {
        case AST_RETURN:
            node = node->child;
            return atoi(node->value);
        default:
            FATAL("Unknown ASTType");
    }
}

void ir_codegen(ASTNode* node) {
    if(!node) return;

    switch(node->type) {
        case AST_FUNCTION:
            printf("Function\n");
            ir_codegen(node->child);
            break;
        case AST_RETURN:
            printf("Return\n");
            int ret_expr = gen_return_expr(node);
            emit(IR_RETURN, NULL, ret_expr);
            break;
        default:
            emit(IR_UNKNOWN, NULL, 0);
            break;
    }
}

void dump_ira(IRArray* ira) {
    remove("main.asm");
    FILE* fasm = fopen("main.asm", "w");

    if(!ira)
        FATAL("The IR array doesn't exist");
    if(!fasm)
        FATAL("The assembly file doesn't exist");
    fprintf(fasm, ".globa\n");
    fprintf(fasm, "section .text\n");
    fprintf(fasm, "_start:\n");

    for(int i=0; i<ira->size; i++) {
        IR ir = ira->data[i];
        switch(ir.op) {
                case IR_RETURN:
                fprintf(fasm, "  mov rax, %d\n", 60);
                fprintf(fasm, "  xor rdi, rdi\n");
                fprintf(fasm, "  syscall\n");
                break;
            default:
                fprintf(fasm, "  ; unknown IR\n");
                break;
        }
    }
}
