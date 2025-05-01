#ifndef IsaLLVM_PARSER
#define IsaLLVM_PARSER

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>

#include <vector>
#include "frontend/lexer/token.hpp"
#include "frontend/parser/ast.hpp"
#include "errors/err.hpp"


class SymbolTable {
    std::unordered_map<std::string, std::string> symbols;
public:
    void addSymbol(const std::string& name, const std::string& type) {
        symbols[name] = type;
    }
    
    std::string getType(const std::string& name) {
        return symbols.count(name) ? symbols[name] : "";
    }
    bool symbolExists(const std::string& name) const {
        return symbols.find(name) != symbols.end();
    }
    
    std::string getSymbolType(const std::string& name) const {
        auto it = symbols.find(name);
        if (it != symbols.end()) {
            return it->second;
        }
        return "";
    }
};


class IsaParser {
    std::vector<Token> tokens;
    int countertoken = 0;
    std::string filename;
    SymbolTable symbolTable;
    std::vector<Token> errors;
    std::vector<std::unique_ptr<ASTNode>> program;
    bool err = false;

    inline const Token& currentToken() const {
        if (countertoken >= tokens.size()) return tokens.back();
        return tokens[countertoken];
    }

    inline const Token& previous() const { 
        if (countertoken == 0) return tokens.front();
        return tokens[countertoken - 1]; 
    }

    bool isAtEnd() const { 
        return currentToken().type == TOK_EOF || countertoken >= tokens.size();
    }

    inline const Token& peek(int offset = 1) const {
        int idx = countertoken + offset;
        if (idx >= tokens.size() || idx < 0) return tokens.back();
        return tokens[idx];
    }
    
    inline bool match(enum TokenType type) {
        if (!isAtEnd() && currentToken().type == type) {
            countertoken++;
            return true;
        }
        return false;
    }

    inline bool match(const std::initializer_list<enum TokenType>& types) {
        for (auto type : types) {
            if (!isAtEnd() && currentToken().type == type) {
                countertoken++;
                return true;
            }
        }
        return false;
    }

    inline bool check(enum TokenType type) const { 
        return !isAtEnd() && currentToken().type == type; 
    }

    inline Token consume(enum TokenType type) {
        if (!check(type)) {
            error("Unexpected token");
            return Token{TOK_ERROR, "", currentToken().line, currentToken().column};
        }
        Token value = currentToken();
        countertoken++;
        return value;
    }

    inline Token consumeRever(enum TokenType type) {
        if (!check(type)) {
            error("Unexpected token");
            return Token{TOK_ERROR, "", currentToken().line, currentToken().column};
        }
        Token value = currentToken();
        return value;
    }

    inline void expect(enum TokenType type, const std::string& message) {
        if (!match(type)) {
            error(message);
        }
    }

    inline void error(const std::string& message) {
        err = true;
        Error(ErrorType::SyntaxError, message, currentToken().line, currentToken().column, filename).printError(errors);
    }

    inline std::string parseIdentifier() {
        if (match(TOK_IDENTIFIER)) {
            return tokens[countertoken-1].value;
        }
        error("Expected 'var name' statement");
        return "";
    }

    inline std::string parseType() {
        if (match(TOK_TYPE)) {
            return tokens[countertoken-1].value;
        }
        error("Expected 'type' statement");
        return "";
    }

    inline std::unique_ptr<ASTNode> matchDeclaration(Token type = Token{TOK_ERROR, "", 0, 0}) {
        if (isAtEnd()) return nullptr;
    
        const bool hasType = (type.type != TOK_ERROR);
        
        switch (currentToken().type) {
            case TOK_LET:    return matchVariableDeclaration();
            case TOK_FN:     return parserFunctionDeclaration();
            case TOK_STRUCT: error("Struct declaration not supported yet"); break;
            case TOK_IF:    error("If statement not supported yet"); break;
            case TOK_WHILE:  return parserWhileStatement();
            case TOK_EXTERN: {
                countertoken++;
                return parserFunctionDeclaration(true);
            }
            case TOK_RETURN: {
                if (hasType) return parserReturn(type);
                error("Return statement outside function");
                break;
            }
            case TOK_IDENTIFIER: {
                if (peek().type == TOK_ASSIGN) {
                    return parserAssignmentStatement();
                }
                return parseFunctionCall();
            }
            default:
                error("Invalid declaration");
        }
        
        countertoken++;
        return nullptr;
    }

