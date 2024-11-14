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
            default:
                error("Invalid declaration.");
        }
        advanceToken(); 
        return nullptr;
    }
    std::unique_ptr<ASTNode> matchDeclaration(Token type) {
        switch (getToken().type) {
            case TOK_LET:       return matchVariableDeclaration();
            case TOK_FN:        return parserFunctionDeclaration();
            case TOK_STRUCT:    error("Struct declaration not supported yet"); break;
            case TOK_IF:        error("If statement not supported yet"); break;
            case TOK_WHILE:     error("While statement not supported yet"); break;
            case TOK_RETURN:    return parserReturn(type);
            default:
                error("Invalid declaration.");
        }
        advanceToken();
        return nullptr;
    }


    std::unique_ptr<ASTNode> parserFunctionDeclaration() {
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
            return std::make_unique<FunctionInstantiationNode>(identifier.value, returnType.value, std::move(parameters));
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
            std::cout << identifier.value;
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
            return std::make_unique<VariableReferenceNode>(previous().value,type.value);
        }
        if (match(TOK_STRING_LITERAL)) {
            return std::make_unique<StringLiteralNode>(previous().value, type.value, true);
        }
        if (match(TOK_LPAREN)) {
            auto expr = parseBinaryExpression(type);
            if (!match(TOK_RPAREN)) error("Expected ')' after expression");
            return expr;
        }
        throw std::runtime_error("Unexpected expression");
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


