//
// Created by 27249 on 25-5-18.
//

#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H
#include "token.h"
#define SYMBOL_TABLE_SIZE 100     /* 名字表容量 */
#define MAX_NESTING_LEVEL 3       /* 最大允许过程嵌套声明层数 [0, MAX_NESTING_LEVEL] */
typedef enum {
    OBJ_CONST,
    OBJ_VAR,
    OBJ_PROC
} SymbolKind;


typedef struct {
    char name[MAX_IDENTIFIER_LENGTH + 1];  // 符号名
    SymbolKind kind;           // 符号种类
    int val;                   // 常量值（若是常量）
    int level;                 // 静态作用域层
    int address;               // 地址（变量和过程）
    int size;                  // 过程数据区大小
} Symbol;

extern Symbol symbolTable[SYMBOL_TABLE_SIZE];
extern int symbol_count;


// 初始化符号表
void init_symbol_table();
// 符号表操作
int enter_symbol(SymbolKind kind, const char* name, int val, int level, int address, int size);
// 查找符号，返回索引，找不到返回 -1
int lookup_symbol(const char* name);
//删除符号表中指定层级的符号
void remove_symbols_at_level(int level);
// 获取符号
Symbol* get_symbol(int index);
// 打印符号表
void print_symbol_table();
int count_variables(int level);
#endif //SYMBOL_TABLE_H
