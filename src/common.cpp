//
// Created by 27249 on 25-5-18.
//
#include "common.h"
#include <iostream>
void error(const ErrorCode err_code,int line,int col) {
    std::cerr<<" ["<<line<<":"<<col <<"] " << "Error: " << err_code<< " " <<get_error_message(err_code) << std::endl;
    exit(1);
}
std::string get_error_message(const ErrorCode err_code) {
   switch (err_code) {
       case ErrorCode::NUM_TOO_LONG:return "数字过长";
       case ErrorCode::ILLEGAL_IDENT_AFTER_NUMBER:return "数字后面出现非法标识符";
       case ErrorCode::ILLEGAL_COLON:return "冒号后面出现非法字符";
       case ErrorCode::ILLEGAL_CHARACTER:return "非法字符";
       case ErrorCode::LEADING_ZERO_NUMBER:return "不允许数字以0开头";
       default:return "未知错误";
   }
}
char* readFile(std::string filename) {
    const char* fileName = filename.data();
    FILE* file = fopen(fileName, "rb");
    if (file == nullptr) {
        perror("打开文件失败");
        return nullptr;
    }

    fseek(file, 0, SEEK_END);
    const long size = ftell(file);
    rewind(file);

    const auto buffer = (char*)malloc(size + 1);
    if (buffer == nullptr) {
        perror("内存分配失败");
        fclose(file);
        return nullptr;
    }

    fread(buffer, 1, size, file);
    buffer[size] = '\0'; // 添加字符串结束符
    fclose(file);
    return buffer;
}
int strToInt(const std::string &str) {
    return std::stoi(str);
}