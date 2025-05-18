//
// Created by 27249 on 25-5-18.
//

#ifndef LEXER_H
#define LEXER_H
#include <token.h>

extern Token currentToken;
// 初始化词法分析器（传入源代码字符串）
void initLexer(const char* src);

// 读取下一个符号（会更新 sym、id、num）
void getNextToken();
// 读取下一个字符
void getch();
bool isDigit(char c);
bool isLetter( char c);\
int isKeywords(const std::string &word);
int isSingleCharSymbol(char c);//判断是否是单字符符号
#endif //LEXER_H
