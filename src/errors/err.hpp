/*
 * Error syntax isa compiler
 *
 * */

#pragma once
#include "frontend/lexer/token.hpp"
#include <vector>
#ifndef IsaLLVM_ERR
#define IsaLLVM_ERR
#include <string>

enum class ErrorType {
  SyntaxError,
  SemanticError,
  RuntimeError
};

struct Error {
  ErrorType type;
  std::string message;
  int line;
  int column;
  std::string filename;

  Error(ErrorType type, const std::string& message, 
        int line, int column, std::string filename) : type(type), message(message), line(line), 
        column(column), filename(filename) {}

  void printError(const std::vector<Token> codes);
};



#endif // !IsaLLVM_ERR



