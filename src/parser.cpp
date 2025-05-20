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
bool isNewProc = false;  // 是否是新过程




void match(const PL0TokenType type) {
    if (currentToken.type != type) {
        error(ERR_UNEXPECTED_TOKEN, currentToken.line, currentToken.column);
    }

    getNextToken();
    if (currentToken.line != lineCounter) {
        lineCounter = currentToken.line;
    }
}


void parse_program() {
    parse_block();


    if (currentToken.type != period) {
        //程序必须以句点结束
        error(ERR_MISSING_PERIOD, currentToken.line, currentToken.column);
    }
}




void parse_block() {

    varCount[currentLevel] = 0; // 变量计数归零
    const int index = codeIndex; // 记录当前代码段的返回地址
    emit(JMP, 0, 0); // 进入程序，生成入口代码


    if (currentToken.type == constsym) {
        parse_const_decl();
    }
    if (currentToken.type == varsym) {
        parse_var_decl();
    }
    while (currentToken.type == procsym) {
        parse_proc_decl();
    }

    code[index].a = codeIndex;
    // 回填代码段大小
    emit(INT_, 0, varCount[currentLevel]+3);

    parse_stmt();
    emit(OPR, 0, 0);// 释放数据段


}





void parse_const_decl() {
    
   
    match(constsym);  // 跳过const关键字
   
    while (true) {
        // 标识符检查
        if (currentToken.type != ident) {
            error(ERR_EXPECT_IDENTIFIER, currentToken.line, currentToken.column);
            return;
        }

        char constName[MAX_IDENTIFIER_LENGTH + 1];
        // 复制标识符名字，注意防止越界
        strncpy(constName, currentToken.lexeme.c_str(), MAX_IDENTIFIER_LENGTH);
        constName[MAX_IDENTIFIER_LENGTH] = '\0';
        
       
        match(ident);  // 跳过标识符
       

        // 必须是等号
        if (currentToken.type != eql) {
            error(ERR_EXPECT_EQUAL_SIGN, currentToken.line, currentToken.column);
            return;
        }

        match(eql);  // 跳过等号

        // 必须是数字
        if (currentToken.type != number) {
            error(ERR_EXPECT_NUMBER, currentToken.line, currentToken.column);
            return;
        }

        const int val = strToInt(currentToken.lexeme);

        match(number);  // 跳过数字

        //  加入符号表（常量）
        enter_symbol(OBJ_CONST, constName, val, currentLevel, 0, 0);

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
        return;
    }

    match(semicolon);  // 跳过分号，结束常量声明部分
    
}





void parse_var_decl() {
    if (currentToken.type != varsym) {
        return; // 不是 var 开头，说明没有变量声明
    }

    match(varsym); // 消费 'var'

    // 变量地址初始值（局部变量在本层的偏移）
    int address = 3;//0 SL; 1 DL; 2 RA

    while (true) {
        // 标识符检查
        if (currentToken.type != ident) {
            error(ERR_EXPECT_IDENTIFIER, currentToken.line, currentToken.column);
            return;
        }
        // 将变量插入符号表，val 和 size 对于变量不需要设置
        enter_symbol(OBJ_VAR, currentToken.lexeme.c_str(), 0, currentLevel, address++, 0);

        varCount[currentLevel]++; // 变量计数
        //std::cout << "varCount: " << varCount[currentLevel] << std::endl;

        match(ident); // 消费 ident

        if (currentToken.type == comma) {
            match(comma); // 消费逗号
        } else {
            break;
        }
    }

    if (currentToken.type != semicolon) {
        error(ERR_EXPECT_SEMICOLON, currentToken.line, currentToken.column);
        return;
    }

    match(semicolon); // 消费分号
    //print_symbol_table(); // 打印符号表
}





