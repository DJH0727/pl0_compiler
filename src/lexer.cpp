//
// Created by 27249 on 25-5-18.
//
#include "lexer.h"
#include <iostream>
#include <symbol_table.h>

#include "global.h"
static const char* source = NULL;  // 源代码字符串
static int pos = 0;         // 当前字符索引
static int line = 1;        // 当前行号
static int column = 1;      // 当前列号
static char ch;             // 当前字符
Token currentToken;

// 初始化词法分析器（传入源代码字符串）
void initLexer(const char* src) {
    source = src;   // 绑定源代码
    pos = 0;        // 初始化索引
    line = 1;       // 第一行
    column = 0;     // 第一列
    getch();        // 读取第一个字符

}
void getch() {
    if (source[pos] == '\0') {
        ch = '\0';  // 到达字符串结尾
        return;
    }

    ch = source[pos++];
    //std::cout<<"currentChar: "<<ch<<std::endl;
    if (ch == '\n') {
        line++;
        column = 0;
    } else {
        column++;
    }
}

void getNextToken() {

    // 跳过空白字符
    while (ch==' ' || ch==10 || ch==13 || ch==9)  /* 忽略空格、换行、回车和TAB */
    {
        getch();
    }
    // 设置 token 的位置
    currentToken.line = line;
    currentToken.column = column;
    // 处理数字
    if (isDigit(ch)) {

        std::string numStr;
        numStr = ch;
        getch();
        while (isDigit(ch)) {
            numStr += ch;
            getch();
        }
        //整数不能以0开头
        if (numStr.length() > 1 && numStr[0] == '0') {
            error(LEADING_ZERO_NUMBER, line, column);
            return;
        }
        // 判断数字长度是否超过最大值
        if (numStr.length() > MAX_NUMBER_LENGTH) {
            error(NUM_TOO_LONG, line, column);
            return;
        }
        //数字后面不能跟标识符，不能出现形如 123abc 的情况
        if (isLetter(ch)) {
            error(ILLEGAL_IDENT_AFTER_NUMBER, line, column);
            return;
        }
        currentToken.type = PL0TokenType::number;
        currentToken.lexeme = numStr;
        return;
    }
    // 处理标识符
    if (isLetter(ch)) {
        std::string idStr;
        idStr = ch;
        getch();
        while (isLetter(ch) || isDigit(ch)) {
            idStr += ch;
            getch();
        }
        // 判断标识符长度是否超过最大值
        if (idStr.length() > MAX_IDENTIFIER_LENGTH) {
            error(IDENT_TOO_LONG, line, column);
            return;
        }
        // 判断关键字
        const int index = isKeywords(idStr);
        if (index!= -1) {
            currentToken.type = keywordTypes[index];
            currentToken.lexeme = idStr;
        } else {
            currentToken.type = PL0TokenType::ident;
            currentToken.lexeme = idStr;
        }
        return;
    }
    // 处理运算符
    if (ch == ':') {
        getch();
        if (ch == '=') {// 赋值运算符
            currentToken.type = PL0TokenType::becomes;
            currentToken.lexeme = ":=";
            getch();
        }
        else {
            error(ILLEGAL_COLON, line, column);

        }
        return;
    }
    if (ch == '<') {
        getch();
        if (ch == '=') { // 小于等于运算符
            currentToken.type = PL0TokenType::leq;
            currentToken.lexeme = "<=";
            getch();
        }
        else if (ch == '>') { // 不等于运算符
            currentToken.type = PL0TokenType::neq;
            currentToken.lexeme = "<>";
            getch();
        }
        else {
            currentToken.type = PL0TokenType::lss;
            currentToken.lexeme = "<";

        }
        return;
    }
    if (ch == '>') {
        getch();
        if (ch == '=') { // 大于等于运算符
            currentToken.type = PL0TokenType::geq;
            currentToken.lexeme = ">=";
            getch();
        }
        else {
            currentToken.type = PL0TokenType::gtr;
            currentToken.lexeme = ">";

        }
        return;
    }
    // 处理单字符符号
    const int index = isSingleCharSymbol(ch);
    if (index != -1) {
        currentToken.type = SingleCharSymbolTypes[index];
        currentToken.lexeme = SingleCharSymbols[index];
        getch();
    }
    else if (ch == 0) {return;}
    else {
        //非法字符，比如中文字符
        error(ILLEGAL_CHARACTER, line, column);
    }





}
bool isDigit(const char c) {
    return (c >= '0' && c <= '9');
}

bool isLetter(const char c) {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}
int isKeywords(const std::string &word) {
    int index = -1;
    for (int i = 0; i < KEYWORD_COUNT; i++) {
        if (word == keywords[i]) {
            index = i;
            break;
        }
    }
    return index;
}
int isSingleCharSymbol(const char c) {
    int index = -1;
    for (int i = 0; i < SINGLE_CHAR_SYMBOL_COUNT; i++) {
        if (c == SingleCharSymbols[i]) {
            index = i;
            break;
        }
    }
    return index;
}