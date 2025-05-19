//
// Created by 27249 on 25-5-18.
//
#include <codegen.h>
#include <windows.h>
#include <iostream>
#include <parser.h>
#include <symbol_table.h>
//#define DEBUG 1
#define PRINT2FILE 1
#include "global.h"
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
#ifdef DEBUG
    while (true) {
        getNextToken();
        std::cout<<currentToken.toString()<< std::endl;
        if (currentToken.type == PL0TokenType::period)break;
    }
#endif


   getNextToken();
    parse_program();
    print_symbol_table();
    printListCode();
#ifdef PRINT2FILE
    FILE* symbol_table_file = fopen((OUTPUT_PATH+"symbol_table.txt").c_str(),"w");
    FILE* code_file = fopen((OUTPUT_PATH+"code.txt").c_str(),"w");
    print_symbol_table_to_file(symbol_table_file);
    print_code_to_file(code_file);
    fclose(symbol_table_file);
#endif



    free(code);
    return 0;
}