void parse_proc_decl() {
    while (currentToken.type == procsym) {
        if (currentLevel >= MAX_NESTING_LEVEL) {
            error(ERR_TOO_MANY_NESTED_PROCEDURES, std::string("最多嵌套层级为")+ std::to_string(MAX_NESTING_LEVEL) );
            return;
        }

        match( procsym); // 消费 'procedure'

        if (currentToken.type != ident) {
            error(ERR_EXPECT_IDENTIFIER, currentToken.line, currentToken.column);
            return;
        }

        // 保存过程名
        char procName[MAX_IDENTIFIER_LENGTH + 1];
        strcpy(procName, currentToken.lexeme.c_str());

        match( ident); // 消费标识符

        if (currentToken.type != semicolon) {
            error(ERR_EXPECT_SEMICOLON, currentToken.line, currentToken.column);
            return;
        }

        match( semicolon) ; // 消费 ';'


        // 插入过程符号（地址、val 先设为 0，稍后生成）
       int procIndex = enter_symbol(OBJ_PROC, procName, 0, currentLevel, 0, 0);
        Symbol* symbol = get_symbol(procIndex);
        symbol->address = codeIndex + 1; // 记录过程起始地址,当前是jmp指令，指向下一条指令int，指向这条也行？

        currentLevel++;                // 进入下一层

        parse_block();                 // 递归处理过程体 block


        // 回填该过程所需的局部变量空间大小
        symbol->size = varCount[currentLevel]+3;// 3 是 SL, DL, RA 的大小
        currentLevel--;                // 返回上一层

        if (currentToken.type != semicolon) {
            error(ERR_EXPECT_SEMICOLON, currentToken.line, currentToken.column);
            return;
        }

        match( semicolon); // 消费过程体后的 ';'
    }
}


void parse_stmt() {
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
                return;
            }
            const Symbol* sym = get_symbol(symIndex);
            if (sym->kind != OBJ_VAR) {
                error(ERR_NOT_VARIABLE, currentToken.line, currentToken.column);
                return;
            }
            const int varLevel = sym->level;
            const int levelDiff = currentLevel - varLevel;
            const int addr = sym->address;

            match( ident); // 消费 ident

            if (currentToken.type != becomes) { // := 符号
                error(ERR_EXPECT_BECOMES, currentToken.line, currentToken.column);
                return;
            }

            match(  becomes); // 消费 :=

            parse_expression(); // 解析表达式

            //生成赋值语句的 p-code：将栈顶内容存到变量地址中
            emit(STO, levelDiff, addr);
            break;
        }
        case callsym: {
            // 调用语句: call ident
            match(  callsym); // 消费 call

            if (currentToken.type != ident) {
                error(ERR_EXPECT_IDENTIFIER, currentToken.line, currentToken.column);
                return;
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
            break;
        }
        case beginsym: {
            // begin Stmt {; Stmt} end
            match(  beginsym); // 消费 begin

            parse_stmt();

            while (currentToken.type == semicolon) {
                match(  semicolon); // 消费分号
                parse_stmt();
            }

            if (currentToken.type != endsym) {
                error(ERR_EXPECT_END, currentToken.line, currentToken.column);
                return;
            }

            match(  endsym); // 消费 end

            break;
        }
        case ifsym: {
            // if Cond then Stmt
            match(  ifsym); // 消费 if

             parse_condition();

            if (currentToken.type != thensym) {
                error(ERR_EXPECT_THEN, currentToken.line, currentToken.column);
                return;
            }

            match(  thensym); // 消费 then
            // 记录当前 codeIndex，用于回填 JPC 跳转地址
            const int jpcIndex = codeIndex;
            // 生成 JPC 指令，暂时跳转地址为 0，稍后回填
            emit(JPC, 0, 0);
            parse_stmt();
            // 回填 JPC 跳转地址为当前 codeIndex（语句执行完跳转位置）
            code[jpcIndex].a = codeIndex;
            break;
        }
        case whilesym: {
            // while Cond do Stmt
            match(  whilesym); // 消费 while
            const int loopStartIndex = codeIndex;
            parse_condition();

            if (currentToken.type != dosym) {
                error(ERR_EXPECT_DO, currentToken.line, currentToken.column);
                return;
            }

            match(  dosym); // 消费 do


            const int jpcIndex = codeIndex;
            emit(JPC, 0, 0);

            parse_stmt();

            emit(JMP, 0, loopStartIndex);
            code[jpcIndex].a = codeIndex;
            break;
        }
        default:
            // ε 空语句
            break;
    }
}
// 解析条件（  Cond  → odd Exp | Exp RelOp Exp ）
void parse_condition() {
    if (currentToken.type == oddsym) {
        // odd Exp
        match(  oddsym); // 消费 'odd'
        parse_expression();    // 解析表达式

        emit(OPR, 0, 6);
    } else {
        // Exp RelOp Exp
        parse_expression(); // 左边表达式

        // 检查关系运算符
        int relOpCode = -1;
        switch (currentToken.type) {
            case eql:  relOpCode = 8; break; // =
            case neq:  relOpCode = 9; break; // <>
            case lss:  relOpCode = 10; break; // <
            case geq:  relOpCode = 11; break; // >=
            case gtr:  relOpCode = 12; break; // >
            case leq:  relOpCode = 13; break; // <=
            default:
                error(ERR_EXPECT_REL_OP, currentToken.line, currentToken.column);
            return;
        }

        match(  currentToken.type); // 消费关系运算符

        parse_expression(); // 右边表达式

        emit(OPR, 0, relOpCode);
    }
}


