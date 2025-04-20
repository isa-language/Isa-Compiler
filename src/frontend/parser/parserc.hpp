#ifndef IsaLLVM_PARSER
#define IsaLLVM_PARSER

#include <cstdlib>
#include <memory>
#include <string>
#include <vector>
#include "frontend/lexer/token.hpp"
#include "frontend/parser/ast.hpp"

class IsaParser {
private:
    std::vector<Token> tokens;
    int countertoken = 0;
    std::string filename;
    std::vector<std::unique_ptr<ASTNode>> program;
    bool err = false;

    void advance();
    void returnToken();
    bool isAtEnd() const;
    Token getToken() const;
    const Token& advanceToken();
    const Token& previous();

    Token consume(TokenType type);
    bool match(TokenType type);
    bool match(const std::initializer_list<TokenType>& types);
    bool check(TokenType type) const;
    void expect(TokenType type, const std::string& message);

    void error(const std::string& message);

    std::string parseIdentifier();
    std::string parseType();
    std::unique_ptr<ASTNode> matchDeclaration();
    std::unique_ptr<ASTNode> matchDeclaration(Token type);

    std::unique_ptr<ASTNode> parseFunctionDeclaration(bool externFunc = false);
    std::vector<std::unique_ptr<ASTNode>> parseStatementList(Token typeFunction);
    std::unique_ptr<ReturnNode> parseReturnStatement();
    
    std::vector<std::unique_ptr<VariableDeclarationNode>> parseParameterList();
    std::vector<std::unique_ptr<ASTNode>> parseParameterListCallFunction();

    std::unique_ptr<FunctionCallNode> parserCallFunction();
    std::unique_ptr<ASTNode> parseExpression();
    std::unique_ptr<VariableDeclarationNode> parseVariableDeclaration();
    std::unique_ptr<ASTNode> parseBinaryExpression();
    std::unique_ptr<ASTNode> parsePrimary(const Token& type);


public:
    IsaParser(const std::vector<Token> tokens, const std::string& filename)
        : tokens(tokens), filename(filename) {}

    std::vector<std::unique_ptr<ASTNode>> parseProgram();
    bool getErr();
};

#endif
