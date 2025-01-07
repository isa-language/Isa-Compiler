#ifndef IsaLLVM_PARSER
#define IsaLLVM_PARSER

#include <cstdlib>
#include <memory>
#include <string>
#include <iostream>
#include <system_error>
#include <vector>
#include "token.hpp"
#include "ast.hpp"
#include "err.hpp"

class IsaParser {
    std::vector<Token> tokens;
    int countertoken = 0;
    std::string filename;
    std::vector<std::string>& errors;
    std::vector<std::unique_ptr<ASTNode>> program;
    bool err = false;

    void advanceToken() { countertoken++; }
    void returnToken() { countertoken--; }
    bool isAtEnd() const { return getToken().type == TOK_EOF; }

    Token getToken() const { return tokens[countertoken]; }
    const Token& advance() { return tokens[countertoken++]; }
    const Token& previous() { return tokens[countertoken - 1]; }

    Token consume(TokenType type) {
        Token value = getToken();
        advance();
        return value;
    }
    
    bool match(TokenType type) {
        if (check(type)) {
            advance();
            return true;
        }
        return false;
    }

    bool match(const std::initializer_list<TokenType>& types) {
        for (auto type : types) {
            if (check(type)) {
                advance();
                return true;
            }
        }
        return false;
    }

    bool check(TokenType type) const { return !isAtEnd() && getToken().type == type; }

    void expect(TokenType type, const std::string& message) {
        if (!match(type)) 
            error(message);
            //Error(ErrorType::SyntaxError, message, getToken().line, getToken().column, filename).printError(errors);
    }

    void error(const std::string& message) {
        err = true;
        Error(ErrorType::SyntaxError, message, previous().line, previous().column, filename).printError(errors);
    }

    std::string parseIdentifier() {
        if (match(TOK_IDENTIFIER)) {
            return previous().value;
        }
        error("Expected 'var name' statement");
        return "";
    }

    std::string parseType() {
        if (match(TOK_TYPE)) {
            return previous().value;
        }
        error("Expected 'type' statement");
        return "";
    }

    std::unique_ptr<ASTNode> matchDeclaration() {
        switch (getToken().type) {
            case TOK_LET:       return matchVariableDeclaration();
            case TOK_FN:        return parserFunctionDeclaration();
            case TOK_STRUCT:    error("Struct declaration not supported yet"); break;
            case TOK_IF:        error("If statement not supported yet"); break;
            case TOK_WHILE:     error("While statement not supported yet"); break;

            case TOK_EXTERN:    {
                advanceToken();
                return parserFunctionDeclaration(true);
            }
            default:
                error("Invalid declaration.");
        }
        advanceToken(); 
        return nullptr;
    }
    std::unique_ptr<ASTNode> matchDeclaration(Token type) {
        switch (getToken().type) {
            case TOK_LET:           return matchVariableDeclaration();
            case TOK_FN:            return parserFunctionDeclaration();
            case TOK_STRUCT:        error("Struct declaration not supported yet"); break;
            case TOK_IF:            error("If statement not supported yet"); break;
            case TOK_WHILE:         error("While statement not supported yet"); break;
            case TOK_RETURN:        return parserReturn(type);
            case TOK_IDENTIFIER:    return parserCallFunction();
            default:
                error("Invalid declaration.");
        }
        advanceToken();
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
            //std::make_unique<ReturnNode>(std::make_unique<IntegerLiteralNode>("i32", 0));
            return std::make_unique<FunctionNode>(identifier.value, returnType.value, std::move(parameters), std::move(body));
        } else {
            error("Expected ';' or '{' after function declaration");
        }
        return nullptr;
    }

    std::vector<std::unique_ptr<ASTNode>> parseStatementList(Token typeFunction) {
    std::vector<std::unique_ptr<ASTNode>> statements;
    
    while (!check(TokenType::TOK_RBRACE) && !isAtEnd()) {
        if (check(TokenType::TOK_RETURN)) {
            statements.push_back(parserReturn(typeFunction));
            //if (!check(TokenType::TOK_SEMICOLON)) 
              //  error("Expected ';' after return statement");
        } else {
            auto stmt = matchDeclaration(typeFunction);
            if (stmt) statements.push_back(std::move(stmt));
            else advanceToken();
        }
    }
    
    if (!match(TokenType::TOK_RBRACE)) {
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

        if(match(TOK_SEMICOLON)) return std::make_unique<FunctionCallNode>(identifier.value,std::move(parameters));
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
            if (match(TOK_SEMICOLON)) 
                return std::make_unique<VariableDeclarationNode>(identifier.value, typeToken.value, std::move(initializer));
            else error("Expected ';' after variable declaration");
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
            std::string operatorToken = previous().value;

            if (operatorToken == "*" || operatorToken == "/") {
                auto right = parsePrimary(type);
                left = std::make_unique<BinaryExpressionNode>(std::move(left), operatorToken, std::move(right));
            } else {
                auto right = parsePrimary(type);
                while (match({TOK_STAR, TOK_SLASH})) {
                    std::string nextOperatorToken = previous().value;
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
            return std::make_unique<IntegerLiteralNode>(type.value,atoi(previous().value.c_str()));
        }
        if (match(TOK_FLOAT_LITERAL)) {
            return std::make_unique<IntegerLiteralNode>(previous().value);
        }
        if (match(TOK_IDENTIFIER)) {
            if(check(TOK_LPAREN)) {
                returnToken();
                return parserCallFunction();
            }
            else return std::make_unique<VariableReferenceNode>(previous().value,type.value);
        }
        if (match(TOK_STRING_LITERAL)) {
            return std::make_unique<StringLiteralNode>(previous().value, type.value, true);
        }
        if (match(TOK_LPAREN)) {
            auto expr = parseBinaryExpression(type);
            if (!match(TOK_RPAREN)) error("Expected ')' after expression");
            return expr;
        }
        return nullptr;
    }

public:
    IsaParser(const std::vector<Token>& tokens, const std::string& filename, std::vector<std::string>& errors) 
        : tokens(tokens), filename(filename), errors(errors) {}

    std::vector<std::unique_ptr<ASTNode>> parseProgram() {
        while (!isAtEnd()) {
            auto exp = matchDeclaration();
            if (exp) program.push_back(std::move(exp));
            else error("Failed to parse program");
        }
        return std::move(program);
    }
    bool getErr() {return err;}
};

#endif