// 解析表达式（  Exp   → [+ | − ] Term {+ Term | − Term} ）
void parse_expression() {
    PL0TokenType op = nul;
    if (currentToken.type == plus || currentToken.type == minus) {
        op = currentToken.type;
        match( op); // 消费 '+' 或 '-'
    }

    parse_term(); // 先解析一个项（Term）

    // 如果前面是负号，执行取负操作
    if (op == minus) {
        emit(OPR, 0, 1);
    }

    // 后续的 + 或 - 连接的 Term
    while (currentToken.type == plus || currentToken.type == minus) {
        op = currentToken.type;
        match( op); // 消费 '+' 或 '-'

        parse_term();

        if (op == plus) {
            emit(OPR, 0, 2);// 加法
        } else {
            emit(OPR, 0, 3);
        }
    }
}


// 解析项（  Term  → Factor {∗ Factor | / Factor} ）
void parse_term() {
    parse_factor(); // 首先解析一个因子

    while (currentToken.type == times || currentToken.type == slash) {
        const PL0TokenType op = currentToken.type;
        match(  op); // 消费 '*' 或 '/'

        parse_factor(); // 右边的因子

        if (op == times) {
            emit(OPR, 0, 4);
        } else {
            emit(OPR, 0, 5);
        }
    }
}


// 解析因子（  Factor  → ident | number | ( Exp ) ）
void parse_factor() {
    if (currentToken.type == ident) {
        const int index = lookup_symbol(currentToken.lexeme.c_str());
        if (index == -1) {
            error(ERR_UNDECLARED_IDENTIFIER, currentToken.line, currentToken.column);
            return;
        }

        const Symbol* sym = get_symbol(index);
        if (sym->kind == OBJ_CONST) {
            emit(LIT, 0, sym->val);
        } else if (sym->kind == OBJ_VAR) {
            emit(LOD, currentLevel - sym->level, sym->address);

        } else {
            error(ERR_INVALID_IDENTIFIER_USAGE, currentToken.line, currentToken.column);
        }

        match(  ident); // 消费标识符
    }
    else if (currentToken.type == number) {
        emit(LIT, 0, strToInt(currentToken.lexeme));
        match(  number); // 消费 number
    }
    else if (currentToken.type == lparen) {
        match(  lparen); // 消费 '('
        parse_expression(); // 递归解析表达式
        if (currentToken.type != rparen) {
            error(ERR_EXPECT_RPAREN, currentToken.line, currentToken.column);
            return;
        }
        match(  rparen); // 消费 ')'
    }
    else {
        error(ERR_INVALID_FACTOR, currentToken.line, currentToken.column);
    }
}


