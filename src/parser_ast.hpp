/**
 * IsaLLVMCompiler -> parser abstract syntax tree 
 * */
#pragma once
// #include <llvm/ADT/STLExtras.h>
#include <iostream>
#include <stdexcept>
#include <string>
#ifndef IsaLLVMParserAST
#define IsaLLVMParserAST
#include "ast.hpp"
#include "token.hpp"
#include <vector>
#include <algorithm>
#include <memory>
#include <utility>

class IsaParser {
  std::unique_ptr<Program> programa;
  std::vector<Token> tokens;
  int countertoken = 0;
  Token getToken() {
    return tokens[countertoken];
  }
  void advancedToken() {
    countertoken++;
  }
  bool match(TokenType type) {
    if(getToken().type == type){
      advancedToken();
      return true;
    }
    return false;
  }
public:
  IsaParser(const std::vector<Token>& tokens) : tokens(tokens) {
    
  }
  
  void parserProgram() {
    while(getToken().type != TokenType::TOK_EOF) {
      if(getToken().type == TokenType::TOK_STRUCT) {
        //programa->addDeclaration(parserStruct());
      } else if(getToken().type == TokenType::TOK_FN) {
        
      } else if(getToken().type == TokenType::TOK_LET) {
        parseVarDeclType();
        advancedToken();
      }
    }
    
  }
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
  std::unique_ptr<ExpType> parseVarDeclType() {
    advancedToken();
    if(getToken().type == TokenType::TOK_COLON) {
      std::cout << "Check 1" << '\n';
      advancedToken();
      if(getToken().type == TokenType::TOK_TYPE) {
        std::cout << "Check 2" << '\n';
        if (getToken().value == "i8" || getToken().value == "i16" || getToken().value == "i32" || getToken().value == "i64" ) return std::make_unique<ExpType>(Type::NUMBER, TokenType::TOK_INTEGER_LITERAL);
        else if(getToken().value == "f16" || getToken().value == "f32" || getToken().value == "f64") return std::make_unique<ExpType>(Type::NUMBER, TokenType::TOK_FLOAT_LITERAL);
        else if(getToken().value == "string") return std::make_unique<ExpType>(Type::STRING,TokenType::TOK_STRING_LITERAL);
        else if(getToken().value == "bool") return std::make_unique<ExpType>(Type::NUMBER, TokenType::TOK_BOOL_LITERAL);
       
      } else throw std::runtime_error("Expected 'Type' statement");
    } else {
      throw std::runtime_error("Expected ':' statement");
    } 
    std::cout << "Check 3" << '\n';
    return nullptr;
  }

};

#endif // !IsaLLVMParserAST

