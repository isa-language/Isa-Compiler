#include "ast.hpp"
#include "err.hpp"
#include "parser_ast.hpp"
#include "token.hpp"
#include <iostream>
#include <iterator>
#include <memory>
#include <string>
#include <utility>
#include <vector>

std::unique_ptr<ExpType> IsaParser::parserIdentVarDeclType() {
advancedToken();
if(getToken().type == TokenType::TOK_COLON) {
    advancedToken();
    if(getToken().type == TokenType::TOK_TYPE) {
    if (getToken().value == "i8" || getToken().value == "i16" || getToken().value == "i32" || getToken().value == "i64" || getToken().value == "u8" || getToken().value == "u16" || getToken().value == "u32" || getToken().value == "u64") return std::make_unique<ExpType>(Type::NUMBER, modToken(TokenType::TOK_INTEGER_LITERAL));
    else if(getToken().value == "f16" || getToken().value == "f32" || getToken().value == "f64") return std::make_unique<ExpType>(Type::NUMBER, modToken(TokenType::TOK_FLOAT_LITERAL));
    else if(getToken().value == "string") return std::make_unique<ExpType>(Type::STRING,modToken(TokenType::TOK_STRING_LITERAL)); 
    else if(getToken().value == "bool") return std::make_unique<ExpType>(Type::NUMBER, modToken(TokenType::TOK_BOOL_LITERAL));
    else if(getToken().value == "array") return std::make_unique<ExpType>(Type::ARRAY, modToken(TokenType::TOK_ARRAY));
    } else {
        Error(ErrorType::SyntaxError,"Expected 'Type' statement", getToken().line, getToken().column, "source.isa").printError(errors);
    }
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
        if(getToken().type == TokenType::TOK_STRING_LITERAL || getToken().type == TokenType::TOK_INTEGER_LITERAL || getToken().type == TokenType::TOK_BOOL_LITERAL || getToken().type == TokenType::TOK_FLOAT_LITERAL) {
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

  std::unique_ptr<ArrayDeclarationNode> IsaParser::arrayDeclarationNode() {
    advancedToken();
    if(getToken().type == TokenType::TOK_LBRACKET) {
        advancedToken();
        if(getToken().type == TokenType::TOK_TYPE) {
          if (getToken().value == "i8" || getToken().value == "i16" || getToken().value == "i32" || getToken().value == "i64" || getToken().value == "u8" || getToken().value == "u16" || getToken().value == "u32" || getToken().value == "u64") {
            auto valuer = std::make_unique<ExpType>(Type::NUMBER, modToken(TokenType::TOK_INTEGER_LITERAL));
            advancedToken();
            if(getToken().type == TokenType::TOK_COMMA) {
                advancedToken();
                
            } else if (getToken().type == TokenType::TOK_RBRACKET) {
                advancedToken();
                if(getToken().type == TokenType::TOK_IDENTIFIER) {
                    std::string valuedecl = getToken().value; 
                    advancedToken();
                    if(getToken().type == TokenType::TOK_ASSIGN) {
                        advancedToken();
                        if (getToken().type == TokenType::TOK_LBRACE) {
                            std::vector<std::unique_ptr<IdentifierExprAST>> params;
                            advancedToken();
                            while (getToken().type != TokenType::TOK_SEMICOLON) {
                                
                                if(getToken().type == TokenType::TOK_IDENTIFIER || getToken().type == TokenType::TOK_BOOL_LITERAL || getToken().type == TokenType::TOK_INTEGER_LITERAL || getToken().type == TokenType::TOK_STRING_LITERAL || getToken().type == TokenType::TOK_FLOAT_LITERAL) {
                                    params.push_back(std::make_unique<IdentifierExprAST>(valuer->token.value,getToken().value));
                                    advancedToken();
                                    if(getToken().type == TokenType::TOK_COMMA) {
                                        advancedToken();
                                    } else if(getToken().type == TokenType::TOK_RBRACE) {
                                        advancedToken();
                                    }else {
                                        Error(ErrorType::SyntaxError,"Expected '}' statement",getToken().line,getToken().column, "source.isa").printError(errors);
                                    }
                                }
                                
                            }
                            return  std::make_unique<ArrayDeclarationNode>(valuedecl,std::move(valuer),std::move(params));
                        }
                    } else if(getToken().type == TokenType::TOK_SEMICOLON) {
                        return std::make_unique<ArrayDeclarationNode>(valuedecl,std::move(valuer));
                    }
                }
            }
          }
          else if(getToken().value == "f16" || getToken().value == "f32" || getToken().value == "f64") {
            auto valuer = std::make_unique<ExpType>(Type::NUMBER, modToken(TokenType::TOK_FLOAT_LITERAL));
          }
          else if(getToken().value == "string") {
            auto valuer = std::make_unique<ExpType>(Type::STRING,modToken(TokenType::TOK_STRING_LITERAL)); 
          }
          else if(getToken().value == "bool") {
             auto valuer = std::make_unique<ExpType>(Type::NUMBER, modToken(TokenType::TOK_BOOL_LITERAL));
          }
        }
    } else {
        Error(ErrorType::SyntaxError,"Expected '[' statement",getToken().line,getToken().column, "source.isa").printError(errors);
    }
    return nullptr;
  }

  std::unique_ptr<VariableExpAST> IsaParser::VariableDeclarator() {
    auto let = parserIdentVarDeclType();
    if(let->type == Type::ARRAY) {
        auto array = arrayDeclarationNode();
        if(let == nullptr || array == nullptr) {
            return nullptr;
        }
        return std::make_unique<VariableExpAST>(array->name,std::move(let),std::move(array));
        
    } else {
        auto identifier = IdentifierDecl(); 
        if(let == nullptr || identifier == nullptr) {
            return nullptr;
        }
        return std::make_unique<VariableExpAST>(identifier->name,std::move(let),std::move(identifier));
    }
    
    
   
  }