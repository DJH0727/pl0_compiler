//
// Created by 27249 on 25-5-18.
//

#ifndef CODEGEN_H
#define CODEGEN_H
#include <string>


enum fct  {
    LIT, // 将常量加载到栈顶
     OPR, // 进行算术或逻辑操作
     LOD, // 加载变量到栈顶
     STO, // 存储栈顶内容到变量
     CAL_, // 调用过程
     INT_, // 分配栈空间
     JMP, // 无条件跳转
     JPC  // 条件跳转
};
std::string getFctName(fct f);  // 指令名称
struct Instruction {
    fct  f;  // 指令操作码
    int l;      // 层次差
    int a;      // 地址或立即数
};
#define FCT_COUNT 8
#define MAX_CODE_SIZE  500  // 指令数组最大长度
// 指令生成接口
void emit(fct f, int l, int a);  // 添加一条指令
void printListCode();                 // 打印所有指令
void print_code_to_file(FILE *file) ;
extern Instruction code[MAX_CODE_SIZE];      // 指令数组
extern int codeIndex;           // 当前指令计数
#endif //CODEGEN_H
