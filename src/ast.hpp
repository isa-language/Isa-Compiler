#pragma once 
#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <llvm/IR/Value.h>
#include "parser.hpp"


class LLVMCodeGenVisitor;

class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual llvm::Value* accept(class LLVMCodeGenVisitor &visitor) = 0;
};


class VariableDeclarationNode : public ASTNode {
public:
    std::string varName;
    std::string varType;
    std::unique_ptr<ASTNode> initializer;

    VariableDeclarationNode(const std::string &name, const std::string &type, std::unique_ptr<ASTNode> init)
        : varName(name), varType(type), initializer(std::move(init)) {}

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
    std::vector<std::unique_ptr<ASTNode>> body;

    FunctionNode(const std::string &name, const std::string &retType, std::vector<std::unique_ptr<ASTNode>> &&body)
        : functionName(name), returnType(retType), body(std::move(body)) {}

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

