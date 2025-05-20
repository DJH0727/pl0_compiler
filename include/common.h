//
// Created by 27249 on 25-5-18.
//

#ifndef COMMON_H
#define COMMON_H
#include <array>
#include <string>
#define OUTPUT_STRING "output/"
#define INPUT_STRING "input/"

const std::string OUTPUT_PATH = OUTPUT_STRING;
const std::string INPUT_PATH =  INPUT_STRING;

enum ErrorCode {
    NUM_TOO_LONG,//数字过长
    ILLEGAL_IDENT_AFTER_NUMBER,//数字后面出现非法标识符
    ILLEGAL_COLON,//非法的冒号
    ILLEGAL_CHARACTER,//非法字符
    LEADING_ZERO_NUMBER,//数字前有0
    IDENT_TOO_LONG,//标识符过长
    SYMBOL_TABLE_FULL,//符号表已满
    SYMBOL_REDEFINED,//符号已被重新定义
    ERR_MISSING_PERIOD,//程序必须以句点结束
    ERR_EXPECT_IDENTIFIER,//     期望标识符
    ERR_EXPECT_EQUAL_SIGN,//     期望等号
    ERR_EXPECT_NUMBER,//     期望数字
    ERR_EXPECT_SEMICOLON,//     期望分号
    ERR_TOO_MANY_NESTED_PROCEDURES,//     嵌套过深
    ERR_EXPECT_END,//     期望END
    ERR_EXPECT_THEN,//     期望THEN
    ERR_EXPECT_ELSE,//     期望ELSE
    ERR_EXPECT_DO,//     期望DO
    ERR_EXPECT_BECOMES,//     期望BECOMES
    ERR_EXPECT_REL_OP,//     期望关系运算符
    ERR_UNDECLARED_IDENTIFIER,//     未声明的标识符
    ERR_INVALID_IDENTIFIER_USAGE,//     标识符用法错误
    ERR_EXPECT_RPAREN,//     期望右括号
    ERR_INVALID_FACTOR,//     因子无效
    ERR_PROC_NOT_DEFINED,//     过程未定义
    ERR_NOT_VARIABLE,//     不是变量
    ERR_UNEXPECTED_TOKEN,//     与预期不符
    ERR_EXPECT_LPAREN,//     期望左括号

};
#define ERROR_COUNT 100

constexpr std::array<const char*, static_cast<size_t>(ERROR_COUNT)> error_messages = {{
    "数字过长",
    "数字后面出现非法标识符",
    "非法的冒号",
    "非法字符",
    "数字前有0",
    "标识符过长",
    "符号表已满",
    "符号已被重新定义",
    "程序必须以句点结束",
    "期望标识符",
    "期望等号",
    "期望数字",
    "期望分号",
    "嵌套过深",
    "期望END",
    "期望THEN",
    "期望ELSE",
    "期望DO",
    "期望BECOMES",
    "期望关系运算符",
    "未声明的标识符",
    "标识符用法错误",
    "期望右括号",
    "因子无效",
    "过程未定义",
    "不是变量",
    "与预期不符",
    "期望左括号"

}};


void error(ErrorCode err_code,int line,int col);
void error(ErrorCode err_code,const std::string& msg);
void error(const std::string &msg);
std::string get_error_message(ErrorCode err_code);
char* readFile(const std::string &filename);
int strToInt(const std::string &str);


#endif //COMMON_H
