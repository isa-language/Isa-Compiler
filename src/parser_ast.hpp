/**
 * IsaLLVMCompiler -> parser abstract syntax tree 
 * */
#pragma once
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
        
      } else if(getToken().type == TokenType::TOK_FN) {
        
      } else if(getToken().type == TokenType::TOK_LE) {
        
      }
    }
    
  }
  


};

#endif // !IsaLLVMParserAST

