# PL/0 编译器
## 目录
- [项目结构](#项目结构)
- [文法](#文法)
- [词法分析](#词法分析)
- [名字表](#名字表)
- [代码生成](#代码生成)
- [语法树](#语法树)
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
│   ├── ast.h             # AST 节点定义, 语法树
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
│   ├── ast.cpp
│   ├── symbol_table.cpp
│   └── common.cpp
│
├── input/                   `.pl0` 源程序
├── output/                 P-code、调试信息、符号表等
├── CMakeLists.txt           
└── README.md
```

Clion中设置工作目录为该项目根目录。

参考项目 reference-projects/c_base

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

## 代码生成
代码生成器负责将PL0代码转换为P-code。

在语法分析器中，在特定位置调用代码生成器的emit()函数生成P-code。
```c++
// codegen.h
enum fct  {LIT,...}; // 过程类型
//P-code指令结构体
struct Instruction {
    fct  f;  // 指令操作码
    int l;      // 层次差
    int a;      // 地址或立即数
};
//OPR l a  中a的类型
enum a_type {
    a_release = 0,    //释放数据段
...
};
extern int codeIndex;           // 当前指令计数
extern Instruction code[MAX_CODE_SIZE];      // 指令数组
void emit(fct f, int l, int a);  // 添加一条指令
```
```c++
// codegen.cpp
...
```
## 语法树
语法树是PL0代码的抽象语法表示，用于表示代码的结构。

在语法分析器中，会调用相应的语法分析函数，生成语法树。
语法树节点定义
```c++
struct ASTNode {
    ASTNodeType type;
    std::string value;
    std::vector<std::unique_ptr<ASTNode>> children;
    explicit ASTNode(const ASTNodeType t, std::string  val = "") : type(t), value(std::move(val)) {}
    void addChild(std::unique_ptr<ASTNode> child) {
        if (child) children.push_back(std::move(child));
    }
};

```
语法树示例
```pl0
var n, i, result;
procedure factorial;
begin
    result := 1;
    i := 1;
    while i <= n do
    begin
        result := result * i;
        i := i + 1;
    end;
end;
begin
    read (n);
    call factorial;
    write (result);
end.
```
```
AST:
 └── PROGRAM
    └── BLOCK
        ├── VAR_DECL
        │   ├── IDENTIFIER : n
        │   ├── IDENTIFIER : i
        │   └── IDENTIFIER : result
        ├── PROC_DECLS
        │   └── PROC_DECL : factorial
        │       └── BLOCK
        │           └── BEGIN_STMT
        │               ├── ASSIGN_STMT : result
        │               │   └── FACTOR_NUMBER : 1
        │               ├── ASSIGN_STMT : i
        │               │   └── FACTOR_NUMBER : 1
        │               ├── WHILE_STMT
        │               │   ├── COND_OP : <=
        │               │   │   ├── FACTOR_IDENTIFIER : i
        │               │   │   └── FACTOR_IDENTIFIER : n
        │               │   └── BEGIN_STMT
        │               │       ├── ASSIGN_STMT : result
        │               │       │   └── TERM_OP : *
        │               │       │       ├── FACTOR_IDENTIFIER : result
        │               │       │       └── FACTOR_IDENTIFIER : i
        │               │       ├── ASSIGN_STMT : i
        │               │       │   └── TERM_OP : +
        │               │       │       ├── FACTOR_IDENTIFIER : i
        │               │       │       └── FACTOR_NUMBER : 1
        │               │       └── EMPTY_STMT
        │               └── EMPTY_STMT
        └── BEGIN_STMT
            ├── READ_STMT
            │   └── FACTOR_IDENTIFIER : n
            ├── CALL_STMT : factorial
            ├── WRITE_STMT
            │   └── FACTOR_IDENTIFIER : result
            └── EMPTY_STMT
```
## 语法分析
语法分析器将词法分析器生成的词法单单元解析
语法分析器的工作流程：
1. 调用getNextToken()函数获取下一个词法单元，并根据词法单元的类型进行语法分析。
2. 调用相应的语法分析函数，进行语法分析。
3. 调用代码生成器的emit()函数生成P-code（若需要）。
4. 重复步骤1-3，直到词法分析器返回结束标记。

```c++
// parser.h
void parse_program();
void parse_block();
void parse_const_decl();
void parse_var_decl();
void parse_proc_decl();
void parse_stmt();
void parse_condition();
void parse_expression();
void parse_term();
void parse_factor();
int currentLevel = 0;  // 当前嵌套层级
int varCount[MAX_NESTING_LEVEL] = {0, 0, 0};      // 当前层级变量数量
```
### parse_program() 
***语法分析入口函数，开始解析整个程序（Program → Block .）***
```c++
void parse_program() {
    parse_block();
   //TODO: 判断是否以.结尾
}
```
### parse_block() 
***解析Block语法块 Block  → [ConstDecl] [VarDecl][ProcDecl] Stmt***

解析一个代码块，包含常量声明、变量声明、过程声明和语句。

### parse_const_decl() 
***解析ConstDecl语法块 ConstDecl → const ConstDef {, ConstDef} ;***

解析const声明语句，将常量定义加入符号表

### parse_var_decl() 
***解析VarDecl语法块 VarDecl → var ident {, ident} ;***

解析变量声明语句，将变量定义加入符号表

### parse_proc_decl() 
***解析ProcDecl语法块 ProcDecl → procedure ident ; Block ; {procedure ident ; Block ;}***

解析过程声明语句，将过程定义加入符号表

### parse_stmt() 
***解析Stmt语法块 Stmt   → ident := Exp | call ident | begin Stmt {; Stmt} end | if Cond then Stmt | while Cond do Stmt | ε***

***拓展文法Stmt   →  read (ident {,ident} ) | write ( Exp {, Exp} )***

1. 解析赋值语句
2. 解析过程调用语句
3. 解析复合语句
4. 解析条件语句
5. 解析循环语句
6. 空语句

### parse_condition() 
***解析Cond语法块 Cond  → odd Exp | Exp RelOp Exp***

解析条件表达式，包括奇偶判断和关系运算

### parse_expression() 
***解析Exp语法块 Exp   → [+ | − ] Term {+ Term | − Term}***

解析表达式，包括一元运算符（+、-）和加减运算

### parse_term() 
***解析Term语法块 Term  → Factor {∗ Factor | / Factor}***

解析项，包括乘除运算

### parse_factor() 
***解析Factor语法块 Factor  → ident | number | ( Exp )***

解析因子，包括标识符、数字和括号表达式

## 虚拟机
虚拟机用于执行P-code。

在语法分析器执行完代码生成后，执行P-code。

```c++
    int pc = 0;            // 程序计数器
    int bp = 0;            // 基地址指针
    int sp = -1;           // 栈顶指针
    int stack[ STACK_SIZE ] = {0}; // 数据栈
...
//根据指令 fct l a 执行相应操作
```