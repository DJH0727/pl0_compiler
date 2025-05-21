//
// Created by 27249 on 25-5-18.
//
#include "parser.h"

#include <codegen.h>
#include <cstring>
#include <iostream>
#include <sstream>
#include <symbol_table.h>
#include <vector>

#include "lexer.h"

int currentLevel = 0;  // 当前嵌套层级
int varCount[MAX_NESTING_LEVEL] = {0, 0, 0};      // 当前层级变量数量

void match(const PL0TokenType type) {
    if (currentToken.type != type) {
        error(ERR_UNEXPECTED_TOKEN, currentToken.line, currentToken.column);
    }
    getNextToken();
    if (currentToken.line != lineCounter) {
        lineCounter = currentToken.line;
    }
}


std::unique_ptr<ASTNode> parse_program() {
    auto node = std::make_unique<ASTNode>(AST_PROGRAM); // 创建程序节点

    std::unique_ptr<ASTNode> blockNode = parse_block(); // 拿到 block 节点的唯一所有权
    node->addChild(std::move(blockNode));
    if (currentToken.type != period) {
        //程序必须以句点结束
        error(ERR_MISSING_PERIOD, currentToken.line, currentToken.column);
    }
    return node;
}




std::unique_ptr<ASTNode> parse_block() {
    auto node = std::make_unique<ASTNode>(AST_BLOCK); // 创建 Block 节点

    varCount[currentLevel] = 0; // 变量计数归零
    const int index = codeIndex; // 记录当前代码段的返回地址
    emit(JMP, 0, 0); // 进入程序，生成入口代码


    if (currentToken.type == constsym) {
        auto constDecl = parse_const_decl(); // 返回 unique_ptr<ASTNode>
        node->addChild(std::move(constDecl));
    }
    if (currentToken.type == varsym) {
        auto varDecl = parse_var_decl();
        node->addChild(std::move(varDecl));
    }
    while (currentToken.type == procsym) {
        auto procDecl = parse_proc_decl();
        node->addChild(std::move(procDecl));
    }

    code[index].a = codeIndex;
    // 回填代码段大小
    emit(INT_, 0, varCount[currentLevel]+RESERVE_ADDRESS_SIZE);

    auto stmt = parse_stmt(); // 语句部分
    node->addChild(std::move(stmt));
    emit(OPR, 0, a_release);// 释放数据段

    return node;
}





std::unique_ptr<ASTNode> parse_const_decl() {
    auto node = std::make_unique<ASTNode>(AST_CONST_DECL); // ConstDecl 节点
   
    match(constsym);  // 跳过const关键字
   
    while (true) {
        // 标识符检查
        if (currentToken.type != ident) {
            error(ERR_EXPECT_IDENTIFIER, currentToken.line, currentToken.column);
        }

        char constName[MAX_IDENTIFIER_LENGTH + 1];
        // 复制标识符名字，注意防止越界
        strncpy(constName, currentToken.lexeme.c_str(), MAX_IDENTIFIER_LENGTH);
        constName[MAX_IDENTIFIER_LENGTH] = '\0';
        
       
        match(ident);  // 跳过标识符
       

        // 必须是等号
        if (currentToken.type != eql) {
            error(ERR_EXPECT_EQUAL_SIGN, currentToken.line, currentToken.column);
        }

        match(eql);  // 跳过等号

        // 必须是数字
        if (currentToken.type != number) {
            error(ERR_EXPECT_NUMBER, currentToken.line, currentToken.column);
        }

        std::string constValue = currentToken.lexeme;
        const int val = strToInt(constValue);

        match(number);  // 跳过数字

        //  加入符号表（常量）
        enter_symbol(OBJ_CONST, constName, val, currentLevel, 0, 0);

        // 构造 const 定义子节点（形如 ident=value）
        auto constDef = std::make_unique<ASTNode>(AST_CONST_DEF);
        constDef->addChild(std::make_unique<ASTNode>(AST_IDENTIFIER, constName));
        constDef->addChild(std::make_unique<ASTNode>(AST_NUMBER, constValue));
        node->addChild(std::move(constDef));

        //处理多个常量，用逗号分隔
        if (currentToken.type == comma) {
            match(comma);  // 跳过逗号，继续解析下一个常量定义
        } else {
            break;
        }
    }

    // 6. 常量声明必须以分号结束
    if (currentToken.type != semicolon) {
        error(ERR_EXPECT_SEMICOLON, currentToken.line, currentToken.column);
    }

    match(semicolon);  // 跳过分号，结束常量声明部分
    return node;
}





