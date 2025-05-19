//
// Created by 27249 on 25-5-18.
//
#include "common.h"
#include <iostream>
void error(const ErrorCode err_code, const int line, const int col) {
    std::cerr<<" ["<<line<<":"<<col <<"] " << "Error: " << err_code<< " " <<get_error_message(err_code) << std::endl;
    exit(1);
}
void error(const ErrorCode err_code,const std::string& msg) {
    std::cerr<<"Error: " << err_code<< " " <<get_error_message(err_code) << " " << msg << std::endl;
    exit(1);
}
void error(const std::string &msg) {
    std::cerr<<"Error: " << msg << std::endl;
    exit(1);
}
std::string get_error_message(const ErrorCode err_code) {
   return error_messages[static_cast<size_t>(err_code)];
}
char* readFile(const std::string &filename) {
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