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
  
  std::vector<Token> tokens;
  std::vector<std::string> errors;
  int countertoken = 0;
  Token getToken() {
    return tokens[countertoken];
  }
  void advancedToken() {
    countertoken++;
  }
  void reverseToken() {
    countertoken--;
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
  std::unique_ptr<Program> programa;
  IsaParser(const std::vector<Token>& tokens, const std::vector<std::string> err) : tokens(tokens) , errors(err) {
    programa = std::make_unique<Program>();
  }
  
  void parserProgram() {
    while(getToken().type != TokenType::TOK_EOF) {
      if(getToken().type == TokenType::TOK_STRUCT) {
        //programa->addDeclaration(parserStruct());
      } else if(getToken().type == TokenType::TOK_FN) {
        
      } else if(getToken().type == TokenType::TOK_LET) {
         //parserIdentVarDeclType();
        auto let = VariableDeclarator();
        // programa->addDeclaration(std::move(let));type->token.value

        if(let != nullptr) programa->addDeclaration(std::move(let));
        //advancedToken();
      }
      advancedToken();
    }
    
  }

   std::unique_ptr<ArrayDeclarationNode> arrayDeclarationNode();

  std::unique_ptr<ExpType> parserIdentVarDeclType();
  std::unique_ptr<IdentifierExprAST> IdentifierDecl();
  std::unique_ptr<VariableExpAST> VariableDeclarator();

};



#endif // !IsaLLVMParserAST