std::unique_ptr<ASTNode> parse_var_decl() {
    auto node = std::make_unique<ASTNode>(AST_VAR_DECL); // VarDecl 节点
    match(varsym); // 消费 'var'

    // 变量地址初始值（局部变量在本层的偏移）
    int address = RESERVE_ADDRESS_SIZE;//0 SL; 1 DL; 2 RA

    while (true) {
        // 标识符检查
        if (currentToken.type != ident) {
            error(ERR_EXPECT_IDENTIFIER, currentToken.line, currentToken.column);
        }
        std::string varName = currentToken.lexeme;

        // 将变量插入符号表，val 和 size 对于变量不需要设置
        enter_symbol(OBJ_VAR, varName.c_str(), 0, currentLevel, address++, 0);

        varCount[currentLevel]++; // 变量计数
        //std::cout << "varCount: " << varCount[currentLevel] << std::endl;

        // 添加 AST 节点
        node->addChild(std::make_unique<ASTNode>(AST_IDENTIFIER, varName));
        match(ident); // 消费 ident

        if (currentToken.type == comma) {
            match(comma); // 消费逗号
        } else {
            break;
        }
    }

    if (currentToken.type != semicolon) {
        error(ERR_EXPECT_SEMICOLON, currentToken.line, currentToken.column);
    }

    match(semicolon); // 消费分号
    return node;
}





std::unique_ptr<ASTNode> parse_proc_decl() {
    auto procDeclsNode = std::make_unique<ASTNode>(AST_PROC_DECLS); // 用一个节点包裹所有过程声明v
    while (currentToken.type == procsym) {
        if (currentLevel >= MAX_NESTING_LEVEL) {
            error(ERR_TOO_MANY_NESTED_PROCEDURES, std::string("最多嵌套层级为")+ std::to_string(MAX_NESTING_LEVEL) );
        }

        match( procsym); // 消费 'procedure'

        if (currentToken.type != ident) {
            error(ERR_EXPECT_IDENTIFIER, currentToken.line, currentToken.column);
        }

        // 保存过程名
        char procName[MAX_IDENTIFIER_LENGTH + 1];
        strcpy(procName, currentToken.lexeme.c_str());

        match( ident); // 消费标识符

        if (currentToken.type != semicolon) {
            error(ERR_EXPECT_SEMICOLON, currentToken.line, currentToken.column);
        }

        match( semicolon) ; // 消费 ';'


        // 插入过程符号（地址、val 先设为 0，稍后生成）
       int procIndex = enter_symbol(OBJ_PROC, procName, 0, currentLevel, 0, 0);
        Symbol* symbol = get_symbol(procIndex);
        symbol->address = codeIndex + 1; // 记录过程起始地址,当前是jmp指令，指向下一条指令int，指向这条也行？

        currentLevel++;                // 进入下一层

        auto blockNode = parse_block(); // 递归解析过程体
        if (!blockNode) {
            return nullptr;
        }
        // 回填该过程所需的局部变量空间大小
        symbol->size = varCount[currentLevel]+RESERVE_ADDRESS_SIZE;// 3 是 SL, DL, RA 的大小
        currentLevel--;                // 返回上一层

        if (currentToken.type != semicolon) {
            error(ERR_EXPECT_SEMICOLON, currentToken.line, currentToken.column);
        }

        match( semicolon); // 消费过程体后的 ';'
        // 构建当前过程 AST 节点
        auto procNode = std::make_unique<ASTNode>(AST_PROC_DECL, procName);
        procNode->addChild(std::move(blockNode)); // 添加过程体作为子节点
        procDeclsNode->addChild(std::move(procNode));
    }
    return procDeclsNode;
}


