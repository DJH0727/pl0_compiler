```tree
pl0-compiler/
├── include/
│   ├── lexer.h           # 词法分析器接口
│   ├── parser.h          # 语法分析器接口
│   ├── codegen.h         # 代码生成器接口
│   ├── vm.h              # 虚拟机接口
│   ├── symbol_table.h       # 符号表接口
│   ├── token.h             # Token 结构体定义
│   ├──global.h             # 全局变量定义
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
文法
```
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
```

parse_const_decl()
```
当前token是 const，确认并跳过（调用 getNextToken()）
解析一个 ConstDef，即：标识符 ident
下一个token必须是 =（eql）
下一个token必须是 number
把这个常量信息加入符号表
如果下一个token是逗号（comma），继续解析下一个 ConstDef，否则结束
最后必须是分号（semicolon）
```

parse_var_decl()
```
匹配 var
至少有一个 ident（标识符）
允许逗号分隔的多个 ident
最后匹配分号 ;
把这些变量信息加入符号表
```
parse_proc_decl()
```
以 procedure 开头；
后面跟着一个标识符；
然后是分号；
然后是一个 Block；
再跟一个分号；
支持多个过程定义（即递归调用自身处理 {procedure ident ; Block ;} 部分）；
```