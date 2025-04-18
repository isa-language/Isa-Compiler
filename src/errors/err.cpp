#include "frontend/lexer/token.hpp"
#include "errors/err.hpp"
#include <cwchar>
#include <string>
#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include "frontend/utils/file.hpp"

int countdigits(int number) {
    if (number == 0) return 1; 
    return std::floor(std::log10(std::abs(number))) + 1;
}

void Error::printError(const std::vector<Token> tokens) {

  std::string color;
  std::string errorType;
  
  switch (type) {
      case ErrorType::SyntaxError:
          color = "\033[1;31m";
          errorType = "Syntax Error";
          break;
      case ErrorType::SemanticError:
          color = "\033[1;33m";
          errorType = "Semantic Error";
          break;
      case ErrorType::RuntimeError:
          color = "\033[1;35m";
          errorType = "Runtime Error";
          break;
      default:
          color = "\033[0m";
          errorType = "Unknown Error";
          break;
  }

  std::cout << color << filename << ":" << line << ":" << column << ": " 
            << errorType << ": " << message << "\033[0m\n";

  const Token* errorToken = nullptr;
  for (const auto& token : tokens) {
      if (token.line == line && token.column <= column && 
          (token.column + token.value.length()) > column) {
          errorToken = &token;
          break;
      }
  }

  if (errorToken) {
      std::vector<const Token*> lineTokens;
      for (const auto& token : tokens) {
          if (token.line == line) {
              lineTokens.push_back(&token);
          }
      }

      std::string fullLine;
      int currentColumn = 1;
      for (const auto* token : lineTokens) {
          while (currentColumn < token->column) {
              fullLine += ' ';
              currentColumn++;
          }
          if( token->type == TokenType::TOK_STRING_LITERAL) {
            fullLine += '"' + token->value + '"';
          } else {
            fullLine += token->value;
          }
          currentColumn += token->value.length();
      }

      std::cout << std::setw(countdigits(line)) << line << " | " << fullLine << "\n";

      int padding = countdigits(line) + 2 + (column - 1); 

      std::cout << std::string(padding, ' ') << color << "^\n";
      std::cout << std::string(padding, ' ') << "└─ " << message << "\033[0m\n";
  }
}