std::unique_ptr<ASTNode> parse_stmt() {
    switch (currentToken.type) {
        case ident: {
            // 赋值语句: ident := Exp
            // 先记下变量名
            char varName[MAX_IDENTIFIER_LENGTH + 1];
            strcpy(varName, currentToken.lexeme.c_str());

            // 查找变量符号
            const int symIndex = lookup_symbol(varName);
            if (symIndex == -1) {
                error(ERR_UNDECLARED_IDENTIFIER, currentToken.line, currentToken.column);
            }
            const Symbol* sym = get_symbol(symIndex);
            if (sym->kind != OBJ_VAR) {
                error(ERR_NOT_VARIABLE, currentToken.line, currentToken.column);
            }
            const int varLevel = sym->level;
            const int levelDiff = currentLevel - varLevel;
            const int addr = sym->address;

            match( ident); // 消费 ident

            if (currentToken.type != becomes) { // := 符号
                error(ERR_EXPECT_BECOMES, currentToken.line, currentToken.column);
            }

            match(  becomes); // 消费 :=

            auto exprNode = parse_expression();
            if (!exprNode) return nullptr; // 解析表达式

            //生成赋值语句的 p-code：将栈顶内容存到变量地址中
            emit(STO, levelDiff, addr);
            auto assignNode = std::make_unique<ASTNode>(AST_ASSIGN_STMT, varName);
            assignNode->addChild(std::move(exprNode));
            return assignNode;
        }
        case callsym: {
            // 调用语句: call ident
            match(  callsym); // 消费 call

            if (currentToken.type != ident) {
                error(ERR_EXPECT_IDENTIFIER, currentToken.line, currentToken.column);
            }

            // 过程名
            char procName[MAX_IDENTIFIER_LENGTH + 1];
            strcpy(procName, currentToken.lexeme.c_str());
            const int index = lookup_symbol(procName);
            if (index == -1) {
                error(ERR_PROC_NOT_DEFINED, currentToken.line, currentToken.column);
            }

            match( ident); // 消费 ident

            // 生成调用过程的 p-code
            const Symbol* procSymbol = get_symbol(index);
            emit(CAL_, currentLevel - procSymbol->level, procSymbol->address);
            return std::make_unique<ASTNode>(AST_CALL_STMT, procName);
        }
        case beginsym: {
            // begin Stmt {; Stmt} end
            match(  beginsym); // 消费 begin
            auto beginNode = std::make_unique<ASTNode>(AST_BEGIN_STMT);

            auto stmtNode = parse_stmt();
            if (!stmtNode) return nullptr;
            beginNode->addChild(std::move(stmtNode));

            while (currentToken.type == semicolon) {
                match(  semicolon); // 消费分号
                stmtNode = parse_stmt();
                if (!stmtNode) return nullptr;
                beginNode->addChild(std::move(stmtNode));
            }

            if (currentToken.type != endsym) {
                error(ERR_EXPECT_END, currentToken.line, currentToken.column);
            }

            match(  endsym); // 消费 end

            return beginNode;
        }
        case ifsym: {
            // if Cond then Stmt
            match(  ifsym); // 消费 if

            auto condNode = parse_condition();
            if (!condNode) return nullptr;

            if (currentToken.type != thensym) {
                error(ERR_EXPECT_THEN, currentToken.line, currentToken.column);
            }

            match(  thensym); // 消费 then
            // 记录当前 codeIndex，用于回填 JPC 跳转地址
            const int jpcIndex = codeIndex;
            // 生成 JPC 指令，暂时跳转地址为 0，稍后回填
            emit(JPC, 0, 0);
            auto stmtNode = parse_stmt();
            if (!stmtNode) return nullptr;
            // 回填 JPC 跳转地址为当前 codeIndex（语句执行完跳转位置）
            code[jpcIndex].a = codeIndex;
            auto ifNode = std::make_unique<ASTNode>(AST_IF_STMT);
            ifNode->addChild(std::move(condNode));
            ifNode->addChild(std::move(stmtNode));

            return ifNode;
        }
        case whilesym: {
            // while Cond do Stmt
            match(  whilesym); // 消费 while
            const int loopStartIndex = codeIndex;

            auto condNode = parse_condition();
            if (!condNode) return nullptr;

            if (currentToken.type != dosym) {
                error(ERR_EXPECT_DO, currentToken.line, currentToken.column);
            }

            match(  dosym); // 消费 do


            const int jpcIndex = codeIndex;
            emit(JPC, 0, 0);

            auto stmtNode = parse_stmt();
            if (!stmtNode) return nullptr;

            emit(JMP, 0, loopStartIndex);
            code[jpcIndex].a = codeIndex;
            auto whileNode = std::make_unique<ASTNode>(AST_WHILE_STMT);
            whileNode->addChild(std::move(condNode));
            whileNode->addChild(std::move(stmtNode));

            return whileNode;
        }
        case readsym: {
            match(readsym);   // 消费 read

            if (currentToken.type != lparen) {
                error(ERR_EXPECT_LPAREN, currentToken.line, currentToken.column);
            }
            match(lparen);    // 消费 '('
            auto readNode = std::make_unique<ASTNode>(AST_READ_STMT);
            do {
                if (currentToken.type != ident) {
                    error(ERR_EXPECT_IDENTIFIER, currentToken.line, currentToken.column);
                }

                char varName[MAX_IDENTIFIER_LENGTH + 1];
                strcpy(varName, currentToken.lexeme.c_str());

                const int index = lookup_symbol(varName);
                const Symbol* sym = get_symbol(index);

                if (sym == nullptr || sym->kind != OBJ_VAR) {
                    error(ERR_UNDECLARED_IDENTIFIER, currentToken.line, currentToken.column);
                }

                match(ident);  // 消费变量标识符

                emit(RED, 0, 0);         // 读入一个整数，压栈
                emit(STO,  currentLevel - sym->level, sym->address);  // 存到变量

                auto varNode = std::make_unique<ASTNode>(AST_FACTOR_IDENTIFIER , varName);
                readNode->addChild(std::move(varNode));

                if (currentToken.type == comma) {
                    match(comma);  // 消费 ','
                } else {
                    break;
                }
            } while(true);

            if (currentToken.type != rparen) {
                error(ERR_EXPECT_RPAREN, currentToken.line, currentToken.column);
            }
            match(rparen);  // 消费 ')'

            return readNode;
        }
        case writesym: {
            match(writesym);  // 消费 'write'

            if (currentToken.type != lparen) {
                error(ERR_EXPECT_LPAREN, currentToken.line, currentToken.column);
            }
            match(lparen);  // 消费 '('
            auto writeNode = std::make_unique<ASTNode>(AST_WRITE_STMT);
            do {
                auto exprNode = parse_expression();
                if (!exprNode) return nullptr;      // 先计算表达式结果压栈
                emit(WRT, 0, 0);
                writeNode->addChild(std::move(exprNode));

                if (currentToken.type == comma) {
                    match(comma);           // 消费 ','
                } else {
                    break;
                }
            } while (true);

            if (currentToken.type != rparen) {
                error(ERR_EXPECT_RPAREN, currentToken.line, currentToken.column);
            }
            match(rparen);  // 消费 ')'
            return writeNode;
        }
        default:
            // ε 空语句
            return std::make_unique<ASTNode>(AST_EMPTY_STMT);
    }
}
// 解析条件（  Cond  → odd Exp | Exp RelOp Exp ）
std::unique_ptr<ASTNode> parse_condition() {
    if (currentToken.type == oddsym) {
        // odd Exp
        match(  oddsym); // 消费 'odd'
        auto expr = parse_expression(); // 返回表达式 AST    // 解析表达式
        if (!expr) return nullptr;

        emit(OPR, 0, a_odd);
        // 构造 AST 节点：类型可以自定义为 AST_COND_ODD
        auto condNode = std::make_unique<ASTNode>(AST_COND_ODD, "odd");
        condNode->addChild(std::move(expr));
        return condNode;
    } else {
        // Exp RelOp Exp
        auto left = parse_expression(); // 左边表达式
        if (!left) return nullptr;

        // 检查关系运算符
        int relOpCode = -1;
        std::string opStr;
        switch (currentToken.type) {
            case eql:  {relOpCode = a_eql; opStr = "="; break;} // =
            case neq:  {relOpCode = a_neq;  opStr = "<>"; break;} // <>
            case lss:  {relOpCode = a_lss;  opStr = "<";  break; } // <
            case geq:  {relOpCode = a_geq;  opStr = ">="; break;} // >=
            case gtr:  {relOpCode = a_gtr;  opStr = ">";  break; }// >
            case leq:  {relOpCode = a_leq;  opStr = "<="; break;} // <=
            default:
                error(ERR_EXPECT_REL_OP, currentToken.line, currentToken.column);
            return nullptr;
        }

        match(  currentToken.type); // 消费关系运算符

        auto right = parse_expression(); // 右边表达式
        if (!right) return nullptr;

        emit(OPR, 0, relOpCode);
        // 构造 AST 节点
        auto condNode = std::make_unique<ASTNode>(AST_COND_OP, opStr); // value 记录操作符
        condNode->addChild(std::move(left));
        condNode->addChild(std::move(right));
        return condNode;
    }

}


