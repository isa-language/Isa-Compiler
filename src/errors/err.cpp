#include "frontend/lexer/token.hpp"
#include "errors/err.hpp"
#include <cwchar>
#include <string>
#include <cstdio>
#include <vector>
#include <cmath>
#include <iomanip>
#include "frontend/utils/file.hpp"
#ifdef _WIN32
    #define COLOR_RED     L"\033[1;31m"
    #define COLOR_YELLOW  L"\033[1;33m"
    #define COLOR_PURPLE  L"\033[1;35m"
    #define COLOR_RESET   L"\033[0m"

#else
    #define COLOR_RED     "\033[1;31m"
    #define COLOR_YELLOW "\033[1;33m"
    #define COLOR_PURPLE "\033[1;35m"
    #define COLOR_RESET  "\033[0m"
#endif

#ifdef _WIN32
    #define WIDE(str) L##str
#else
    #define WIDE(str) str
#endif

int countdigits(int number) {
    if (number == 0) return 1; 
    return std::floor(std::log10(std::abs(number))) + 1;
}

void Error::printError(const std::vector<Token> tokens) {
    #ifdef _WIN32
    const wchar_t* color;
    const wchar_t* errorType;
    std::wstring wmessage = string_to_wstring(message);
    std::wstring wfilename = string_to_wstring(filename);
    #else
    std::string color;
    std::string errorType;
    #endif
  
    switch (type) {
        case ErrorType::SyntaxError:
            color = COLOR_RED;
            errorType = WIDE("Syntax Error");
            break;
        case ErrorType::SemanticError:
            color = COLOR_YELLOW;
            errorType = WIDE("Semantic Error");
            break;
        case ErrorType::RuntimeError:
            color = COLOR_PURPLE;
            errorType = WIDE("Runtime Error");
            break;
        default:
            color = COLOR_RESET;
            errorType = WIDE("Unknown Error");
            break;
    }

    #ifdef _WIN32
    wprintf(L"%ls%ls:%ls:%ls: %ls: %ls%ls\n", 
            color,
            wfilename.c_str(),
            int_to_wstring(line).c_str(),
            int_to_wstring(column).c_str(),
            errorType,
            wmessage.c_str(),
            COLOR_RESET);
    #else
    std::cout << color << filename << ":" << line << ":" << column << ": " 
              << errorType << ": " << message << "\033[0m\n";
    #endif

    const Token* errorToken = nullptr;
    for (const auto& token : tokens) {
        if (token.line == line && token.column <= column && 
            (token.column + token.value.length()) > column) {
            errorToken = &token;
            break;
        }
    }

    if (errorToken) {
        std::vector<const Token*> lineTokens;
        for (const auto& token : tokens) {
            if (token.line == line) {
                lineTokens.push_back(&token);
            }
        }

        std::string fullLine;
        int currentColumn = 1;
        for (const auto* token : lineTokens) {
            while (currentColumn < token->column) {
                fullLine += ' ';
                currentColumn++;
            }
            if(token->type == TokenType::TOK_STRING_LITERAL) {
                fullLine += '"' + token->value + '"';
            } else {
                fullLine += token->value;
            }
            currentColumn += token->value.length();
        }
        #ifdef _WIN32
        wprintf(L"%ls | %ls\n", int_to_wstring(line).c_str(), string_to_wstring(fullLine).c_str());

        #else
        std::cout << std::setw(countdigits(line)) << line << " | " << fullLine << "\n";
        #endif
        int padding = countdigits(line) + 2 + (column - 1);
        #ifdef _WIN32
        wprintf(L"%*ls^\n", padding, L"");
        wprintf(L"%*ls└─ %ls%ls\n", padding, L"", wmessage.c_str(), COLOR_RESET);
        #else
        std::cout << std::string(padding, ' ') << color << "^\n";
        std::cout << std::string(padding, ' ') << "└─ " << message << "\033[0m\n";
        #endif
    }
}