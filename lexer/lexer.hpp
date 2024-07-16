#pragma once
#include "token.hpp"
#include <string>
#include <vector>

class Lexer {
public:
    Lexer(const std::string& source) : source(source), position(0) {}

    std::vector<Token> tokenize();

private:
    std::string source;
    size_t position;
    Token recognizeIdentifier();
    Token recognizeNumber();
    Token recognizeString();
    Token recognizeBool();
    Token recognizeOperator();

    const std::unordered_map<std::string, TokenType> keywords = {
        {"let", TokenType::LET}, {"fn", TokenType::FN}, {"struct", TokenType::STRUCT}, {"return", TokenType::RETURN},
        {"for", TokenType::FOR}, {"while", TokenType::WHILE}, {"if", TokenType::IF}, {"else", TokenType::ELSE},
        {"switch", TokenType::SWITCH}, {"case", TokenType::CASE}, {"default", TokenType::DEFAULT}, {"println", TokenType::PRINTLN},
        {"i8", TokenType::TYPE}, {"i16", TokenType::TYPE}, {"i32", TokenType::TYPE}, {"i64", TokenType::TYPE},
        {"f8", TokenType::TYPE}, {"f16", TokenType::TYPE}, {"f32", TokenType::TYPE}, {"f64", TokenType::TYPE},
        {"bool", TokenType::TYPE}, {"string", TokenType::TYPE}, {"->", TokenType::SEPARATOR}
    };
};
