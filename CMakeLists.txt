cmake_minimum_required(VERSION 3.5)
project(IsaLLVM)

find_package(LLVM REQUIRED CONFIG)
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(AddLLVM)

option(ENABLE_DEBUG "Enable debug mode" OFF)

if(ENABLE_DEBUG)
    add_compile_definitions(DEBUG=1)
else()
    add_compile_definitions(DEBUG=0)
endif()

add_executable(isa main.cpp ./src/lexer.cpp ./src/err.cpp ./src/parser.cpp ./src/ast.cpp)
target_link_libraries(isa LLVM)

# clang++ -S -emit-llvm ex.cpp -o ex.ll
# clang++ -emit-llvm ex.cpp -o ex.bc
# llvm-dis ex.bc

# opt -O3 ex.ll -o ex_op.ll
# llc ex.ll -o ex.s
# clang++ ex.s -o ex
#clang -O3 -o out out_op.ll -Wl,--no- as-needed -lm