    inline std::unique_ptr<ASTNode> parserFunctionDeclaration(bool externFunc = false) {
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

    inline std::vector<std::unique_ptr<ASTNode>> parseStatementList(Token typeFunction) {
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

    inline std::unique_ptr<ASTNode> parserAssignmentStatement() {
        Token name = consume(TOK_IDENTIFIER);
        if (!symbolTable.symbolExists(name.value)) {
            error("Variable '" + name.value + "' not declared");
            return nullptr;
        }
        if (!match(TOK_ASSIGN)) {
            error("Expected '=' in assignment");
            return nullptr;
        }
        std::string varType = symbolTable.getSymbolType(name.value);
        auto value = tokens[countertoken];
        value.value = varType;
        auto expr = parseBinaryExpression(value, true);
        if (!match(TOK_SEMICOLON)) {
            error("Expected ';' after assignment");
        }
        return std::make_unique<AssignmentNode>(name.value, std::move(expr));
    }
    
    inline std::vector<std::unique_ptr<ASTNode>> parseStatementList() {
        std::vector<std::unique_ptr<ASTNode>> statements;
        
        while (!check(TOK_RBRACE) && !isAtEnd()) {
            auto stmt = matchDeclaration();
            if (stmt) statements.push_back(std::move(stmt));
        }
        
        if (!match(TOK_RBRACE)) {
            error("Expected '}' at the end of function");
        }
        
        return statements;
    }
    
    inline std::unique_ptr<ReturnNode> parserReturn(Token type) {
        if (check(TOK_RETURN)) consume(TOK_RETURN);
        auto value = parseBinaryExpression(type);
        if(match(TOK_SEMICOLON)) return std::make_unique<ReturnNode>(std::move(value));
        else error("Expected ';' after return statement");
        return nullptr;
    }

    inline std::vector<std::unique_ptr<VariableDeclarationNode>> parseParameterList() {
        std::vector<std::unique_ptr<VariableDeclarationNode>> parameters;
        if (check(TOK_RPAREN)) return parameters;
        
        do {
            Token identifier, type;
            if (check(TOK_IDENTIFIER)) identifier = consume(TOK_IDENTIFIER);
            else error("Expected identifier");

            if (!match(TOK_COLON)) error("Expected ':' after identifier");
            
            if (check(TOK_TYPE)) type = consume(TOK_TYPE);
            else error("Expected type after 'type'");
            parameters.push_back(std::make_unique<VariableDeclarationNode>(identifier.value, type.value));
        } while (match({TOK_COMMA}));
        
        return parameters;
    }

    inline std::vector<std::unique_ptr<ASTNode>> parseParameterListCallFunction() {
        std::vector<std::unique_ptr<ASTNode>> parameters;
        if (check(TOK_RPAREN)) return parameters;
        
        do {
            Token identifier;
            if (match(TOK_STAR)) {
                if(check(TOK_IDENTIFIER)) {
                    identifier = consume(TOK_IDENTIFIER);
                    parameters.push_back(std::make_unique<VariableReferenceNode>(identifier.value));
                }
                else error("Expected identifier");
            } else if (check(TOK_IDENTIFIER)) {
                identifier = consume(TOK_IDENTIFIER);
                parameters.push_back(std::make_unique<VariableValueNode>(identifier.value));
            } else if (check(TOK_STRING_LITERAL)) {
                identifier = consume(TOK_STRING_LITERAL);
                parameters.push_back(std::make_unique<StringLiteralNode>(identifier.value));
            } else if (check(TOK_INTEGER_LITERAL)) {
                identifier = consume(TOK_INTEGER_LITERAL);
                countertoken--;
                auto initializer = parseBinaryExpression(identifier, true);
                parameters.push_back(std::move(initializer));
            }
            else error("Expected identifier");
            
        } while (match(TOK_COMMA));
        
        return parameters;
    }

    inline std::unique_ptr<FunctionCallNode> parseFunctionCall(bool expectSemicolon = true) {
        Token identifier;
        if      (check(TOK_IDENTIFIER))    identifier = consume(TOK_IDENTIFIER);
        else if (check(TOK_STRING_LITERAL)) identifier = consume(TOK_STRING_LITERAL);
        else {
            error("Expected function name");
            return nullptr;
        }

        if (!match(TOK_LPAREN)) {
            error("Expected '(' after function name");
            return nullptr;
        }

        auto args = parseParameterListCallFunction();
   
        if (!match(TOK_RPAREN)) {
            error("Expected ')' after parameters");
            return nullptr;
        }

        if (expectSemicolon) {
            if (!match(TOK_SEMICOLON)) {
                error("Expected ';' after function call");
            }
        }
    
        return std::make_unique<FunctionCallNode>(identifier.value, std::move(args));
    }

    inline std::unique_ptr<VariableDeclarationNode> matchVariableDeclaration() {
        if (check(TOK_LET)) consume(TOK_LET);
        else error("Expected 'let'");

        Token identifier, typeToken;
        if (!match(TOK_COLON)) error("Expected ':' after 'let'");
        
        if (check(TOK_TYPE)) typeToken = consume(TOK_TYPE);
        else error("Expected type");

        if (check(TOK_IDENTIFIER)) identifier = consume(TOK_IDENTIFIER);
        else error("Expected identifier");

        if (match(TOK_ASSIGN)) {
            auto initializer = parseBinaryExpression(typeToken, true);
            symbolTable.addSymbol(identifier.value, typeToken.value);
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

    inline std::unique_ptr<ASTNode> parserWhileStatement() {
        if (!match(TOK_WHILE)) {
            error("Expected 'while'");
            return nullptr;
        }
        
        if (!match(TOK_LPAREN)) {
            error("Expected '(' after 'while'");
            return nullptr;
        }

        if (isAtEnd()) {
            error("Unexpected end of input in while condition");
            return nullptr;
        }
    
        auto token = currentToken();
        auto condition = parseBinaryExpression(token);
        if (!condition) {
            error("Invalid while condition");
            return nullptr;
        }
        
        if (!match(TOK_RPAREN)) {
            error("Expected ')' after while condition");
            return nullptr;
        }
        
        if (!match(TOK_LBRACE)) {
            error("Expected '{' after while condition");
            return nullptr;
        }
    
        auto bodyStmts = parseStatementList();
    
        return std::make_unique<WhileNode>(std::move(condition), std::make_unique<BlockNode>(std::move(bodyStmts)));
    }

    inline std::unique_ptr<ASTNode> parseBinaryExpression(Token type, bool default_ = false) {
        auto left = parsePrimary(type, default_);

        while (match({TOK_PLUS, TOK_MINUS, TOK_STAR, TOK_SLASH, TOK_PERCENT, TOK_STARSTAR, TOK_PLUSPLUS, TOK_MINUSMINUS})) {
            std::string operatorToken = tokens[countertoken-1].value;

            if (operatorToken == "=") {
                auto right = parseBinaryExpression(type, default_);
                left = std::make_unique<BinaryExpressionNode>(std::move(left), operatorToken, std::move(right));
                continue;
            }
    
            if (operatorToken == "**") {
                auto right = parseBinaryExpression(type, default_);
                left = std::make_unique<BinaryExpressionNode>(std::move(left), operatorToken, std::move(right));
                continue;
            }

            if (operatorToken == "++" || operatorToken == "--") {
                left = std::make_unique<BinaryExpressionNode>(std::move(left), operatorToken, nullptr);
                continue;
            }
            if (operatorToken == "*" || operatorToken == "/" || operatorToken == "%") {
                auto right = parsePrimary(type, default_);
                left = std::make_unique<BinaryExpressionNode>(std::move(left), operatorToken, std::move(right));
            } else {
                auto right = parsePrimary(type, default_);
                while (match({TOK_STAR, TOK_SLASH, TOK_PERCENT, TOK_STARSTAR})) {
                    std::string nextOperatorToken = tokens[countertoken-1].value;
                    if (nextOperatorToken == "**") {
 
                        auto exponent = parseBinaryExpression(type, default_);
                        right = std::make_unique<BinaryExpressionNode>(std::move(right), nextOperatorToken, std::move(exponent));
                    } else {
                        auto nextRight = parsePrimary(type, default_);
                        right = std::make_unique<BinaryExpressionNode>(std::move(right), nextOperatorToken, std::move(nextRight));
                    }
                }
                left = std::make_unique<BinaryExpressionNode>(std::move(left), operatorToken, std::move(right));
            }
        }

        return left;
    }

    inline std::unique_ptr<ASTNode> parsePrimary(const Token &type, bool default_) {
        bool isNegative = false;
        if (match(TOK_MINUS)) {
            if (check(TOK_INTEGER_LITERAL) || check(TOK_FLOAT_LITERAL)) {
                isNegative = true;
            } else {
                countertoken--; 
            }
        }

        if (match({TOK_PLUSPLUS, TOK_MINUSMINUS})) {
            std::string op = tokens[countertoken-1].value;
            if (check(TOK_IDENTIFIER)) {
                Token ident = consume(TOK_IDENTIFIER);
                auto varRef = std::make_unique<VariableReferenceNode>(ident.value, type.value);
                return std::make_unique<BinaryExpressionNode>(std::move(varRef), op, nullptr);
            } else {
                error("Expected identifier after " + op + " operator");
            }
        }

        if (match(TOK_INTEGER_LITERAL)) {
            int value = atoi(tokens[countertoken - 1].value.c_str());
            if (isNegative) value = -value;
            return std::make_unique<IntegerLiteralNode>((default_) ? "i32" : type.value, value);
        }
        if (match(TOK_FLOAT_LITERAL)) {
            double value = atof(tokens[countertoken - 1].value.c_str());
            if (isNegative) value = -value;
            return std::make_unique<IntegerLiteralNode>(type.value, value);
        }
        if (check(TOK_IDENTIFIER)) {
            if (peek().type == TOK_LPAREN) {
                auto call = parseFunctionCall(false);
                if (call) return std::move(call);
            } else {
                Token ident = consume(TOK_IDENTIFIER);

                if (match({TOK_PLUSPLUS, TOK_MINUSMINUS})) {
                    std::string op = tokens[countertoken-1].value;
                    auto varRef = std::make_unique<VariableReferenceNode>(ident.value, type.value);
                    return std::make_unique<BinaryExpressionNode>(std::move(varRef), op, nullptr);
                }
                
                return std::make_unique<VariableReferenceNode>(ident.value, type.value);
            }
        }

        if (match(TOK_STAR)) {
            if (check(TOK_IDENTIFIER)) {
                return std::make_unique<VariableReferenceNode>(tokens[countertoken].value);
            } else {
                error("Expected identifier");
            }
        }
        if (match(TOK_STRING_LITERAL)) {
            return std::make_unique<StringLiteralNode>(tokens[countertoken - 1].value, type.value, true);
        }
        if (match(TOK_BOOL_LITERAL)) {
            if (tokens[countertoken - 1].value == "true") {
                return std::make_unique<BoolLiteralNode>(true, (default_) ? "bool" : type.value);
            }
            if (tokens[countertoken - 1].value == "false") {
                return std::make_unique<BoolLiteralNode>(false, (default_) ? "bool" : type.value);
            }
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

    inline std::vector<std::unique_ptr<ASTNode>> parseProgram() {
        while (!isAtEnd()) {
            auto exp = matchDeclaration();
            if (exp) program.push_back(std::move(exp));
        }
        return std::move(program);
    }
    
    bool hasError() { return err; }
};

#endif