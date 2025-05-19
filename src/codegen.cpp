//
// Created by 27249 on 25-5-18.
//
#include "codegen.h"

#include <common.h>
#include <iostream>

Instruction code[MAX_CODE_SIZE];
int codeIndex = 0;

// 添加一条指令
void emit(const fct f, const int l, const int a) {
    if (codeIndex >= MAX_CODE_SIZE) {
        error("Too many instructions");
        return;
    }
    code[codeIndex].f = f;
    code[codeIndex].l = l;
    code[codeIndex].a = a;
    codeIndex++;
}

// 打印所有指令（方便调试）
void printListCode() {
    printf("\nList of instructions:\n");
    printf("Idx  FCT  L   A\n");
    printf("--- ---- --- ---\n");

    for (int i = 0; i < codeIndex; i++) {
        printf("%-3d %-4s %3d %3d\n",
            i,
            getFctName(code[i].f).c_str(),
            code[i].l,
            code[i].a);
    }
}
void print_code_to_file(FILE *file) {
    fprintf(file, "List of instructions:\n");
    fprintf(file, "Idx  FCT  L   A\n");
    fprintf(file, "--- ---- --- ---\n");

    for (int i = 0; i < codeIndex; i++) {
        fprintf(file, "%-3d %-4s %3d %3d\n",
            i,
            getFctName(code[i].f).c_str(),
            code[i].l,
            code[i].a);
    }

}
std::string getFctName(fct f) {
    switch (f) {
        case LIT: return "lit";
        case OPR: return "opr";
        case LOD: return "lod";
        case STO: return "sto";
        case CAL_: return "cal";
        case INT_: return "int";
        case JMP: return "jmp";
        case JPC: return "jpc";
        default: return "unknown";
    }
}
