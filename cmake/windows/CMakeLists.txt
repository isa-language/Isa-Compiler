cmake_minimum_required(VERSION 3.20)
project(IsaLLVM)

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_C_COMPILER "clang")
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_FLAGS "--target=x86_64-w64-windows-gnu -D_WIN32_WINNT=0x0601 -DNOGDI -DNOMINMAX")
set(CMAKE_EXE_LINKER_FLAGS "-fuse-ld=lld -static")

set(LLVM_INSTALL_DIR "~/llvm-mingw-install") # install build llvm-isalang -> $(HOME)/llvm-mingw-install
set(LLVM_DIR "${LLVM_INSTALL_DIR}/lib/cmake/llvm")

find_package(LLVM REQUIRED CONFIG)

message(STATUS "LLVM found: ${LLVM_DIR}")
message(STATUS "LLVM Includes: ${LLVM_INCLUDE_DIRS}")
message(STATUS "LLVM Libraries: ${LLVM_LIBRARY_DIRS}")

set(LLVM_REQUIRED_COMPONENTS
    Core
    Support
    MC
    ExecutionEngine
    MCJIT 
    Target
    X86CodeGen
    X86AsmParser
    TransformUtils
    Analysis
    AsmParser
    Object
    Passes
)

llvm_map_components_to_libnames(LLVM_LIBS ${LLVM_REQUIRED_COMPONENTS})

get_filename_component(ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../" ABSOLUTE)

set(SOURCES
    ${ROOT_DIR}/src/main.cpp
    ${ROOT_DIR}/src/frontend/lexer/lexer.cpp
    ${ROOT_DIR}/src/errors/err.cpp
    ${ROOT_DIR}/src/frontend/parser/parser.cpp
    ${ROOT_DIR}/src/frontend/parser/ast.cpp
)

option(ENABLE_DEBUG "Enable debug mode" OFF)
if(ENABLE_DEBUG)
    add_compile_definitions(DEBUG=1)
    add_compile_options(-g -O0)
else()
    add_compile_definitions(DEBUG=0)
    add_compile_options(-O2)
endif()

add_compile_options(-Wno-tautological-constant-out-of-range-compare)

add_executable(isa.exe ${SOURCES})

target_include_directories(isa.exe PRIVATE
    ${ROOT_DIR}/src
    ${ROOT_DIR}/third_party/indicators/include
    ${ROOT_DIR}/third_party/termcolor/include

    ${LLVM_INCLUDE_DIRS}
)

target_link_directories(isa.exe PRIVATE
    ${LLVM_LIBRARY_DIRS}
)

target_link_libraries(isa.exe PRIVATE
    ${LLVM_LIBS}
    mingw32
    winpthread
    stdc++
    psapi
    shell32
    ole32
    uuid
    advapi32
    ws2_32
)

target_compile_definitions(isa.exe PRIVATE
    __STDC_CONSTANT_MACROS
    __STDC_FORMAT_MACROS
    __STDC_LIMIT_MACROS
)

set_target_properties(isa.exe PROPERTIES
    LINK_FLAGS "-fuse-ld=lld -static"
    LINK_DEPENDS_NO_SHARED TRUE
)