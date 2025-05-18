//
// Created by 27249 on 25-5-18.
//

#ifndef GLOBAL_H
#define GLOBAL_H
// 调试开关
#define DEBUG_MODE 1


// 路径
#define OUTPUT_STRING "output/"
#define INPUT_STRING "input/"

const std::string OUTPUT_PATH = OUTPUT_STRING;
const std::string INPUT_PATH =  INPUT_STRING;



#define SYMBOL_TABLE_SIZE 100     /* 名字表容量 */
#define MAX_ADDRESS 2047          /* 地址上界 */
#define MAX_NESTING_LEVEL 3       /* 最大允许过程嵌套声明层数 [0, MAX_NESTING_LEVEL] */
#define MAX_CODE_SIZE 500         /* 最多的虚拟机代码数 */







#endif //GLOBAL_H
