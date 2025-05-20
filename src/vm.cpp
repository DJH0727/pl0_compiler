//
// Created by 27249 on 25-5-18.
//
#include "vm.h"

#include <iostream>
#include <limits>
#include <ostream>

#include "common.h"
#include "codegen.h"


void execute_program() {
    std::cout<<"Start of executing program"<< std::endl;
    FILE *read_write_file = fopen((OUTPUT_PATH+"read_write.txt").c_str(), "w+");
    Instruction ir{};        // 当前指令寄存器
    int pc = 0;            // 程序计数器
    int bp = 0;            // 基地址指针
    int sp = -1;           // 栈顶指针
    int stack[ STACK_SIZE ] = {0}; // 数据栈

    pc =0; bp = 0; sp = -1;
    stack[0]=0;stack[1]=0;stack[2]=0;
    do {
        ir = code[pc];
        pc++;
       switch (ir.f) {
           case LIT:
               stack[++sp] = ir.a;
               break;
           case OPR:
               switch (ir.a) {
                   case a_release:
                       sp = bp - 1;
                       pc = stack[sp + 3];
                       bp = stack[sp + 2];
                       break;
                   case a_neg:
                       stack[sp] = -stack[sp];
                       break;
                   case a_plus:
                       sp--;
                       stack[sp] = stack[sp] + stack[sp + 1];
                       break;
                   case a_minus:
                       sp--;
                       stack[sp] = stack[sp] - stack[sp + 1];
                       break;
                   case a_times:
                       sp--;
                       stack[sp] = stack[sp] * stack[sp + 1];
                       break;
                   case a_slash:
                       sp--;
                       stack[sp] = stack[sp] / stack[sp + 1];
                       break;
                   case a_odd:
                       stack[sp] = stack[sp] % 2;
                       break;
                   case a_eql:
                       sp--;
                       stack[sp] = (stack[sp] == stack[sp + 1]);
                       break;
                   case a_neq:
                       sp--;
                       stack[sp] = (stack[sp]!= stack[sp + 1]);
                       break;
                   case a_lss:
                       sp--;
                       stack[sp] = (stack[sp] < stack[sp + 1]);
                       break;
                   case a_geq:
                       sp--;
                       stack[sp] = (stack[sp] >= stack[sp + 1]);
                       break;
                   case a_gtr:
                       sp--;
                       stack[sp] = (stack[sp] > stack[sp + 1]);
                       break;
                   case a_leq:
                       sp--;
                       stack[sp] = (stack[sp] <= stack[sp + 1]);
                       break;
                   default: error("Illegal operator");
               }
               break;
           case LOD:
               stack[++sp] = stack[base(ir.l,bp,stack) + ir.a];
               break;
           case STO:
               stack[base(ir.l,bp,stack) + ir.a] = stack[sp--];
               break;
           case CAL_: {
               // 静态链、动态链、返回地址
               /* 将父过程基地址入栈 */
               stack[sp + 1] = base(ir.l, bp, stack); // 静态链
               /* 将本过程基地址入栈，此两项用于base函数 */
               stack[sp + 2] = bp;                    // 动态链
               /* 将当前指令指针入栈 */
               stack[sp + 3] = pc;                    // 返回地址
               /* 改变基地址指针值为新过程的基地址 */
               bp = sp + 1;
               /* 跳转到新过程 */
               pc = ir.a;
               break;
           }
           case INT_:
               sp += ir.a;
               break;
           case JMP:
               pc = ir.a;
               break;
           case JPC:
               if (stack[sp--]==0) {
                   pc = ir.a;
               }
               break;
           case RED: {
               std::cout << "read>>  ";
               int num;
               if (!(std::cin >> num)) {
                   error("Invalid input. Please enter an integer.");
               }
               stack[++sp] = num;  // 把输入值压栈
               fprintf(read_write_file, "read>>   %d\n", num);
               break;
           }
           case WRT: {
               const int value = stack[sp--];  // 弹出要输出的值
               std::cout <<"write<< "<< value<<std::endl;
               fprintf(read_write_file, "write<<  %d\n", value);
               break;
           }
           default: error("Illegal instruction");
       }

        //print_stack(stack);
    }while (pc != 0);

    fclose(read_write_file);
    std::cout<<"End of executing program"<< std::endl;
    std::cout<<"I/O is in output/read_write.txt"<< std::endl;
}

// 静态链查找
int base(int l, const int b, const int stack[]) {
    int b1 = b;
    while (l > 0) {
        b1 = stack[b1]; // 向上找静态链
        l--;
    }
    return b1;
}
void print_stack(int stack[]) {
    for (int i = 0; i < STACK_SIZE; i++) {
        std::cout <<"["<<i<<"]" << stack[i] << " ";
    }
    std::cout << std::endl;
}