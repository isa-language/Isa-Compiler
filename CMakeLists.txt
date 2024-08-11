cmake_minimum_required(VERSION 3.5)
project(Compiler)

find_package(LLVM REQUIRED CONFIG)
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(AddLLVM)

add_executable(Compiler main.cpp ./src/lexer.cpp)
target_link_libraries(Compiler LLVM)

# clang++ -S -emit-llvm ex.cpp -o ex.ll
# clang++ -emit-llvm ex.cpp -o ex.bc
# llvm-dis ex.bc

# opt -O3 ex.ll -o ex_op.ll
# llc ex.ll -o ex.s
# clang++ ex.s -o ex
