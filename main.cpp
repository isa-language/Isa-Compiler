#include "src/lexer.hpp"
//#include "parser/ast.hpp"
#include "src/token.hpp"
#include "src/parser.hpp"
#include <iostream>
#include <string>
#include <vector>
#include "nametoken.hpp"
#include "src/file.hpp"
#include "flags.hpp"
#include "src/parser_ast.hpp"

/* alterar valor no modo de compilação! */
// #define DEBUG 0 // cmake -DENABLE_DEBUG=ON .. ou OFF 



int main(int argc, char **argv) {
#if DEBUG
    
    std::string codes = R"(let:i32 test = 1000;
        let:i32 num = 22;
)";
    if(argc > 1) {
        codes = fileopen(std::string(argv[1]));
    }
    std::vector<std::string> err = splitByErr(codes);
    Lexer lexer(codes);
    IsaParser parser(lexer.tokenize(),argv[1],err);
    auto AST = parser.parseProgram();

    IsaLLVM isa;
    if(!parser.getErr()) isa.exec(std::move(AST));
   
    
#else

     std::string codes {"let:i32 num = 10;"};
    if(argc > 1) {
        codes = fileopen(std::string(argv[1]));
    }
    // std::fstream open(); 
    
    /**
    * 
    * Lexer generetor.
    *
    */
    Lexer lexer(codes);
    auto tokens = lexer.tokenize();

    for (const auto& token : tokens) {
        std::cout << "Token(" << tokenStrings[token.type] << ", \"" << token.value << "\", " << token.line << ", " << token.column << ")\n";
    }

    /**
     * Compiler instance
     * implemented instance Compiler 
     * O methodo exec receberar um vector de tokens para o parser AST 
     * */
    //IsaLLVM isa;
    //isa.exec(tokens);
    #endif

    return 0;
}