// 解析表达式（  Exp   → [+ | − ] Term {+ Term | − Term} ）
std::unique_ptr<ASTNode>  parse_expression() {
    PL0TokenType op = nul;
    if (currentToken.type == plus || currentToken.type == minus) {
        op = currentToken.type;
        match( op); // 消费 '+' 或 '-'
    }

    // 解析第一个项
    auto left = parse_term();
    if (!left) return nullptr;

    // 如果前面是负号，执行取负操作
    if (op == minus) {
        emit(OPR, 0, a_neg);
        auto negateNode = std::make_unique<ASTNode>(AST_NEGATE, "-");
        negateNode->addChild(std::move(left));
        left = std::move(negateNode);
    }

    // 后续的 + 或 - 连接的 Term
    while (currentToken.type == plus || currentToken.type == minus) {
        op = currentToken.type;
        match( op); // 消费 '+' 或 '-'

        auto right = parse_term();
        if (!right) return nullptr;

        if (op == plus) {
            emit(OPR, 0, a_plus);// 加法
        } else {
            emit(OPR, 0, a_minus);
        }
        // 构建 AST 子树
        std::string opStr = (op == plus ? "+" : "-");
        auto opNode = std::make_unique<ASTNode>(AST_TERM_OP, opStr);
        opNode->addChild(std::move(left));
        opNode->addChild(std::move(right));
        left = std::move(opNode);
    }
    return left;
}


