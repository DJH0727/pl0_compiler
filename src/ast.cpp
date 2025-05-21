//
// Created by 27249 on 25-5-21.
//

#include "ast.h"

#include <iostream>
#include <sstream>


const char* astNodeTypeToString(ASTNodeType type) {
    switch (type) {
        case AST_PROGRAM: return "PROGRAM";
        case AST_BLOCK: return "BLOCK";
        case AST_CONST_DECL: return "CONST_DECL";
        case AST_CONST_DEF: return "CONST_DEF";
        case AST_IDENTIFIER: return "IDENTIFIER";
        case AST_NUMBER: return "NUMBER";
        case AST_VAR_DECL: return "VAR_DECL";
        case AST_PROC_DECLS: return "PROC_DECLS";
        case AST_PROC_DECL: return "PROC_DECL";
        case AST_ASSIGN_STMT: return "ASSIGN_STMT";
        case AST_CALL_STMT: return "CALL_STMT";
        case AST_BEGIN_STMT: return "BEGIN_STMT";
        case AST_IF_STMT: return "IF_STMT";
        case AST_WHILE_STMT: return "WHILE_STMT";
        case AST_READ_STMT: return "READ_STMT";
        case AST_WRITE_STMT: return "WRITE_STMT";
        case AST_EMPTY_STMT: return "EMPTY_STMT";
        case AST_COND_ODD: return "COND_ODD";
        case AST_COND_OP: return "COND_OP";
        case AST_EXPRESSION: return "EXPRESSION";
        case AST_TERM_OP: return "TERM_OP";
        case AST_NEGATE: return "NEGATE";
        case AST_FACTOR: return "FACTOR";
        case AST_FACTOR_IDENTIFIER: return "FACTOR_IDENTIFIER";
        case AST_FACTOR_NUMBER: return "FACTOR_NUMBER";
        case AST_FACTOR_EXPR: return "FACTOR_EXPR";
        case AST_COND: return "COND";
        case AST_ODD: return "ODD";
        default: return "UNKNOWN";
    }
}

std::string printASTToString(const ASTNode* node, const std::string& prefix = "", bool isLast = true) {
    if (!node) return "";

    std::ostringstream oss;

    // 打印当前节点前缀（父节点的线条）
    oss << prefix;

    // 打印节点连接符号（├── 或 └──）
    if (isLast) {
        oss << "└── ";
    } else {
        oss << "├── ";
    }

    // 打印节点类型和可选值
    oss << astNodeTypeToString(node->type);
    if (!node->value.empty()) {
        oss << " : " << node->value;
    }
    oss << "\n";

    // 计算新的前缀，传给孩子节点
    std::string childPrefix = prefix + (isLast ? "    " : "│   ");

    // 遍历孩子，递归拼接字符串
    for (size_t i = 0; i < node->children.size(); ++i) {
        oss << printASTToString(node->children[i].get(), childPrefix, i == node->children.size() - 1);
    }

    return oss.str();
}
void printASTNode(const ASTNode* node, const std::string& prefix , bool isLast) {
    const std::string str = printASTToString(node);
    std::cout << str;
}
void print_ast_to_file(FILE* ast_file, const ASTNode* node) {
    const std::string str = printASTToString(node);
    fprintf(ast_file, "AST: %s\n", str.c_str());
    fprintf(ast_file, "%s", str.c_str());
}