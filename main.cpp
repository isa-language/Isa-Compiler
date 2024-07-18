#include "lexer/lexer.hpp"
//#include "parser/parser.hpp"
//#include "parser/ast.hpp"

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>




int main() {
    std::string code = R"(let: i32 num = 10;
        let: f32 ponto = 0.40;
        let: array[i32, 3] vetor = { 10, 5, 3 };
        let: str[10] fixed_str = "Hello";
        let: string dynamic_str = "Hello, World!";
        let: u8 small_number = 255;
        let: u32 large_number = 4294967295;

        struct Tipo {
            let: i32 value;
            let: string nome;

            fn new(v: i32, n: string) -> Tipo {
                let: Tipo t = Tipo { value = v, nome = n };
                return t;
            }
        };

        fn main() -> i32 {
            let: Tipo t = Tipo::new(42, "Exemplo");
            for (let: i8 i = 0; i < 10; i++) {
                println("{}", i);
            }

            while (num > 0) {
                println("{}", num);
                num = num - 1;
            }

            if (num == 0) {
                println("End of loop");
            } else {
                println("Error");
            }

            switch (num) {
                case 0:
                    println("Zero");
                    break;
                case 1:
                    println("One");
                    break;
                default:
                    println("Other");
            }

            let: i32 *p_num = new i32(10);
            let: i32 &ref_num = num;
            ref_num = 20;

            return 0;
        }
        )";

    Lexer lexer(code);
    auto tokens = lexer.tokenize();

    for (const auto& token : tokens) {
        std::cout << "Token(" << token.type << ", \"" << token.value << "\", " << token.line << ", " << token.column << ")\n";
    }

    return 0;
}
