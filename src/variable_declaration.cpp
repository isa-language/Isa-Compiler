#include "parser_ast.hpp"

std::unique_ptr<ExpType> IsaParser::parserIdentVarDeclType() {
advancedToken();
if(getToken().type == TokenType::TOK_COLON) {
    advancedToken();
    if(getToken().type == TokenType::TOK_TYPE) {
    if (getToken().value == "i8" || getToken().value == "i16" || getToken().value == "i32" || getToken().value == "i64" ) return std::make_unique<ExpType>(Type::NUMBER, modToken(TokenType::TOK_INTEGER_LITERAL));
    else if(getToken().value == "f16" || getToken().value == "f32" || getToken().value == "f64") return std::make_unique<ExpType>(Type::NUMBER, modToken(TokenType::TOK_FLOAT_LITERAL));
    else if(getToken().value == "string") return std::make_unique<ExpType>(Type::STRING,modToken(TokenType::TOK_STRING_LITERAL)); 
    else if(getToken().value == "bool") return std::make_unique<ExpType>(Type::NUMBER, modToken(TokenType::TOK_BOOL_LITERAL));
    
    } else Error(ErrorType::SyntaxError,"Expected 'Type' statement", getToken().line, getToken().column, "source.isa").printError(errors);
} else {
    Error(ErrorType::SyntaxError,"Expected ':' statement", getToken().line, getToken().column, "source.isa").printError(errors);
} 
return nullptr;
}


std::unique_ptr<IdentifierExprAST> IsaParser::IdentifierDecl() {
    advancedToken();
    if(getToken().type == TokenType::TOK_IDENTIFIER) { 
      auto valuedecl = getToken().value; 
      advancedToken();
     if(getToken().type == TOK_ASSIGN) {
        advancedToken();
        if(getToken().type == TokenType::TOK_STRING_LITERAL || getToken().type == TokenType::TOK_INTEGER_LITERAL || getToken().type == TokenType::TOK_BOOL_LITERAL) {
          auto valueVariable = getToken().value;
          advancedToken();
          if(getToken().type == TokenType::TOK_SEMICOLON) return std::make_unique<IdentifierExprAST>(valuedecl, valueVariable);
          else {
            reverseToken();
            Error(ErrorType::SyntaxError, "Unexpected token ';'", getToken().line, getToken().column, "source.isa").printError(errors);
          }
        } else {
            Error(ErrorType::SyntaxError, "Unexpected Value", getToken().line, getToken().column, "source.isa").printError(errors);
        }
        
      } else {
          Error(ErrorType::SyntaxError,"Expected '=' statement", getToken().line, getToken().column, "source.isa").printError(errors);
      }
    } else {
        Error(ErrorType::SyntaxError,"Expected 'var name' statement",getToken().line,getToken().column, "source.isa").printError(errors);
    }
    return nullptr;
  }

  std::unique_ptr<VariableExpAST> IsaParser::VariableDeclarator() {
    auto let = parserIdentVarDeclType();
    auto identifier = IdentifierDecl(); 
    if(let == nullptr || identifier == nullptr) {
      return nullptr;
    }
   return std::make_unique<VariableExpAST>(identifier->name,std::move(let),std::move(identifier));
  }