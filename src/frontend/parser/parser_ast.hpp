#ifndef IsaLLVM_PARSER
#define IsaLLVM_PARSER

#include <cstdlib>
#include <memory>
#include <string>
#include <iostream>
#include <system_error>
#include <vector>
#include "frontend/lexer/token.hpp"
#include "frontend/parser/ast.hpp"
#include "errors/err.hpp"

class IsaParser {
    std::vector<Token> tokens;
    int countertoken = 0;
    std::string filename;
    std::vector<Token> errors;
    std::vector<std::unique_ptr<ASTNode>> program;
    bool err = false;

    const Token& currentToken() const {
        if (countertoken >= tokens.size()) return tokens.back();
        return tokens[countertoken];
    }


    const Token& previous() const { 
        if (countertoken == 0) return tokens.front();
        return tokens[countertoken - 1]; 
    }

    bool isAtEnd() const { 
        return currentToken().type == TOK_EOF || countertoken >= tokens.size();
    }

    bool match(TokenType type) {
        if (!isAtEnd() && currentToken().type == type) {
            countertoken++;
            return true;
        }
        return false;
    }

    bool match(const std::initializer_list<TokenType>& types) {
        for (auto type : types) {
            if (!isAtEnd() && currentToken().type == type) {
                countertoken++;
                return true;
            }
        }
        return false;
    }

    bool check(TokenType type) const { 
        return !isAtEnd() && currentToken().type == type; 
    }

    Token consume(TokenType type) {
        if (!check(type)) {
            error("Unexpected token");
            return Token{TOK_ERROR, "", currentToken().line, currentToken().column};
        }
        Token value = currentToken();
        countertoken++;
        return value;
    }

    void expect(TokenType type, const std::string& message) {
        if (!match(type)) {
            error(message);
        }
    }

    void error(const std::string& message) {
        err = true;
        Error(ErrorType::SyntaxError, message, currentToken().line, currentToken().column, filename).printError(errors);
    }

    std::string parseIdentifier() {
        if (match(TOK_IDENTIFIER)) {
            return tokens[countertoken-1].value;
        }
        error("Expected 'var name' statement");
        return "";
    }

    std::string parseType() {
        if (match(TOK_TYPE)) {
            return tokens[countertoken-1].value;
        }
        error("Expected 'type' statement");
        return "";
    }

    std::unique_ptr<ASTNode> matchDeclaration() {
        if (isAtEnd()) return nullptr;

        switch (currentToken().type) {
            case TOK_LET:       return matchVariableDeclaration();
            case TOK_FN:        return parserFunctionDeclaration();
            case TOK_STRUCT:    error("Struct declaration not supported yet"); break;
            case TOK_IF:        error("If statement not supported yet"); break;
            case TOK_WHILE:     error("While statement not supported yet"); break;
            case TOK_EXTERN:    {
                countertoken++;
                return parserFunctionDeclaration(true);
            }
            default:
                error("Invalid declaration.");
        }
        countertoken++;
        return nullptr;
    }

    std::unique_ptr<ASTNode> matchDeclaration(Token type) {
        if (isAtEnd()) return nullptr;

        switch (currentToken().type) {
            case TOK_LET:       return matchVariableDeclaration();
            case TOK_FN:        return parserFunctionDeclaration();
            case TOK_STRUCT:    error("Struct declaration not supported yet"); break;
            case TOK_IF:        error("If statement not supported yet"); break;
            case TOK_WHILE:     error("While statement not supported yet"); break;
            case TOK_RETURN:    return parserReturn(type);
            case TOK_IDENTIFIER: return parserCallFunction();
            default:
                error("Invalid declaration.");
        }
        countertoken++;
        return nullptr;
    }

    std::unique_ptr<ASTNode> parserFunctionDeclaration(bool externFunc = false) {
        Token identifier;
        Token returnType;
        
        if (check(TOK_FN)) consume(TOK_FN);
        if (check(TOK_IDENTIFIER)) identifier = consume(TOK_IDENTIFIER);
        else error("Expected function name");
        
        if (!match(TOK_LPAREN)) error("Expected '(' after function name");

        auto parameters = parseParameterList();
        if (!match(TOK_RPAREN)) error("Expected ')' after parameters");
        if (!match(TOK_ARROW)) error("Expected '->' after parameters");

        if (check(TOK_TYPE)) returnType = consume(TOK_TYPE);
        else error("Expected return type");

        if (match(TOK_SEMICOLON)) {
            return std::make_unique<FunctionInstantiationNode>(identifier.value, returnType.value, std::move(parameters), externFunc);
        } else if (match(TOK_LBRACE)) {
            auto body = parseStatementList(returnType);
            return std::make_unique<FunctionNode>(identifier.value, returnType.value, std::move(parameters), std::move(body));
        } else {
            error("Expected ';' or '{' after function declaration");
        }
        return nullptr;
    }

    std::vector<std::unique_ptr<ASTNode>> parseStatementList(Token typeFunction) {
        std::vector<std::unique_ptr<ASTNode>> statements;
        
        while (!check(TOK_RBRACE) && !isAtEnd()) {
            if (check(TOK_RETURN)) {
                statements.push_back(parserReturn(typeFunction));
            } else {
                auto stmt = matchDeclaration(typeFunction);
                if (stmt) statements.push_back(std::move(stmt));
            }
        }
        
        if (!match(TOK_RBRACE)) {
            error("Expected '}' at the end of function");
        }
        
        return statements;
    }

    std::unique_ptr<ReturnNode> parserReturn(Token type) {
        if (check(TOK_RETURN)) consume(TOK_RETURN);
        auto value = parseBinaryExpression(type);
        if(match(TOK_SEMICOLON)) return std::make_unique<ReturnNode>(std::move(value));
        else error("Expected ';' after return statement");
        return nullptr;
    }

