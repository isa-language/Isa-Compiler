#include "frontend/parser/parserc.hpp"
#include "errors/err.hpp"
#include "frontend/lexer/token.hpp"
#include "frontend/parser/ast.hpp"
#include "frontend/utils/type_render_ast.hpp"
#include <algorithm>
#include <memory>
#include <llvm/CodeGen/TargetFrameLowering.h>

#define MATCH_DECLARATION(TOKEN, FUNC) \
if (match(TOKEN)) { \
    return FUNC(); \
}

#define MATCH_VARIABLE(TOKEN, FUNC, VAR, ERR) \
if (match(TOKEN)) {  \
    (VAR) = FUNC();  \
} else { \
    error(ERR); \
}

void IsaParser::error(const std::string& message) {
    err = true;
    Error(ErrorType::SyntaxError, message, previous().line, previous().column, filename).printError(tokens);
}

Token IsaParser::getToken() const {
    return tokens[countertoken];
};

const Token& IsaParser::advanceToken() {
    if (countertoken < tokens.size()) countertoken++;
    return tokens[countertoken - 1];
}

bool IsaParser::check(TokenType type) const {
    return getToken().type == type;
}

const Token& IsaParser::previous() {
    return tokens[countertoken - 1];
}
void IsaParser::expect(TokenType type, const std::string& errorMessage) {
    if (!match(type)) {
        error(errorMessage);
    }
}

std::unique_ptr<ASTNode> IsaParser::matchDeclaration() {
    MATCH_DECLARATION(TOK_LET, parseVariableDeclaration)
    MATCH_DECLARATION(TOK_FN, parseFunctionDeclaration)
    //MATCH_DECLARATION(TOK_STRUCT, parseStructDeclaration)
    //MATCH_DECLARATION(TOK_IF, parseIfStatement)
    //MATCH_DECLARATION(TOK_WHILE, parseWhileStatement)
    //MATCH_DECLARATION(TOK_FOR, parseForStatement)
    //MATCH_DECLARATION(TOK_SWITCH, parseSwitchStatement)
    MATCH_DECLARATION(TOK_RETURN, parseReturnStatement)

    auto expression = parseBinaryExpression();
    expect(TOK_SEMICOLON, "Esperado ';' após a expressão.");
    return expression;
}


std::vector<std::unique_ptr<ASTNode>> IsaParser::parseProgram() {

    while (!isAtEnd()) {
        auto exp = matchDeclaration();
        if (exp) program.push_back(std::move(exp));
        else error("Falha ao analisar a declaração no programa.");
    }

    return program;
}


/*      Variable    */

std::unique_ptr<VariableDeclarationNode> IsaParser::parseVariableDeclaration() {
    Token varType, varName;
    expect(TOK_LET, "Expected 'let'");
    expect(TOK_COLON,"Expected ':' after 'let'");
    MATCH_VARIABLE(TOK_TYPE, getToken, varType, "Expected type")
    MATCH_VARIABLE(TOK_IDENTIFIER, getToken, varName,"Expected identifier")
    if(check(TOK_SEMICOLON)) CREATE_VAR_NODE(varName.value, varType.value)
    expect(TOK_ASSIGN, "Expected '=' after variable declaration");
    std::unique_ptr<ASTNode> inicialize = parseExpression();
    CREATE_VAR_DECL_NODE(varName.value, varType.value, inicialize)
    return nullptr;
}

void createFDS() {
    Token varType;
} 