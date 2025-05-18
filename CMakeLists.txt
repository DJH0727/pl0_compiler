cmake_minimum_required(VERSION 3.30)
project(pl0_compiler)

set(CMAKE_CXX_STANDARD 20)

add_executable(pl0_compiler
        src/main.cpp
        src/lexer.cpp
        src/parser.cpp
        src/codegen.cpp
        src/vm.cpp
        src/symbol_table.cpp
        src/common.cpp
)

include_directories(include)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
