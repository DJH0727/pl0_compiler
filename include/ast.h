//
// Created by 27249 on 25-5-20.
//

#ifndef AST_H
#define AST_H
#include <utility>
#include <vector>
#include <string>
#include <memory>

enum ASTNodeType {
    AST_PROGRAM,
    AST_BLOCK,
    AST_CONST_DECL,
    AST_CONST_DEF,
    AST_IDENTIFIER,
    AST_NUMBER,
    AST_VAR_DECL,
    AST_PROC_DECLS,
    AST_PROC_DECL,
    AST_ASSIGN_STMT,
    AST_CALL_STMT,
    AST_BEGIN_STMT,
    AST_IF_STMT,
    AST_WHILE_STMT,
    AST_READ_STMT,
    AST_WRITE_STMT,
    AST_EMPTY_STMT,
    AST_COND_ODD,
    AST_COND_OP,
    AST_EXPRESSION,    // 表达式根
    AST_TERM_OP,       // 加法或减法操作 (+ 或 -)
    AST_NEGATE ,        // 一元负号
    AST_FACTOR,       // 可以选择是否显式表示 factor 节点（可选）
    AST_FACTOR_IDENTIFIER,
    AST_FACTOR_NUMBER,
    AST_FACTOR_EXPR ,// 括号中的表达式
    AST_COND,
    AST_ODD


};

struct ASTNode {
    ASTNodeType type;
    std::string value;
    std::vector<std::unique_ptr<ASTNode>> children;
    explicit ASTNode(const ASTNodeType t, std::string  val = "") : type(t), value(std::move(val)) {}
    void addChild(std::unique_ptr<ASTNode> child) {
        if (child) children.push_back(std::move(child));
    }
};

const char* astNodeTypeToString(ASTNodeType type);

// 打印函数
void printASTNode(const ASTNode* node, const std::string& prefix = "", bool isLast = true);
void print_ast_to_file(FILE* ast_file, const ASTNode* node);
#endif //AST_H
