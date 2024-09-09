/**
 * IsaLLVMCompiler -> parser abstract syntax tree 
 * */
#pragma once
#include <cstdlib>
#ifndef IsaLLVMParserAST
#define IsaLLVMParserAST
#include <llvm/ADT/STLExtras.h>
#include "ast.hpp"
#include "token.hpp"
#include "err.hpp"
#include <vector>
#include <algorithm>
#include <memory>
#include <utility>
#include <iostream>
#include <stdexcept>
#include <string>

class IsaParser {
  std::unique_ptr<Program> programa;
  std::vector<Token> tokens;
  std::vector<std::string> errors;
  int countertoken = 0;
  Token getToken() {
    return tokens[countertoken];
  }
  void advancedToken() {
    countertoken++;
  }
  Token getReverseToken() {
    return tokens[countertoken-1];
  }
  bool match(TokenType type) {
    if(getToken().type == type){
      advancedToken();
      return true;
    }
    return false;
  }
  Token modToken(TokenType token) {
    tokens[countertoken].type = token;
    return tokens[countertoken];
  }
public:
  IsaParser(const std::vector<Token>& tokens, const std::vector<std::string> err) : tokens(tokens) , errors(err) {}
  
  void parserProgram() {
    while(getToken().type != TokenType::TOK_EOF) {
      if(getToken().type == TokenType::TOK_STRUCT) {
        //programa->addDeclaration(parserStruct());
      } else if(getToken().type == TokenType::TOK_FN) {
        
      } else if(getToken().type == TokenType::TOK_LET) {
         //parserIdentVarDeclType();
        auto let = VariableDeclarator();
        //programa->addDeclaration(VariableDeclarator());
        //std::cout << "variavel declarete " << let->identifier->value << " type " << let->type->token.value << '\n';
        advancedToken();
      }
      // advancedToken();
    }
    
  }

  

  std::unique_ptr<ExpType> parserIdentVarDeclType() {
    advancedToken();
    if(getToken().type == TokenType::TOK_COLON) {
      advancedToken();
      if(getToken().type == TokenType::TOK_TYPE) {
        if (getToken().value == "i8" || getToken().value == "i16" || getToken().value == "i32" || getToken().value == "i64" ) return std::make_unique<ExpType>(Type::NUMBER, modToken(TokenType::TOK_INTEGER_LITERAL));
        else if(getToken().value == "f16" || getToken().value == "f32" || getToken().value == "f64") return std::make_unique<ExpType>(Type::NUMBER, modToken(TokenType::TOK_FLOAT_LITERAL));
        else if(getToken().value == "string") return std::make_unique<ExpType>(Type::STRING,modToken(TokenType::TOK_STRING_LITERAL)); 
        else if(getToken().value == "bool") return std::make_unique<ExpType>(Type::NUMBER, modToken(TokenType::TOK_BOOL_LITERAL));
       
      } else throw std::runtime_error("Expected 'Type' statement");
    } else {
      throw std::runtime_error("Expected ':' statement");
    } 
    return nullptr;
  }


  std::unique_ptr<IdentifierExprAST> IdentifierDecl() {
    advancedToken();
    if(getToken().type == TokenType::TOK_IDENTIFIER) { 
      auto valuedecl = getToken().value; 
      advancedToken();
     if(getToken().type == TOK_ASSIGN) {
        advancedToken();
        if(getToken().type == TokenType::TOK_STRING_LITERAL || getToken().type == TokenType::TOK_INTEGER_LITERAL) {
          advancedToken();
          if(getToken().type == TokenType::TOK_SEMICOLON) return std::make_unique<IdentifierExprAST>(valuedecl);
          else {
            
            Error(ErrorType::SyntaxError, "Unexpected token ';'", getReverseToken().line, getReverseToken().column, "source.isa").printError(errors);
          }
        }
        
      } else {
        Error(ErrorType::SyntaxError,"Expected '=' statement", getToken().line, getToken().column, "source.isa").printError(errors);
      }
    } else {
      std::cout << "Expected 'var name' statement" << '\n';
    }
    return nullptr;
  }

  std::unique_ptr<VariableExpAST> VariableDeclarator() {
    auto let = parserIdentVarDeclType();
    auto identifier = IdentifierDecl(); 
    return std::make_unique<VariableExpAST>(std::move(let),std::move(identifier));
  }
};

  


  /*
  ASTNodePtr parserStruct() {
    advancedToken();
    std::string name;
    if(getToken().type == TokenType::TOK_STRING_LITERAL) {
      name = getToken().value;
    }
    advancedToken();
    std::unique_ptr<> members;
    if(match(TokenType::TOK_LBRACE)) {
      while (!match(TokenType::TOK_RBRACE)) {
            if (getToken().type == TokenType::TOK_LET) {
                programa->addDeclaration(parseVarDecl());
            } else if (getToken().type == TokenType::TOK_FN) {
                programa->addDeclaration(parseFuncDecl())
            } else {
                throw std::runtime_error("Unexpected token in struct");
            }
        }
    }
  }
  */


#endif // !IsaLLVMParserAST

