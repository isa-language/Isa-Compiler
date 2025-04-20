cmake_minimum_required(VERSION 3.20)
project(IsaLLVM)

get_filename_component(DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../" ABSOLUTE)

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
    ${DIR}/src/main.cpp
    ${DIR}/src/frontend/lexer/lexer.cpp
    ${DIR}/src/errors/err.cpp
    ${DIR}/src/frontend/parser/parser.cpp
    ${DIR}/src/frontend/parser/ast.cpp
)

add_executable(isa ${SOURCES})

target_include_directories(isa PRIVATE
    ${DIR}/src
    ${DIR}/third_party/indicators/include
    ${LLVM_INCDIRDIRLUDE_DIRS}
)

target_link_libraries(isa PRIVATE LLVM)