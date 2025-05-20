//
// Created by 27249 on 25-5-18.
//
#include "codegen.h"

#include <common.h>
#include <iostream>
#include <sstream>

Instruction code[MAX_CODE_SIZE];//p-code
std::string originalCode[MAX_CODE_SIZE];//原始代码
std::string labeledCode[MAX_CODE_SIZE];//带标签的代码
int codeIndex = 0;
int lineCounter = 1;
int lastLineCounter = 0;//上一次打印的行号
int originalLineCount = 0;

void init_code(const char* original_code) {
    //将code转化为字符串数组，根据换行符分割
    const std::string codeStr(original_code);
    std::istringstream iss(codeStr);
    std::string line;


    while (std::getline(iss, line)) {
        if (originalLineCount >= MAX_CODE_SIZE) break;
        originalCode[originalLineCount++] = line;
    }
}

void print_label_code() {
    for (int i = 0; i < originalLineCount; i++) {
        std::cout <<labeledCode[i] << std::endl;
    }
}
void print_label_code_to_file(FILE *file) {
    for (int i = 0; i < originalLineCount; i++) {
        fprintf(file, "%s\n", labeledCode[i].c_str());
    }
}
void label_code() {
    for (int i = lastLineCounter; i < lineCounter; i++) {
        labeledCode[i] = std::to_string(codeIndex) + " " + originalCode[i];
    }
    lastLineCounter = lineCounter;
}

// 添加一条指令
void emit(const fct f, const int l, const int a) {
    if (codeIndex >= MAX_CODE_SIZE) {
        error("Too many instructions");
        return;
    }
    label_code();//给当前行添加标签

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