/*
    std::unique_ptr<VariableDeclarationNode> matchVariableDeclaration() {
        if (match(TOK_LET)) {
            if (match(TOK_COLON)) {
                if (matchType()) {
                    auto type = getToken().value;
                    if (match(TOK_IDENTIFIER)) {
                        auto identifier = getToken().value;
                        if (match(TOK_ASSIGN)) {
                            if(matchExpression()){
                                
                            }
                            else if (match(TOK_SEMICOLON)) {
                                // return std::make_unique<VariableDeclarationNode>(...);
                                //return true;
                            } else {
                                Error(ErrorType::SyntaxError,"Expected 'var name' statement",getToken().line,getToken().column, filename).printError(errors);
                            }
                        } else if (match(TOK_SEMICOLON)) {
                            // return std::make_unique<VariableDeclarationNode>(...);
                            //return true;
                        } else {
                            Error(ErrorType::SyntaxError,"Expected ';' statement",getToken().line,getToken().column, filename).printError(errors);
                        }
                    } else {
                        Error(ErrorType::SyntaxError,"Expected 'var name' statement",getToken().line,getToken().column, filename).printError(errors);
                    }
                } else {
                    Error(ErrorType::SyntaxError,"Expected 'type' statement",getToken().line,getToken().column, filename).printError(errors);
                }
            } else {
                Error(ErrorType::SyntaxError,"Expected ':' statement",getToken().line,getToken().column, filename).printError(errors);
            }
        } else if(match(TOK_IDENTIFIER)) {
            Error(ErrorType::SyntaxError,"Expected 'var name' statement",getToken().line,getToken().column, filename).printError(errors);
        }
        return nullptr;
    }

     std::unique_ptr<FunctionNode> matchFunctionDeclaration() {
        if (match(TOK_FN)) {
            if (match(TOK_IDENTIFIER)) {
                if (match(TOK_LPAREN)) {
                    matchParameterList();
                    if (match(TOK_RPAREN)) {
                        if (match(TOK_ARROW)) {
                            if (matchType()) {
                                if (match(TOK_LBRACE)) {
                                    // parseStatementList();
                                    if (match(TOK_RBRACE)) {
                                        // return std::make_unique<FunctionNode>(...);
                                        //return true;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        //return false;
    }

     std::unique_ptr<StructDeclarationNode> matchStructDeclaration() {
        if (match(TOK_STRUCT)) {
            if (match(TOK_IDENTIFIER)) {
                if (match(TOK_LBRACE)) {
                    matchStructMemberList();
                    if (match(TOK_RBRACE)) {
                        matchConstructor();
                        // return std::make_unique<StructDeclarationNode>(...);
                        return nullptr;
                    }
                }
            }
        }
        return nullptr;
    }

    bool matchType() {
        return match(TOK_TYPE);
    }

    void matchParameterList() {
        if (matchParameter()) {
            while (match(TOK_COMMA)) {
                matchParameter();
            }
        }
    }

    bool matchParameter() {
        if (matchType()) {
            return match(TOK_IDENTIFIER);
        }
        return false;
    }

    void matchStructMemberList() {
        while (matchStructMember()) {}
    }

    bool matchStructMember() {
        if (match(TOK_LET)) {
            if (match(TOK_COLON)) {
                if (matchType()) {
                    if (match(TOK_IDENTIFIER)) {
                        if (match(TOK_SEMICOLON)) {
                            // return std::make_unique<StructMemberNode>(...);
                            return true;
                        }
                    }
                }
            }
        }
        return false;
    }

    bool matchConstructor() {
        if (match(TOK_FN)) {
            if (match(TOK_IDENTIFIER)) {
                if (match(TOK_LPAREN)) {
                    matchParameterList();
                    if (match(TOK_RPAREN)) {
                        if (match(TOK_LBRACE)) {
                            // parseStatementList();
                            if (match(TOK_RBRACE)) {
                                // return std::make_unique<ConstructorNode>(...);
                                return true;
                            }
                        }
                    }
                }
            }
        }
        return false;
    }

    bool matchExpression() {
        return matchLiteral() || matchIdentifier() || matchBinaryExpression() || matchParenthesizedExpression()
            || matchArrayExpression() || matchStructExpression() || matchFunctionCall()
            || matchPointerExpression() || matchReferenceExpression();
    }

    bool matchLiteral() {
        return match(TOK_INTEGER_LITERAL) || match(TOK_FLOAT_LITERAL) || match(TOK_BOOL_LITERAL) || match(TOK_FLOAT_LITERAL);
    }

    bool matchIdentifier() {
        return match(TOK_IDENTIFIER);
    }

    bool matchBinaryExpression() {
        if (matchExpression()) {
            if (matchBinaryOperator()) {
                return matchExpression();
            }
        }
        return false;
    }

    bool matchBinaryOperator() {
        return match(TOK_PLUS) || match(TOK_MINUS) || match(TOK_STAR) || match(TOK_SLASH) 
            || match(TOK_EQUAL) || match(TOK_NOT_EQUAL) || match(TOK_LT) || match(TOK_GT)
            || match(TOK_LE) || match(TOK_GE);
    }

    bool matchParenthesizedExpression() {
        if (match(TOK_LPAREN)) {
            if (matchExpression()) {
                return match(TOK_RPAREN);
            }
        }
        return false;
    }

    bool matchArrayExpression() {
        if (match(TOK_LBRACE)) {
            matchExpressionList();
            return match(TOK_RBRACE);
        }
        return false;
    }

    void matchExpressionList() {
        if (matchExpression()) {
            while (match(TOK_COMMA)) {
                matchExpression();
            }
        }
    }

    bool matchStructExpression() {
        if (match(TOK_IDENTIFIER)) {
            if (match(TOK_LBRACE)) {
                matchStructFieldList();
                return match(TOK_RBRACE);
            }
        }
        return false;
    }

    void matchStructFieldList() {
        if (matchStructField()) {
            while (match(TOK_COMMA)) {
                matchStructField();
            }
        }
    }

    bool matchStructField() {
        if (match(TOK_IDENTIFIER)) {
            if (match(TOK_ASSIGN)) {
                return matchExpression();
            }
        }
        return false;
    }

    bool matchFunctionCall() {
        if (match(TOK_IDENTIFIER)) {
            if (match(TOK_LPAREN)) {
                matchArgumentList();
                return match(TOK_RPAREN);
            }
        }
        return false;
    }

    void matchArgumentList() {
        if (matchExpression()) {
            while (match(TOK_COMMA)) {
                matchExpression();
            }
        }
    }

    bool matchPointerExpression() {
        if (match(TOK_NEW)) {
            if (matchType()) {
                if (match(TOK_LPAREN)) {
                    matchExpression();
                    return match(TOK_RPAREN);
                }
            }
        }
        return false;
    }

    bool matchReferenceExpression() {
        if (match(TOK_AMP)) {
            return match(TOK_IDENTIFIER);
        }
        return false;
    }
*/