//
// Created by 27249 on 25-5-18.
//

#ifndef COMMON_H
#define COMMON_H
#include <array>
#include <string>

enum ErrorCode {
    NUM_TOO_LONG,
    ILLEGAL_IDENT_AFTER_NUMBER,
    ILLEGAL_COLON,
    ILLEGAL_CHARACTER,
    LEADING_ZERO_NUMBER,
    IDENT_TOO_LONG,
    SYMBOL_TABLE_FULL,
    SYMBOL_REDEFINED,
    ERR_MISSING_PERIOD,
    ERR_EXPECT_IDENTIFIER,
    ERR_EXPECT_EQUAL_SIGN,
    ERR_EXPECT_NUMBER,
    ERR_EXPECT_SEMICOLON,
    ERR_TOO_MANY_NESTED_PROCEDURES,
    ERR_EXPECT_END,
    ERR_EXPECT_THEN,
    ERR_EXPECT_ELSE,
    ERR_EXPECT_DO,
    ERR_EXPECT_BECOMES,
    ERR_EXPECT_REL_OP,
    ERR_UNDECLARED_IDENTIFIER,
    ERR_INVALID_IDENTIFIER_USAGE,
    ERR_EXPECT_RPAREN,
    ERR_INVALID_FACTOR

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
    "因子无效"

}};


void error(ErrorCode err_code,int line,int col);
void error(ErrorCode err_code,const std::string& msg);
void error(const std::string &msg);
std::string get_error_message(ErrorCode err_code);
char* readFile(const std::string &filename);
int strToInt(const std::string &str);


#endif //COMMON_H
