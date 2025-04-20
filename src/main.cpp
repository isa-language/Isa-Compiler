#include "frontend/lexer/lexer.hpp"
#include "frontend/lexer/token.hpp"

#include "frontend/parser/parser.hpp"
//#include "parser/ast.hpp"
#include "frontend/parser/parser_ast.hpp"
#include "frontend/utils/file.hpp"
#include "frontend/utils/results.hpp"
#include "frontend/utils/initialized_flags.hpp"
#include "llvm/Config/abi-breaking.h"
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include "nametoken.hpp"



#include "llvm/Support/Compiler.h"
// #define DEBUG 1 // cmake -DENABLE_DEBUG=ON .. ou OFF 

#ifdef _WIN32

#include <io.h>
#include <fcntl.h>
#include <windows.h>
extern "C" {LLVM_ATTRIBUTE_USED int EnableABIBreakingChecks = 1;}
#elif defined(__linux__)
LLVM_ATTRIBUTE_USED int llvm::EnableABIBreakingChecks = 1;
#endif

void enable_utf8_console() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
}


int main(int argc, char **argv) {


    enable_utf8_console();


    Result<std::string> init_flags = inicialize_flags(argc, argv);
    if(init_flags.is_err()) {
        std::cerr << "Erro: " << init_flags.error.value();
        return EXIT_FAILURE;
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

    IsaParser parser(tokens,argv[1]);
    auto AST = parser.parseProgram();

    IsaLLVM isa(get_output_filename());
    if(!parser.getErr()) isa.exec(std::move(AST));
    if(flags.isActive(FlagID::EmittLexer)) {
        for (const auto& token : tokens) {
            std::cout << "Token(" << tokenStrings[token.type] << ", \"" << token.value << "\", " << token.line << ", " << token.column << ")\n";
        }
    }
    return 0;
}
