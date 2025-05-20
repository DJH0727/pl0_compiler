# PL/0 编译器
## 目录
- [项目结构](#项目结构)
- [文法](#文法)
- [词法分析](#词法分析)
- [名字表](#名字表)
- [代码生成](#代码生成)
- [语法分析](#语法分析)
- [虚拟机](#虚拟机)


## 项目结构
```tree
pl0-compiler/
├── include/
│   ├── lexer.h           # 词法分析器接口
│   ├── parser.h          # 语法分析器接口
│   ├── codegen.h         # 代码生成器接口
│   ├── vm.h              # 虚拟机接口
│   ├── symbol_table.h       # 符号表接口
│   ├── token.h             # Token 结构体定义
│   └──common.h             # 公共工具
│
├── src/
│   ├── main.cpp
│   ├── lexer.cpp
│   ├── parser.cpp
│   ├── codegen.cpp
│   ├── vm.cpp
│   ├── symbol_table.cpp
│   └── common.cpp
│
├── input/                   `.pl0` 源程序
├── output/                 P-code、调试信息、符号表等
├── CMakeLists.txt           
└── README.md
```

Clion中设置工作目录为该项目根目录。

## 文法
```
基础文法：
  Program  → Block . 
  Block  → [ConstDecl] [VarDecl][ProcDecl] Stmt 
  ConstDecl → const ConstDef {, ConstDef} ; 
  ConstDef  → ident = number 
  VarDecl  → var ident {, ident} ; 
  ProcDecl  → procedure ident ; Block ; {procedure ident ; Block ;} 
  Stmt   → ident := Exp | call ident | begin Stmt {; Stmt} end | if Cond then Stmt | while Cond do Stmt | ε 
  Cond  → odd Exp | Exp RelOp Exp 
  RelOp  → = | <> | < | > | <= | >= 
  Exp   → [+ | − ] Term {+ Term | − Term} 
  Term  → Factor {∗ Factor | / Factor} 
  Factor  → ident | number | ( Exp ) 
拓展文法：
Stmt   →  read (ident {,ident} ) | write ( Exp {, Exp} )
支持单行注释： //...
```

## 词法分析

### token
```c++
// token.h
class Token {
public:
    PL0TokenType type;       // 词法单元的类型，比如标识符、关键字、加号等
    std::string lexeme;   // 原始文本，例如变量名 abc、关键字 begin、数字 123
    int line;             // 出现在源代码的第几行
    int column;           // 出现在该行的第几个字符位置
    // 构造函数
    ...
};

//PL0TokenType 枚举32种类型：
enum class PL0TokenType {
    nul,      // 空
    ident,    // 标识符
    number,   // 数字
    ...
};
```
### lexer
词法分析器的将PL0源代码字符串转换为一系列的词法单元。

在语法分析器中，会调用getNextToken()函数获取下一个词法单元，并根据词法单元的类型进行语法分析。
```c++
// lexer.h
/*currentToken在parser中使用*/
extern Token currentToken;  // 当前词法单元
void getNextToken();        // 获取下一个词法单元
void getch();               // 获取下一个字符

```
```c++
// lexer.cpp
static const char* source = NULL;  // 源代码字符串
static int pos = 0;         // 当前字符索引
static int line = 1;        // 当前行号
static int column = 1;      // 当前列号
static char ch;             // 当前字符
//getch 读取下一个字符 并更新pos、line、column
//主要函数：getNextToken()
/*当前字符为空白符时，跳过空白符，直到遇到非空白符*/
    while (ch==' ' || ch==10 || ch==13 || ch==9)  /* 忽略空格、换行、回车和TAB */
    {
        getch();
    }
    
/* 设置当前token的行列信息 */
 currentToken.line = line;
 currentToken.column = column;
 
/* 数字 */
if (isDigit(ch)) {
    std::string numStr = ch;
    getch();
    while (isDigit(ch)) {
        numStr += ch; getch();
    }
    //TODO: 整数不能以0开头
    //TODO: 判断数字长度是否超过最大值
    //TODO: 数字后面不能跟标识符，不能出现形如 123abc 的情况
    //TODO: 设置currentToken
    return;    
}

/* 标识符 */
if (isLetter(ch)) {
        std::string idStr = ch;
        getch();
        while (isLetter(ch) || isDigit(ch)) {
            idStr += ch;getch();
        }
        //TODO: 判断标识符长度是否超过最大值
        //判断关键字
        const int index = isKeywords(idStr);
        //index != -1 代表是关键字
        //否则是标识符
        //TODO: 设置currentToken
        return;
}

/* 运算符 */
if (ch == ':') {
        getch();
        if (ch == '=') {// 赋值运算符
            //TODO: 设置currentToken
            getch();
        }
        else error(ILLEGAL_COLON, line, column);
        return;
}
if (ch == '<') {
        getch();
        if (ch == '=') { // 小于等于运算符
           //TODO: 设置currentToken
            getch();
        }
        else if (ch == '>') { // 不等于运算符
            //TODO: 设置currentToken
            getch();
        }
        else {
           //TODO: 设置currentToken
           //这里不需要getch()，否则类似a>0中的0会被吃掉
        }
        return;
}
if (ch == '>') {
        getch();
        if (ch == '=') { // 大于等于运算符
            //TODO: 设置currentToken
            getch();
        }
        else {
            //TODO: 设置currentToken
        }
        return;
}

/* 其他单字符符号 */
    // 处理单字符符号
    const int index = isSingleCharSymbol(ch);
    if (index != -1) {
        //TODO: 设置currentToken
        getch();
    }
    else if (ch == 0) {return;}
    else {
        //非法字符，比如中文字符
        error(ILLEGAL_CHARACTER, line, column);
    }
```

## 名字表
在实现语法分析前，需要先实现名字表。名字表用于存储变量、过程名、常量名等符号的定义和相关信息。

语法分析时，需要维护一个符号表，用于存储当前作用域的符号定义。
```c++
// symbol_table.h
typedef struct {
    char name[MAX_IDENTIFIER_LENGTH + 1];  // 符号名
    SymbolKind kind;           // 符号种类
    int val;                   // 常量值（若是常量）
    int level;                 // 静态作用域层
    int address;               // 地址（变量和过程）
    int size;                  // 过程数据区大小
} Symbol;
/*整个程序只维护一个符号表，符号表中存储了全部符号的定义
这样导致在不同的作用域中不能使用同名符号*/
extern Symbol symbolTable[SYMBOL_TABLE_SIZE];//符号表
extern int symbol_count;//符号表中符号个数
//插入符号，返回符号表索引
int enter_symbol(SymbolKind kind, const char* name, int val, int level, int address, int size);
int lookup_symbol(const char* name);//查找符号，返回符号表索引
Symbol* get_symbol(int index);//获取符号指针
```
```c++
// symbol_table.cpp
int enter_symbol(...) {
//TODO:判断是否超过最大符号个数
//TODO:判断是否重复定义
symbolTable[symbol_count] = new Symbol{...};//伪代码
return symbol_count++;
}
/*查找符号，返回索引，找不到返回 -1 */
int lookup_symbol(const char* name) {
// 从后向前查找，优先最近声明的
    return find_symbol? index:-1;
}
/*获取符号指针*/
Symbol* get_symbol(const int index) {
    return(index < 0 || index >= symbol_count) ? nullptr : &symbolTable[index];
}
```

