#pragma once

#include <string>
enum TokenType {
    // Keywords
    TOK_LET,
    TOK_FN,
    TOK_STRUCT,
    TOK_RETURN,
    TOK_FOR,
    TOK_WHILE,
    TOK_IF,
    TOK_ELSE,
    TOK_SWITCH,
    TOK_CASE,
    TOK_DEFAULT,
    TOK_NEW,
    TOK_EXTERN,

    // Types
    TOK_TYPE,
    
    // Literals
    TOK_INTEGER_LITERAL,
    TOK_FLOAT_LITERAL,
    TOK_BOOL_LITERAL,
    TOK_STRING_LITERAL,
    TOK_IDENTIFIER,
    TOK_ARRAY,
    // Operators and Delimiters
    TOK_ASSIGN,  // '='
    TOK_ARROW,  // ->
    TOK_SEMICOLON,  // ';'
    TOK_COMMA,  // ','
    TOK_LPAREN,  // '('
    TOK_RPAREN,  // ')'
    TOK_LBRACE,  // '{'
    TOK_RBRACE,  // '}'
    TOK_LBRACKET,  // '['
    TOK_RBRACKET,  // ']'
    TOK_PLUS,  // '+'
    TOK_MINUS,  // '-'
    TOK_STAR,  // '*'
    TOK_SLASH,  // '/'
    TOK_EQUAL,  // '=='
    TOK_NOT_EQUAL,  // '!='
    TOK_LT,  // '<'
    TOK_GT,  // '>'
    TOK_LE,  // '<='
    TOK_GE,  // '>='
    TOK_COLON,  // ':'
    TOK_DOT,  // '.'
    TOK_AMP,    //&

    // End of File
    TOK_EOF,

    // Errors
    TOK_ERROR
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;

    Token(TokenType type, const std::string& value, int line, int column)
        : type(type), value(value), line(line), column(column) {}
    Token() {}
};
