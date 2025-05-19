//
// Created by 27249 on 25-5-18.
//

#ifndef PARSER_H
#define PARSER_H

// 语法分析入口函数，开始解析整个程序（Program → Block .）
void parse_program();

// 解析Block语法块，  Block  → [ConstDecl] [VarDecl][ProcDecl] Stmt
void parse_block();

// 解析常量声明：const ConstDef {, ConstDef} ;
void parse_const_decl();

// 解析变量声明：var ident {, ident} ;
void parse_var_decl();

// 解析过程声明：procedure ident ; Block ; {procedure ident ; Block ;}
void parse_proc_decl();

// 解析语句（  Stmt   → ident := Exp | call ident | begin Stmt {; Stmt} end | if Cond then Stmt | while Cond do Stmt | ε ）
void parse_stmt();

// 解析条件（  Cond  → odd Exp | Exp RelOp Exp ）
void parse_condition();

// 解析表达式（  Exp   → [+ | − ] Term {+ Term | − Term} ）
void parse_expression();

// 解析项（  Term  → Factor {∗ Factor | / Factor} ）
void parse_term();

// 解析因子（  Factor  → ident | number | ( Exp ) ）
void parse_factor();






#endif //PARSER_H
