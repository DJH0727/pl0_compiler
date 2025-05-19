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
    for (int i = 0; i < codeIndex; i++) {
        std::cout << i << ": ";
        std::cout << getFctName(code[i].f) << " ";
        std::cout << code[i].l << " ";
        std::cout << code[i].a << std::endl;
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
