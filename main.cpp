#include "lexer/lexer.hpp"
//#include "parser/parser.hpp"
#include <iostream>
#include <vector>
#include <memory>


int main() {
    std::string sourceCode = R"(
        struct Node {
            let: i32 num;
            let: string; 
            let : Node
        }

        fn main() -> void {
            let i32 x = 42;
            println("{x}");
        }
    )";

    Lexer lexer(sourceCode);
    std::vector<Token> tokens = lexer.tokenize();

    for (const auto& token : tokens) {
        std::cout << "Token: " << static_cast<int>(token.type) << " Value: " << token.value << "\n";
    }

    return 0;
}
