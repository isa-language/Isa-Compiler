#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

enum class TokenType {
    LET,
    FN,
    FUNCTION_CALL,
    PROTOTYPEFUN,
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
