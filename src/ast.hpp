/**
 * IsaLLVM_AST Abstract syntax tree -> isa compiler
 * */
#pragma once 
#ifndef IsaLLVM_AST
#define IsaLLVM_AST
/* libs llvm include */
#include <llvm/IR/Value.h>

/* includes project */
#include "token.hpp"
/* include libc */
#include <algorithm>
#include <memory>
#include <string>
#include <vector>



class ASTNode {
  public:
    virtual ~ASTNode() = default;
    // virtual llvm::Value* codegen() = 0;

};

enum class Type {
  NUMBER,
  STRING,
  SYMBOL,
  LIST
};

struct ExpType {
  Type type;
  Token token;
  ~ExpType() = default;
  ExpType(Type type, Token token) : type(type), token(token) {}
};

using ASTNodePtr = std::unique_ptr<ASTNode>;

/* *
 * pessima forma de fazer isso 
 * vou mudar mais tarde...
 * */
class Program : public ASTNode {
  public:
    Program() {}
    std::vector<ASTNodePtr> program;

    void addDeclaration(ASTNodePtr declaration) {
      program.push_back(std::move(declaration));
  }
};

/* ^ isso nao esta bom! */
class IdentifierExprAST : public ASTNode {
public:
  std::string value;

  IdentifierExprAST(std::string value) : value(value) {}
  //llvm::Value *codegen() override;
  
};

class ExprAST : public ASTNode {
  public:
  std::string value;
  ExprAST( std::string value ) : value(value) {}
  //llvm::Value *codegen() override;
};


class VariableExpAST : public ASTNode {
public:
  //std::string name;
  std::unique_ptr<ExpType> type;
  std::unique_ptr<IdentifierExprAST> identifier;
  // std::unique_ptr<ExpType> expression; 



  VariableExpAST(std::unique_ptr<ExpType> type, std::unique_ptr<IdentifierExprAST> identifier) 
    : type(std::move(type)), identifier(std::move(identifier)) {}

  //llvm::Value *codegen() override;
};

class BinaryOpExpAST : public ASTNode {
public:
  std::unique_ptr<Token> op;
  ASTNodePtr lhs, rhs;

  BinaryOpExpAST( std::unique_ptr<Token> op, ASTNodePtr lhs, ASTNodePtr rhs) : op{std::move(op)}, lhs{std::move(lhs)}, rhs{std::move(rhs)} {}

  //llvm::Value *codegen() override;
};


class CallExpAST : public ASTNode {
  std::unique_ptr<Token> callee;
  std::vector<ASTNodePtr> args;

public:
  CallExpAST(std::unique_ptr<Token> callee, std::vector<ASTNodePtr> args) : callee{std::move(callee)}, args{std::move(args)} {}
  const std::string& getName() const {
    return callee->value;
  }
};

#endif // !IsaLLVM_AST
