//
// Created by 27249 on 25-5-18.
//
#include <codegen.h>
#include <windows.h>
#include <iostream>
#include <parser.h>
#include <symbol_table.h>
#include <vm.h>
//#define DEBUG 1
#define PRINT2FILE 1
// 路径
#define OUTPUT_STRING "output/"
#define INPUT_STRING "input/"

const std::string OUTPUT_PATH = OUTPUT_STRING;
const std::string INPUT_PATH =  INPUT_STRING;
#include "token.h"
#include "lexer.h"


//
// 解决控制台输出乱码问题,也可以将文件编码改为GBK
//
struct ConsoleEncodingInitializer {
    ConsoleEncodingInitializer() {
        SetConsoleOutputCP(65001);
        SetConsoleCP(65001);
    }
};
static ConsoleEncodingInitializer consoleEncodingInitializer;

int main() {
    char* code = readFile(INPUT_PATH+"input.txt");
    if (code == nullptr) {
        std::cout << "Failed to read input file." << std::endl;
        return 1;
    }
    initLexer(code);
    getNextToken();
    init_code(code);
    parse_program();
    execute_program();
    //print_symbol_table();
    //printListCode();
    //print_label_code();
#ifdef PRINT2FILE
    FILE* symbol_table_file = fopen((OUTPUT_PATH+"symbol_table.txt").c_str(),"w");
    FILE* code_file = fopen((OUTPUT_PATH+"code.txt").c_str(),"w");
    FILE* label_file = fopen((OUTPUT_PATH+"label_code.txt").c_str(),"w");
    print_symbol_table_to_file(symbol_table_file);
    print_code_to_file(code_file);
    print_label_code_to_file(label_file);
    printf("Output files generated in %s.\n%s \n%s \n%s \n",
        OUTPUT_PATH.c_str(),
        ("symbol_table.txt"),
        ("code.txt"),
        ("label_code.txt"));
    fclose(symbol_table_file);
#endif



    free(code);
    return 0;
}
