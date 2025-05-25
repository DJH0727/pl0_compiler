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
     JPC,  // 条件跳转
     RED, // read
     WRT, // write
    NOP,//空指令
};
std::string getFctName(fct f);  // 指令名称
struct Instruction {
    fct  f;  // 指令操作码
    int l;      // 层次差
    int a;      // 地址或立即数
};
enum a_type {
    a_release = 0,    //释放数据段
    a_neg = 1,           //取负
    a_plus = 2,    //加
    a_minus = 3,   //减
    a_times = 4,   //乘
    a_slash = 5,  //除
    a_odd = 6,        //奇数
    a_eql = 8,         //=
    a_neq = 9,         //<>
    a_lss = 10,        //<
    a_geq = 11,          //>=
    a_gtr = 12,         //>
    a_leq = 13,         //<=
};
#define FCT_COUNT 8
#define MAX_CODE_SIZE  500  // 指令数组最大长度
void init_code(const char* original_code);  // 初始化指令数组
// 指令生成接口
void emit(fct f, int l, int a);  // 添加一条指令
void printListCode();                 // 打印所有指令
void print_code_to_file(FILE *file) ;
void print_label_code();//打印带标签的原始代码
void print_label_code_to_file(FILE *file);
extern Instruction code[2 * MAX_CODE_SIZE];      // 指令数组
extern int codeIndex;           // 当前指令计数
extern int lineCounter;         // 当前行号
#endif //CODEGEN_H
