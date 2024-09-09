#include "err.hpp"
#include <cwchar>
#include <string>
#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include "file.hpp"


int countdigits(int number) {
    if (number == 0) return 1; 
    return std::floor(std::log10(std::abs(number))) + 1;
}


void Error::printError(const std::vector<std::string> codes) {
  const int contextLines = 1;  
  int startLine = std::max(0, line - contextLines - 1);
  int endLine = std::min((int)codes.size(), line + contextLines);
  std::string color;
  switch (type) {
    case ErrorType::SyntaxError: {
      color = "\033[1;31m"; 
      std::cout << color << "Error: " << "Syntax Error";
      break;
    }
    case ErrorType::SemanticError: {
      color = "\033[1;33m"; 
      std::cout << color << "Error: " << "Semantic Error";
      break;
    }
    case ErrorType::RuntimeError: {
      color = "\033[1;35m";
      std::cout << color << "Error: " << "Runtime Error";
      break;
    }
    default: {
      color = "\033[0m"; 
      std::cout << "Unknown Error";
      break;
    }
  }
  std::cout << " at " << filename << ":" << line << ":" << colunm << '\n';
  std::cout << "Message: " << message << "\033[0m" << '\n';
  // std::cout << '\n' << codes << '\n';
  //
  for (int i = startLine - 1; i < endLine; i++) {
     
    if (i == line - 1) {
      std::cout << (i + 1) << " | " << codes[i] << '\n';
        // std::cout << colunm <<  '\n'; 
        std::cout << std::string(colunm + (countdigits(i+1) + 2), ' ')  << "↑______" << message << '\n';
    }
  }
  
}
