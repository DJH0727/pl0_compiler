//
// Created by 27249 on 25-5-18.
//
#include "symbol_table.h"

#include <cstring>

#include "common.h"
Symbol symbolTable[SYMBOL_TABLE_SIZE];
int symbol_count;
// 初始化符号表
void init_symbol_table() {
    symbol_count = 0;
}
// 符号表操作
int enter_symbol(const SymbolKind kind, const char* name, const int val, const int level, const int address, const int size) {
    if (symbol_count >= SYMBOL_TABLE_SIZE) {
        error(SYMBOL_TABLE_FULL, std::string(" 无法插入符号: ")+name);
        return -1;
    }
    if (lookup_symbol_current_level(name, level) != -1) {
        error(SYMBOL_REDEFINED, std::string(" 符号 ")+name+std::string(" 已定义"));
        return -1;
    }
    Symbol* sym = &symbolTable[symbol_count];
    strncpy(sym->name, name, MAX_IDENTIFIER_LENGTH);
    sym->name[MAX_IDENTIFIER_LENGTH] = '\0'; // 保证字符串结束符
    sym->kind = kind;
    sym->val = val;
    sym->level = level;
    sym->address = address;
    sym->size = size;
    return symbol_count++;
}
int lookup_symbol_current_level(const char* name, const int current_level) {
    for (int i = symbol_count - 1; i >= 0; i--) {  // 从后向前查找，优先最近声明的
        if (symbolTable[i].level == 0&&strcmp(symbolTable[i].name, name) == 0) {
            return i;
        }
        if (symbolTable[i].level == current_level && strcmp(symbolTable[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}
// 查找符号，返回索引，找不到返回 -1
int lookup_symbol(const char* name) {
    for (int i = symbol_count - 1; i >= 0; i--) {  // 从后向前查找，优先最近声明的
        if (strcmp(symbolTable[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}
//删除符号表中指定层级的符号
void remove_symbols_at_level(const int level) {
    while (symbol_count > 0 && symbolTable[symbol_count - 1].level == level) {
        symbol_count--;
    }
}
// 获取符号
Symbol* get_symbol(const int index) {
    if (index < 0 || index >= symbol_count) {
        return nullptr;
    }
    return &symbolTable[index];
}

// 打印符号表
void print_symbol_table() {
    printf("Symbol Table:\n");
    for (int i = 0; i < symbol_count; i++) {
        Symbol* s = &symbolTable[i];
        printf("[%d] name=%s kind=%d val=%d level=%d addr=%d size=%d\n",
            i, s->name, s->kind, s->val, s->level, s->address, s->size);
    }
}
// 统计指定层级的变量数量
int count_variables(const int level) {
    int count = 0;
    for (int i = 0; i < symbol_count; i++) {
        const Symbol* s = &symbolTable[i];
        if (s->level == level && s->kind == OBJ_VAR) {
            count++;
        }
    }
    return count;
}