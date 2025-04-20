#include "flags.hpp"
#include "frontend/lexer/lexer.hpp"
#include "frontend/lexer/token.hpp"
#include "frontend/parser/parser.hpp"
#include "frontend/parser/parser_ast.hpp"
#include "frontend/utils/file.hpp"
#include "frontend/utils/results.hpp"
#include "frontend/utils/initialized_flags.hpp"
#include "llvm/Config/abi-breaking.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include "nametoken.hpp"

#include "llvm/Support/Compiler.h"
// #define DEBUG 1 // cmake -DENABLE_DEBUG=ON .. ou OFF 

void print_help() {
    #ifdef _WIN32
    wprintf(L"usage: isa <OPTION>... [<COMPONENT>...]\n\nOptions:\n");
    #else 
        std::cout << "Options:" << '\n';
    #endif
    for(auto descrip : flagDescriptions) {
        #ifdef _WIN32
            wprintf(L"  %ls\n", char_to_wstring(descrip).c_str());
        #else 
            std::cout << descrip << '\n';
        #endif
    }
}

#ifdef _WIN32

    #include <io.h>
    #include <fcntl.h>
    #include <windows.h>
    extern "C" {LLVM_ATTRIBUTE_USED int EnableABIBreakingChecks = 1;}
#elif defined(__linux__)
    LLVM_ATTRIBUTE_USED int llvm::EnableABIBreakingChecks = 1;
#endif

int main(int argc, char **argv) {
    #ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
        _setmode(_fileno(stdout), _O_U8TEXT);
    #endif

    Result<std::string> init_flags = inicialize_flags(argc, argv);
    if(init_flags.is_err()) {
        std::cerr << "Erro: " << init_flags.error.value();
        print_help();
        return EXIT_FAILURE;
    }
    if(flags.isActive(FlagID::Help)) {
        print_help();
        return EXIT_SUCCESS;
    }
  
    Result<std::string> codes;
    auto filename = get_input_filename();


    if(filename.is_err()) {
        std::cerr << "Erro: " << filename.error.value();
        return EXIT_FAILURE;
    } else {
        codes = fileopen(std::string(filename.unwrap()));
        if(codes.is_err()) {
            std::cerr << codes.error.value();
            return 0;
        }
    }


    Lexer lexer(codes.unwrap());
    auto tokens = lexer.tokenize();

    IsaParser parser(tokens,filename.unwrap());
    auto AST = parser.parseProgram();

    IsaLLVM isa(get_output_filename());
    if(!parser.getErr()) isa.exec(std::move(AST));
    if(flags.isActive(FlagID::EmittLexer)) {
        #ifdef _WIN32
            wprintf(L"\n\nTokens emitt:\n\n");
        #else
            std::cout << "Tokens emitt:\n\n";
        #endif
        for (const auto& token : tokens) {
            #ifdef _WIN32
                wprintf(L"Token(%ls, L\"%ls\", %ls, %ls)\n", char_to_wstring(tokenStrings[token.type]).c_str(), string_to_wstring(token.value).c_str(), int_to_wstring(token.line).c_str(), int_to_wstring(token.column).c_str());
            #else
                std::cout << "Token(" << tokenStrings[token.type] << ", \"" << token.value << "\", " << token.line << ", " << token.column << ")\n";
            #endif
        }
    }
    return 0;
}
