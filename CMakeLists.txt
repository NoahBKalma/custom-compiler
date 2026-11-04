cmake_minimum_required(VERSION 3.16)

project(compiler LANGUAGES CXX)

# C++20, no compiler-specific extensions
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(compiler
        src/main.cpp
        src/Lexer.cpp
        src/Lexer.h
        src/Tokens.h
        src/AST.h
        src/Parser.cpp
        src/Parser.h
        src/CodeGen.h
        src/CodeGen.cpp
)

# Lets RUNTIME_DIR be used in main.cpp
target_compile_definitions(compiler PRIVATE
        RUNTIME_DIR="${CMAKE_CURRENT_SOURCE_DIR}/runtime")