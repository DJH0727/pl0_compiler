//
// Created by 27249 on 25-5-18.
//

#ifndef TOKEN_H
#define TOKEN_H
#include <string>

#include "common.h"
// 符号类型数量
#define SYMBOL_TYPE_COUNT 32
#define MAX_NUMBER_LENGTH 14      /* number的最大位数 */
#define MAX_IDENTIFIER_LENGTH 10  /* 符号的最大长度 */
/* 符号 */
enum PL0TokenType {
    nul,         // 空符号（占位符或错误）
    ident,       // 标识符（变量名、过程名等）
    number,      // 无符号整数

    // 运算符
    plus,        // 加号：+
    minus,       // 减号：-
    times,       // 乘号：*
    slash,       // 除号：/
    oddsym,      // 关键字 odd：判断奇偶（如 if odd x then）

    // 比较运算符
    eql,         // 等于：=
    neq,         // 不等于：#（PL/0中用#表示!=）
    lss,         // 小于：<
    leq,         // 小于等于：<=
    gtr,         // 大于：>
    geq,         // 大于等于：>=

    // 界符
    lparen,      // 左括号：(
    rparen,      // 右括号：)
    comma,       // 逗号：,
    semicolon,   // 分号：;
    period,      // 句点：.（程序结束）
    becomes,     // 赋值符号：:=

    // 关键字
    beginsym,    // begin（语句块起始）
    endsym,      // end（语句块结束）
    ifsym,       // if（条件语句）
    thensym,     // then（条件成立执行）
    whilesym,    // while（循环条件）
    writesym,    // write（输出语句）
    readsym,     // read（输入语句）
    dosym,       // do（循环体开始）
    callsym,     // call（过程调用）
    constsym,    // const（常量声明）
    varsym,      // var（变量声明）
    procsym      // procedure（过程声明）
};
inline std::string tokenTypeToString(const PL0TokenType t) {
    switch (t) {
        case nul: return "nul";
        case ident: return "ident";
        case number: return "number";
        case plus: return "plus";
        case minus: return "minus";
        case times: return "times";
        case slash: return "slash";
        case oddsym: return "oddsym";
        case eql: return "eql";
        case neq: return "neq";
        case lss: return "lss";
        case leq: return "leq";
        case gtr: return "gtr";
        case geq: return "geq";
        case lparen: return "lparen";
        case rparen: return "rparen";
        case comma: return "comma";
        case semicolon: return "semicolon";
        case period: return "period";
        case becomes: return "becomes";
        case beginsym: return "beginsym";
        case endsym: return "endsym";
        case ifsym: return "ifsym";
        case thensym: return "thensym";
        case whilesym: return "whilesym";
        case writesym: return "writesym";
        case readsym: return "readsym";
        case dosym: return "dosym";
        case callsym: return "callsym";
        case constsym: return "constsym";
        case varsym: return "varsym";
        case procsym: return "procsym";
        default: return "unknown";
    }
}

//保留字
const std::string keywords[] = {
    "begin", "call", "const", "do",
    "end", "if", "odd", "procedure",
    "read", "then", "var", "while",
    "write"
};
#define KEYWORD_COUNT 13          /* 关键字个数 */
constexpr PL0TokenType keywordTypes[] = {
    beginsym, callsym, constsym, dosym,
    endsym, ifsym, oddsym, procsym,
    readsym, thensym, varsym, whilesym,
    writesym
};
constexpr char SingleCharSymbols[] {
    '+', '-', '*', '/',
    '=', '#' , '(', ')',
    ',', ';', '.'
};
#define SINGLE_CHAR_SYMBOL_COUNT 11  /* 单字符符号个数 (除去<和>) */
constexpr PL0TokenType SingleCharSymbolTypes[] {
    plus, minus, times, slash,
    eql, neq, lparen, rparen,
    comma, semicolon, period
};

class Token {
public:
    PL0TokenType type;       // 词法单元的类型，比如标识符、关键字、加号等
    std::string lexeme;   // 原始文本，例如变量名 abc、关键字 begin、数字 123
    int line;             // 出现在源代码的第几行
    int column;           // 出现在该行的第几个字符位置
    // 构造函数
    Token() : type(PL0TokenType::nul), lexeme(""), line(0), column(0) {}
    Token(const PL0TokenType type, std::string& lexeme, const int line, const int column)
        : type(type), lexeme(std::move(lexeme)), line(line), column(column) {}
    std::string toString() const {
        return "Token{" + std::to_string(line) + "," + std::to_string(column) + "}: " + lexeme + " (" + tokenTypeToString(type) + ")";
    }
};


#endif //TOKEN_H
