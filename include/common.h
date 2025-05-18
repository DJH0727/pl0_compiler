//
// Created by 27249 on 25-5-18.
//

#ifndef COMMON_H
#define COMMON_H
#include <string>

enum ErrorCode {
    NUM_TOO_LONG = -1,
    ILLEGAL_IDENT_AFTER_NUMBER= -2,
    ILLEGAL_COLON = -3,
    ILLEGAL_CHARACTER = -4,
    LEADING_ZERO_NUMBER = -5,
};

void error(ErrorCode err_code,int line,int col);
std::string get_error_message(ErrorCode err_code);
char* readFile(std::string filename);
int strToInt(const std::string &str);


#endif //COMMON_H
