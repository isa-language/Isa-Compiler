/**
 *
 * IsaLLVMCompilerLEXER
 *
 **/
#pragma once
#ifndef isaLEXER
#define isaLEXER

#include <vector>
#include <string>
#include <cctype>
#include <unordered_map>
/* include files project*/
#include "token.hpp"

class Lexer {
public:
    Lexer(const std::string& source)
        : source(source), position(0), line(1), column(1) {}

    std::vector<Token> tokenize();

private:
    std::string source;
    size_t position;
    int line;
    int column;

    void handleWhitespace();
    Token handleIdentifierOrKeyword();
    void handleComents();
    void handleBlockComents();
    Token handleNumber();
    Token handleStringLiteral();
    Token handleOperatorOrDelimiter();
};
#endif // !DEBUG
