cmake_minimum_required(VERSION 3.5)
project(Compiler)

find_package(LLVM REQUIRED CONFIG)
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(AddLLVM)

add_executable(Compiler main.cpp ./lexer/lexer.cpp)
target_link_libraries(Compiler LLVM)