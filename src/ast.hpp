/**
 * IsaLLVM_AST Abstract syntax tree -> isa compiler
 * */
#pragma once 
#include <cctype>
#include <charconv>
#include <cstddef>
#include <llvm/IR/LLVMContext.h>
#include <utility>
#ifndef IsaLLVM_AST
#define IsaLLVM_AST
/* libs llvm include */
#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>

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
  LIST,
  FUNCTION,
  ARRAY
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
    std::vector<std::unique_ptr<ASTNode>> program;

    void addDeclaration(std::unique_ptr<ASTNode> declaration) {
      program.push_back(std::move(declaration));
  }
};

/* ^ isso nao esta bom! */
class IdentifierExprAST : public ASTNode {
public:
  std::string name;
  std::string value;

  IdentifierExprAST(const std::string& name, const std::string& value) : name(name), value(value) {}
  //llvm::Value *codegen() override;
  
};

class ExprAST : public ASTNode {
  public:
  std::string value;
  ExprAST( std::string value ) : value(value) {}
  //llvm::Value *codegen() override;
};

class ArrayDeclarationNode : public ASTNode {
  public:
  std::string name;
  size_t size;
  std::unique_ptr<ExpType> type;
  std::vector<std::unique_ptr<IdentifierExprAST>> elements;
  ArrayDeclarationNode(std::string &name,size_t size,std::unique_ptr<ExpType> type,std::vector<std::unique_ptr<IdentifierExprAST>> elements) : name(name), size(size), type(std::move(type)), elements(std::move(elements)) {} 
  ArrayDeclarationNode(std::string &name,std::unique_ptr<ExpType> type,std::vector<std::unique_ptr<IdentifierExprAST>> elements) : size(1), type(std::move(type)), elements(std::move(elements)), name(name) {} 
  ArrayDeclarationNode(std::string &name,std::unique_ptr<ExpType> type) : size(1), type(std::move(type)), name(name) {} 

};

class VariableExpAST : public ASTNode {
public:
  std::string name;
  std::unique_ptr<ExpType> type;
  std::unique_ptr<IdentifierExprAST> identifier;
  std::unique_ptr<ArrayDeclarationNode> array;
  // std::unique_ptr<ExpType> expression; 



  VariableExpAST(std::string name, std::unique_ptr<ExpType> type, std::unique_ptr<IdentifierExprAST> identifier) 
    : type(std::move(type)), identifier(std::move(identifier)), name(name) {}

  VariableExpAST(std::string name, std::unique_ptr<ExpType> type, std::unique_ptr<ArrayDeclarationNode> array) 
    : type(std::move(type)),  name(name), array(std::move(array)) {}
  

