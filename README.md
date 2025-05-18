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