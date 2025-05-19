//
// Created by 27249 on 25-5-18.
//

#include <windows.h>
#include <iostream>
#include <parser.h>
#include <symbol_table.h>

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
    getNextToken();
    parse_program();
    print_symbol_table();


    free(code);
    return 0;
}
