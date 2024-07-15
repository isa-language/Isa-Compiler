#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <unordered_map>

enum class TokenType {
    LET,
    FN,
    STRUCT,
    RETURN,
    FOR,
    WHILE,
    IF,
    ELSE,
    SWITCH,
    CASE,
    DEFAULT,
    PRINTLN,
    IDENTIFIER,
    INTEGER_LITERAL,
    FLOAT_LITERAL,
    BOOL_LITERAL,
    STRING_LITERAL,
    OPERATOR,
    SEPARATOR,
    TYPE,
    UNKNOWN,
    END_OF_FILE
};

struct Token {
    TokenType type;
    std::string value;

    Token(TokenType t, const std::string& v) : type(t), value(v) {}
};

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
        {"bool", TokenType::TYPE}, {"string", TokenType::TYPE}
    };
};

