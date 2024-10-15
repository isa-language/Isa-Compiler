#pragma once 
#include <algorithm>
#ifndef IsaLLVM_AST
#define IsaLLVM_AST
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <llvm/IR/Value.h>



class ASTNode;
class VariableDeclarationNode;
class IntegerLiteralNode;
class StructDeclarationNode;
class FunctionNode;
class BinaryExpressionNode;
class ReturnNode;
class IfNode;
class WhileNode;
class ForNode;

class Visitor {
  virtual llvm::Value* visit(VariableDeclarationNode &node) = 0;
  virtual llvm::Value* visit(IntegerLiteralNode &node) = 0;
  virtual llvm::Value* visit(StructDeclarationNode &node) = 0;
  virtual llvm::Value* visit(FunctionNode &node) = 0;
  virtual llvm::Value* visit(BinaryExpressionNode &node) = 0;
  virtual llvm::Value* visit(ReturnNode &node) = 0;
  virtual llvm::Value* visit(IfNode &node) = 0;
  virtual llvm::Value* visit(WhileNode &node) = 0;
  virtual llvm::Value* visit(ForNode &node) = 0;
};

class LLVMCodeGenVisitor : public Visitor {
public:
    llvm::IRBuilder<> *builder;
    llvm::LLVMContext *context;
    llvm::Module *module;

  LLVMCodeGenVisitor(llvm::IRBuilder<> *builder, llvm::LLVMContext *context, llvm::Module *module) : builder(builder), context(context), module(module) {}

  llvm::Value* visit(VariableDeclarationNode &node) override;
  llvm::Value* visit(IntegerLiteralNode &node) override;
  llvm::Value* visit(StructDeclarationNode &node) override;
  llvm::Value* visit(FunctionNode &node) override;
  llvm::Value* visit(BinaryExpressionNode &node) override;
  llvm::Value* visit(ReturnNode &node) override;
  llvm::Value* visit(IfNode &node) override;
  llvm::Value* visit(WhileNode &node) override;
  llvm::Value* visit(ForNode &node) override;
  llvm::Type* getLLVMType(const std::string &type);
  llvm::Type* getLLVMTypeFromASTType(const std::string &type);
};


class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual llvm::Value* accept(class LLVMCodeGenVisitor &visitor) = 0;
};

class IntegerLiteralNode : public ASTNode {
public:
    std::string type;
    int value;

IntegerLiteralNode(const std::string &type, int val) : type(type),value(val) {}

    llvm::Value* accept(LLVMCodeGenVisitor &visitor) override;
};

class VariableDeclarationNode : public ASTNode {
public:
    std::string varName;
    std::string varType;
    std::unique_ptr<ASTNode> initializer;

    VariableDeclarationNode(const std::string &name, const std::string &type, std::unique_ptr<ASTNode> init)
        : varName(name), varType(type), initializer(std::move(init)) {}
    VariableDeclarationNode(const std::string &name, const std::string &type)
        : varName(name), varType(type), initializer(nullptr) {}

    llvm::Value* accept(LLVMCodeGenVisitor &visitor) override;
};



class StructDeclarationNode : public ASTNode {
public:
    std::string structName;
    std::vector<std::unique_ptr<VariableDeclarationNode>> members;

    StructDeclarationNode(const std::string &name, std::vector<std::unique_ptr<VariableDeclarationNode>> &&m)
        : structName(name), members(std::move(m)) {}

    llvm::Value* accept(LLVMCodeGenVisitor &visitor) override;
};

class FunctionNode : public ASTNode {
public:
    std::string functionName;
    std::string returnType;
    std::vector<std::unique_ptr<VariableDeclarationNode>> parameters;
    std::vector<std::unique_ptr<ASTNode>> functionBody;

    FunctionNode(const std::string &name, const std::string &retType, std::vector<std::unique_ptr<VariableDeclarationNode>> params, 
                 std::vector<std::unique_ptr<ASTNode>> body)
        : functionName(name), returnType(retType), parameters(std::move(params)), functionBody(std::move(body)) {}

    FunctionNode(const std::string &name, const std::string &retType)
        : functionName(name), returnType(retType) {}

    llvm::Value* accept(LLVMCodeGenVisitor &visitor) override;
};



class BinaryExpressionNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> left;
    std::string op;
    std::unique_ptr<ASTNode> right;

    BinaryExpressionNode(std::unique_ptr<ASTNode> l, const std::string &o, std::unique_ptr<ASTNode> r)
        : left(std::move(l)), op(o), right(std::move(r)) {}

    llvm::Value* accept(LLVMCodeGenVisitor &visitor) override;
};


class ReturnNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> returnValue;

    ReturnNode(std::unique_ptr<ASTNode> retVal)
        : returnValue(std::move(retVal)) {}

    llvm::Value* accept(LLVMCodeGenVisitor &visitor) override;
};


class IfNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> thenBlock;
    std::unique_ptr<ASTNode> elseBlock;

    IfNode(std::unique_ptr<ASTNode> cond, std::unique_ptr<ASTNode> thenB, std::unique_ptr<ASTNode> elseB)
        : condition(std::move(cond)), thenBlock(std::move(thenB)), elseBlock(std::move(elseB)) {}

    llvm::Value* accept(LLVMCodeGenVisitor &visitor) override;
};


class WhileNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> body;

    WhileNode(std::unique_ptr<ASTNode> cond, std::unique_ptr<ASTNode> b)
        : condition(std::move(cond)), body(std::move(b)) {}

    llvm::Value* accept(LLVMCodeGenVisitor &visitor) override;
};


class ForNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> initializer;
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> increment;
    std::unique_ptr<ASTNode> body;

    ForNode(std::unique_ptr<ASTNode> init, std::unique_ptr<ASTNode> cond, std::unique_ptr<ASTNode> incr, std::unique_ptr<ASTNode> b)
        : initializer(std::move(init)), condition(std::move(cond)), increment(std::move(incr)), body(std::move(b)) {}

    llvm::Value* accept(LLVMCodeGenVisitor &visitor) override;
};



#endif // !IsaLLVM_AST