    std::vector<std::unique_ptr<VariableDeclarationNode>> parseParameterList() {
        std::vector<std::unique_ptr<VariableDeclarationNode>> parameters;
        if (check(TOK_RPAREN)) return parameters;
        
        do {
            Token identifier, type;
            if (check(TOK_IDENTIFIER)) identifier = consume(TOK_IDENTIFIER);
            else error("Expected identifier");

            if (!match(TOK_COLON)) error("Expected ':' after identifier");
            
            if (check(TOK_TYPE)) type = consume(TOK_TYPE);
            else error("Expected type after ':'");
            parameters.push_back(std::make_unique<VariableDeclarationNode>(identifier.value, type.value));
        } while (match(TOK_COMMA));
        
        return parameters;
    }

    std::vector<std::unique_ptr<ASTNode>> parseParameterListCallFunction() {
        std::vector<std::unique_ptr<ASTNode>> parameters;
        if (check(TOK_RPAREN)) return parameters;
        
        do {
            Token identifier;
            if (check(TOK_IDENTIFIER)) identifier = consume(TOK_IDENTIFIER);
            else error("Expected identifier");
            
            parameters.push_back(std::make_unique<VariableValueNode>(identifier.value));
        } while (match(TOK_COMMA));
        
        return parameters;
    }

    std::unique_ptr<FunctionCallNode> parserCallFunction() {
        Token identifier;
        if(check(TOK_IDENTIFIER)) identifier = consume(TOK_IDENTIFIER);
        if (!match(TOK_LPAREN)) error("Expected '(' after function name");

        auto parameters = parseParameterListCallFunction();
        if (!match(TOK_RPAREN)) error("Expected ')' after parameters");

        if(match(TOK_SEMICOLON)) return std::make_unique<FunctionCallNode>(identifier.value, std::move(parameters));
        else error("Expected ';' after return statement");
        return nullptr;
    }

    std::unique_ptr<VariableDeclarationNode> matchVariableDeclaration() {
        if (check(TOK_LET)) consume(TOK_LET);
        else error("Expected 'let'");

        Token identifier, typeToken;
        if (!match(TOK_COLON)) error("Expected ':' after 'let'");
        
        if (check(TOK_TYPE)) typeToken = consume(TOK_TYPE);
        else error("Expected type");

        if (check(TOK_IDENTIFIER)) identifier = consume(TOK_IDENTIFIER);
        else error("Expected identifier");

        if (match(TOK_ASSIGN)) {
            auto initializer = parseBinaryExpression(typeToken);
            if (match(TOK_SEMICOLON)) {
                return std::make_unique<VariableDeclarationNode>(identifier.value, typeToken.value, std::move(initializer));
            } else {
                error("Expected ';' after variable declaration");
            }
        } else if (match(TOK_SEMICOLON)) {
            return std::make_unique<VariableDeclarationNode>(identifier.value, typeToken.value, nullptr);
        } else {
            error("Expected ';' after variable declaration");
        }
        return nullptr;
    }

    std::unique_ptr<ASTNode> parseBinaryExpression(Token type) {
        auto left = parsePrimary(type);

        while (match({TOK_PLUS, TOK_MINUS, TOK_STAR, TOK_SLASH})) {
            std::string operatorToken = tokens[countertoken-1].value;

            if (operatorToken == "*" || operatorToken == "/") {
                auto right = parsePrimary(type);
                left = std::make_unique<BinaryExpressionNode>(std::move(left), operatorToken, std::move(right));
            } else {
                auto right = parsePrimary(type);
                while (match({TOK_STAR, TOK_SLASH})) {
                    std::string nextOperatorToken = tokens[countertoken-1].value;
                    auto nextRight = parsePrimary(type);
                    right = std::make_unique<BinaryExpressionNode>(std::move(right), nextOperatorToken, std::move(nextRight));
                }
                left = std::make_unique<BinaryExpressionNode>(std::move(left), operatorToken, std::move(right));
            }
        }

        return left;
    }

    std::unique_ptr<ASTNode> parsePrimary(const Token &type) {
        if (match(TOK_INTEGER_LITERAL)) {
            return std::make_unique<IntegerLiteralNode>(type.value, atoi(tokens[countertoken-1].value.c_str()));
        }
        if (match(TOK_FLOAT_LITERAL)) {
            return std::make_unique<IntegerLiteralNode>(type.value, atof(tokens[countertoken-1].value.c_str()));
        }
        if (match(TOK_IDENTIFIER)) {
            if(check(TOK_LPAREN)) {
                countertoken--;
                return parserCallFunction();
            }
            return std::make_unique<VariableReferenceNode>(tokens[countertoken-1].value, type.value);
        }
        if (match(TOK_STRING_LITERAL)) {
            return std::make_unique<StringLiteralNode>(tokens[countertoken-1].value, type.value, true);
        }
        if (match(TOK_LPAREN)) {
            auto expr = parseBinaryExpression(type);
            if (!match(TOK_RPAREN)) error("Expected ')' after expression");
            return expr;
        }
        
        error("Expected primary expression");
        return nullptr;
    }

public:
    IsaParser(const std::vector<Token>& tokens, const std::string& filename) 
        : tokens(tokens), filename(filename), errors(tokens) {}

    std::vector<std::unique_ptr<ASTNode>> parseProgram() {
        while (!isAtEnd()) {
            auto exp = matchDeclaration();
            if (exp) program.push_back(std::move(exp));
        }
        return std::move(program);
    }
    
    bool getErr() { return err; }
};

#endif