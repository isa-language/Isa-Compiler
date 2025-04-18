cmake_minimum_required(VERSION 3.20)
project(IsaLLVM)


set(LLVM_CMAKE_DIR "/usr/local/lib/cmake/llvm/")
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(AddLLVM)
find_package(LLVM REQUIRED CONFIG)


set(LLVM_REQUIRED_COMPONENTS
    Core
    Support
    MC
    ExecutionEngine
    Target
    X86CodeGen
    TransformUtils
    Analysis
    AsmParser
    Object
    Passes
)

option(ENABLE_DEBUG "Enable debug mode" OFF)

if(ENABLE_DEBUG)
    add_compile_definitions(DEBUG=1)
else()
    add_compile_definitions(DEBUG=0)
endif()


set(SOURCES
    src/main.cpp
    src/frontend/lexer/lexer.cpp
    src/errors/err.cpp
    src/frontend/parser/parser.cpp
    src/frontend/parser/ast.cpp
)

add_executable(isa ${SOURCES})

target_include_directories(isa PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${LLVM_INCLUDE_DIRS}
)

target_link_libraries(isa PRIVATE LLVM)