// 解析项（  Term  → Factor {∗ Factor | / Factor} ）
std::unique_ptr<ASTNode> parse_term() {
    auto left = parse_factor();  // 首先解析一个因子
    if (!left) return nullptr;

    while (currentToken.type == times || currentToken.type == slash) {
        const PL0TokenType op = currentToken.type;
        match(  op); // 消费 '*' 或 '/'

        auto right = parse_factor();
        if (!right) return nullptr;

        if (op == times) {
            emit(OPR, 0, a_times);
        } else {
            emit(OPR, 0, a_slash);
        }
        // 构造乘除操作符 AST 节点
        std::string opStr = (op == times ? "*" : "/");
        auto opNode = std::make_unique<ASTNode>(AST_TERM_OP, opStr);
        opNode->addChild(std::move(left));
        opNode->addChild(std::move(right));
        left = std::move(opNode); // 更新为新子树
    }
    return left;
}


// 解析因子（  Factor  → ident | number | ( Exp ) ）
std::unique_ptr<ASTNode> parse_factor() {
    if (currentToken.type == ident) {
        const int index = lookup_symbol(currentToken.lexeme.c_str());
        if (index == -1) {
            error(ERR_UNDECLARED_IDENTIFIER, currentToken.line, currentToken.column);
        }

        const Symbol* sym = get_symbol(index);
        if (sym->kind == OBJ_CONST) {
            emit(LIT, 0, sym->val);
        } else if (sym->kind == OBJ_VAR) {
            emit(LOD, currentLevel - sym->level, sym->address);

        } else {
            error(ERR_INVALID_IDENTIFIER_USAGE, currentToken.line, currentToken.column);
        }

        std::string name = currentToken.lexeme;
        match(  ident); // 消费标识符
        return std::make_unique<ASTNode>(AST_FACTOR_IDENTIFIER, name);
    }
    else if (currentToken.type == number) {

        emit(LIT, 0, strToInt(currentToken.lexeme));
        std::string valStr = currentToken.lexeme;
        match(  number); // 消费 number
        return std::make_unique<ASTNode>(AST_FACTOR_NUMBER, valStr);
    }
    else if (currentToken.type == lparen) {
        match(  lparen); // 消费 '('
        auto exprNode = parse_expression(); // 返回表达式 AST
        if (!exprNode) return nullptr;
        if (currentToken.type != rparen) {
            error(ERR_EXPECT_RPAREN, currentToken.line, currentToken.column);
        }
        match(  rparen); // 消费 ')'
        auto factorNode = std::make_unique<ASTNode>(AST_FACTOR_EXPR);
        factorNode->addChild(std::move(exprNode));
        return factorNode;
    }
    else {
        error(ERR_INVALID_FACTOR, currentToken.line, currentToken.column);
        return nullptr;
    }
}


