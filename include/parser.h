//
// Created by 27249 on 25-5-18.
//

#ifndef PARSER_H
#define PARSER_H
#include <ast.h>
#include <string>
#include <token.h>
//保留栈的大小
#define RESERVE_ADDRESS_SIZE 3 // 保留地址大小为3,SL,DL,RA
void match(PL0TokenType type);// 匹配token的类型


// 语法分析入口函数，开始解析整个程序（Program → Block .）
std::unique_ptr<ASTNode> parse_program();

// 解析Block语法块，  Block  → [ConstDecl] [VarDecl][ProcDecl] Stmt
std::unique_ptr<ASTNode> parse_block();

// 解析常量声明：const ConstDef {, ConstDef} ;
std::unique_ptr<ASTNode> parse_const_decl();

// 解析变量声明：var ident {, ident} ;
std::unique_ptr<ASTNode> parse_var_decl();

// 解析过程声明：procedure ident ; Block ; {procedure ident ; Block ;}
std::unique_ptr<ASTNode> parse_proc_decl();

// 解析语句（  Stmt   → ident := Exp | call ident | begin Stmt {; Stmt} end | if Cond then Stmt | while Cond do Stmt | ε ）
std::unique_ptr<ASTNode> parse_stmt();

// 解析条件（  Cond  → odd Exp | Exp RelOp Exp ）
std::unique_ptr<ASTNode> parse_condition();

// 解析表达式（  Exp   → [+ | − ] Term {+ Term | − Term} ）
std::unique_ptr<ASTNode> parse_expression();

// 解析项（  Term  → Factor {∗ Factor | / Factor} ）
std::unique_ptr<ASTNode> parse_term();

// 解析因子（  Factor  → ident | number | ( Exp ) ）
std::unique_ptr<ASTNode> parse_factor();






#endif //PARSER_H