  llvm::Value *codegen(llvm::IRBuilder<> &builder, llvm::LLVMContext &context, llvm::Module& module) {
   llvm::AllocaInst* variable = nullptr;
    if (type->token.value == "i8") {
      llvm::Type* int8Type = llvm::Type::getInt8Ty(context);
      variable = builder.CreateAlloca(int8Type, nullptr, name);
      llvm::Value* initialValue = llvm::ConstantInt::get(int8Type, std::stoi(identifier->value), true);
      if (initialValue != nullptr) {
        builder.CreateStore(initialValue, variable);
      }
    } else if(type->token.value == "i16") {
      llvm::Type* int16Type = llvm::Type::getInt16Ty(context);
      variable = builder.CreateAlloca(int16Type, nullptr, name);
      llvm::Value* initialValue = llvm::ConstantInt::get(int16Type, std::stoi(identifier->value), true);
      if (initialValue != nullptr) {
        builder.CreateStore(initialValue, variable);
      }
    } else if (type->token.value == "i32") {
      llvm::Type* int32Type = llvm::Type::getInt32Ty(context);
      variable = builder.CreateAlloca(int32Type, nullptr, name);
      llvm::Value* initialValue = llvm::ConstantInt::get(int32Type, std::stoi(identifier->value), true);
      if (initialValue != nullptr) {
        builder.CreateStore(initialValue, variable);
      }
    } else if (type->token.value == "i64") {
      llvm::Type* int64Type = llvm::Type::getInt64Ty(context);
      variable = builder.CreateAlloca(int64Type, nullptr, name);
      llvm::Value* initialValue = llvm::ConstantInt::get(int64Type, std::stoi(identifier->value), true);
      if (initialValue != nullptr) {
        builder.CreateStore(initialValue, variable);
      }
    } else if(type->token.value == "u8") {
      llvm::Type* int8Type = llvm::Type::getInt8Ty(context);
      variable = builder.CreateAlloca(int8Type, nullptr, name);
      llvm::Value* initialValue = llvm::ConstantInt::get(int8Type, std::stoi(identifier->value), false);
      if (initialValue != nullptr) {
        builder.CreateStore(initialValue, variable);
      }
    } else if(type->token.value == "u16") {
      llvm::Type* int16Type = llvm::Type::getInt16Ty(context);
      variable = builder.CreateAlloca(int16Type, nullptr, name);
      llvm::Value* initialValue = llvm::ConstantInt::get(int16Type, std::stoi(identifier->value), false);
      if (initialValue != nullptr) {
        builder.CreateStore(initialValue, variable);
      }
    } else if (type->token.value == "u32") {
      llvm::Type* int32Type = llvm::Type::getInt32Ty(context);
      variable = builder.CreateAlloca(int32Type, nullptr, name);
      llvm::Value* initialValue = llvm::ConstantInt::get(int32Type, std::stoi(identifier->value), false);
      if (initialValue != nullptr) {
        builder.CreateStore(initialValue, variable);
      }
    } else if (type->token.value == "u64") {
      llvm::Type* int64Type = llvm::Type::getInt64Ty(context);
      variable = builder.CreateAlloca(int64Type, nullptr, name);
      llvm::Value* initialValue = llvm::ConstantInt::get(int64Type, std::stoi(identifier->value), false);
      if (initialValue != nullptr) {
        builder.CreateStore(initialValue, variable);
      }
    } else if(type->token.value == "f16") {
      llvm::Type *floatType = llvm::Type::getHalfTy(context);
      variable = builder.CreateAlloca(floatType, nullptr, name);
      llvm::AllocaInst *alloca = builder.CreateAlloca(floatType, nullptr, name);
      llvm::Value *initialValue = llvm::ConstantFP::get(floatType, std::stof(identifier->value));
      if (initialValue != nullptr) {
        builder.CreateStore(initialValue, variable);
      }
    } else if(type->token.value == "f32") {
      llvm::Type *floatType = llvm::Type::getFloatTy(context);
      variable = builder.CreateAlloca(floatType, nullptr, name);
      llvm::AllocaInst *alloca = builder.CreateAlloca(floatType, nullptr, name);
      llvm::Value *initialValue = llvm::ConstantFP::get(floatType, std::stof(identifier->value));
      if (initialValue != nullptr) {
        builder.CreateStore(initialValue, variable);
      }
    } else if(type->token.value == "f64") {
      llvm::Type *floatType = llvm::Type::getDoubleTy(context);
      variable = builder.CreateAlloca(floatType, nullptr, name);
      llvm::AllocaInst *alloca = builder.CreateAlloca(floatType, nullptr, name);
      llvm::Value *initialValue = llvm::ConstantFP::get(floatType, std::stod(identifier->value));
      if (initialValue != nullptr) {
        builder.CreateStore(initialValue, variable);
      }
    } else if(type->token.value == "string") {
      llvm::Type *charPtrType = llvm::Type::getInt8PtrTy(context);
      llvm::Constant *stringValue = llvm::ConstantDataArray::getString(context, identifier->value, true);
      llvm::GlobalVariable *globalString = new llvm::GlobalVariable(
        module, stringValue->getType(), true, llvm::GlobalValue::PrivateLinkage, stringValue, name);
      llvm::Constant *zero = llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0);
      llvm::Value *stringPtr = builder.CreateInBoundsGEP(globalString->getValueType(), globalString, {zero, zero}, name);
    } else if(type->token.value == "array") {
      llvm::ArrayType* arrayType = llvm::ArrayType::get(llvm::Type::getInt32Ty(context), array->size);
      llvm::Value* arrayAlloca = builder.CreateAlloca(arrayType, nullptr, name);
      return arrayAlloca;
    }
    
    
   

    return variable;
};
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


class FunctionDeclaration : public ASTNode {
  std::unique_ptr<ExpType> function;
  std::vector<std::unique_ptr<VariableExpAST>> params;
  std::unique_ptr<Token> returnType;
  std::vector<std::unique_ptr<ASTNode>> body;

  FunctionDeclaration(std::unique_ptr<ExpType> function, std::vector<std::unique_ptr<VariableExpAST>> params, 
                      std::unique_ptr<Token> returnType,std::vector<std::unique_ptr<ASTNode>> body) : 
                      function(std::move(function)), params(std::move(params)), returnType(std::move(returnType)), body(std::move(body)) {} 
};

#endif // !IsaLLVM_AST
