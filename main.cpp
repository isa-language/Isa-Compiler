#include "src/lexer.hpp"
//#include "parser/parser.hpp"
//#include "parser/ast.hpp"
#include "src/token.hpp"
#include "src/parser.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

std::string fileopen(const std::string& filename) {
    std::fstream fs;
    std::stringstream ss;
    fs.open(filename , std::fstream::in);
    if(fs.is_open()) {
        ss << fs.rdbuf();
    }
    fs.close();
    return ss.str();
}


int main(int argc, char **argv) {

    /* Exemplo code */
    std::string codes = " ";
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
        std::cout << "Token(" << token.type << ", \"" << token.value << "\", " << token.line << ", " << token.column << ")\n";
    }

    /**
     * Compiler instance
     * implemented instance Compiler 
     * O methodo exec receberar um vector de tokens para o parser AST 
     * */
    IsaLLVM isa;
    isa.exec(tokens);
    


    return 0;
}
