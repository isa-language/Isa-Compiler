#include "lexer.hpp"
#include <vector>

bool isSeparator(char ch) {
    return ch == ';' || ch == ',' || ch == '(' || ch == ')' || ch == '{' || ch == '}' || ch == ':' || ch == '[' || ch == ']';
}

bool isOperator(char ch) {
    return ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '=' || ch == '!' || ch == '<' || ch == '>';
}

bool isDoubleOperator(char ch1, char ch2) {
    return (ch1 == '=' && ch2 == '=') || (ch1 == '!' && ch2 == '=') ||
           (ch1 == '<' && ch2 == '=') || (ch1 == '>' && ch2 == '=') ||
           (ch1 == '-' && ch2 == '>');
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;

    while (position < source.size()) {
        char current = source[position];

        if (isspace(current)) {
            position++;
            continue;
        }

        if (isalpha(current) || current == '_') {
            tokens.push_back(recognizeIdentifier());
        } else if (isdigit(current)) {
            tokens.push_back(recognizeNumber());
        } else if (current == '"') {
            tokens.push_back(recognizeString());
        } else if (current == 't' || current == 'f') {
            tokens.push_back(recognizeBool());
        } else if (isSeparator(current)) {
            tokens.emplace_back(TokenType::SEPARATOR, std::string(1, current));
            position++;
        } else if (isOperator(current)) {
            tokens.push_back(recognizeOperator());
        } else {
            tokens.emplace_back(TokenType::UNKNOWN, std::string(1, current));
            position++;
        }
    }

    tokens.emplace_back(TokenType::END_OF_FILE, "");
    return tokens;
}

Token Lexer::recognizeIdentifier() {
    size_t start = position;
    while (isalnum(source[position]) || source[position] == '_') {
        position++;
    }
    std::string value = source.substr(start, position - start);
    return Token(keywords.count(value) ? keywords.at(value) : TokenType::IDENTIFIER, value);
}

Token Lexer::recognizeNumber() {
    size_t start = position;
    bool isFloat = false;
    while (isdigit(source[position]) || source[position] == '.') {
        if (source[position] == '.') {
            isFloat = true;
        }
        position++;
    }
    std::string value = source.substr(start, position - start);
    return Token(isFloat ? TokenType::FLOAT_LITERAL : TokenType::INTEGER_LITERAL, value);
}

Token Lexer::recognizeString() {
    size_t start = ++position; // Skip the opening quote
    while (source[position] != '"' && position < source.size()) {
        position++;
    }
    std::string value = source.substr(start, position - start);
    position++; // Skip the closing quote
    return Token(TokenType::STRING_LITERAL, value);
}

Token Lexer::recognizeBool() {
    size_t start = position;
    while (isalpha(source[position])) {
        position++;
    }
    std::string value = source.substr(start, position - start);
    return Token(TokenType::BOOL_LITERAL, value);
}

Token Lexer::recognizeOperator() {
    char current = source[position];
    char next = (position + 1 < source.size()) ? source[position + 1] : '\0';

    if (isDoubleOperator(current, next)) {
        position += 2;
        return Token(TokenType::OPERATOR, std::string(1, current) + next);
    } else {
        position++;
        return Token(TokenType::OPERATOR, std::string(1, current));
    }
}
