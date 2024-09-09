/*
 * Error syntax isa compiler
 *
 * */

#pragma once
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
  int colunm;
  std::string filename;

  Error(ErrorType type, const std::string& message, 
        int line, int colunm, std::string filename) : type(type), message(message), line(line), 
                                                      colunm(colunm), filename(filename) {}

  void printError(const std::vector<std::string> codes);
};



#endif // !IsaLLVM_ERR



