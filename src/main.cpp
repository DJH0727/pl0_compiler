//
// Created by 27249 on 25-5-18.
//
#include <codegen.h>
#include <windows.h>
#include <iostream>
#include <parser.h>
#include <symbol_table.h>
#include <vm.h>
// 路径

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
    const std::unique_ptr<ASTNode> root = parse_program();
    execute_program();
//#define PRINT2CONSOLE 1
#ifdef PRINT2CONSOLE
    print_symbol_table();
    printListCode();
    print_label_code();
    printASTNode(root.get());
#endif

#define PRINT2FILE 1
#ifdef PRINT2FILE
    FILE* symbol_table_file = fopen((OUTPUT_PATH+"symbol_table.txt").c_str(),"w");
    FILE* code_file = fopen((OUTPUT_PATH+"code.txt").c_str(),"w");
    FILE* label_file = fopen((OUTPUT_PATH+"label_code.txt").c_str(),"w");
    FILE* ast_file = fopen((OUTPUT_PATH+"ast.txt").c_str(),"w");
    print_symbol_table_to_file(symbol_table_file);
    print_code_to_file(code_file);
    print_label_code_to_file(label_file);
    print_ast_to_file(ast_file, root.get());
    printf("Output files generated in %s.\n%s \n%s \n%s \n%s \n",
        OUTPUT_PATH.c_str(),
        ("symbol_table.txt"),
        ("code.txt"),
        ("label_code.txt"),
        ("ast.txt"));
    fclose(symbol_table_file);
    fclose(code_file);
    fclose(label_file);
    fclose(ast_file);
#endif



    free(code);
    return 0;
}
