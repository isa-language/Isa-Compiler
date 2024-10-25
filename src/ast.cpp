#include "ast.hpp"
#include "parser.hpp"
#include <llvm/IR/Value.h>

llvm::Value* VariableDeclarationNode::accept(LLVMCodeGenVisitor &visitor) {
    return visitor.visit(*this);
}

llvm::Value* StructDeclarationNode::accept(LLVMCodeGenVisitor &visitor) {
    return visitor.visit(*this);
}

llvm::Value* FunctionNode::accept(LLVMCodeGenVisitor &visitor) {
    return visitor.visit(*this);
}
llvm::Value* FunctionInstantiationNode::accept(LLVMCodeGenVisitor &visitor) {
    return visitor.visit(*this);
}
llvm::Value* BinaryExpressionNode::accept(LLVMCodeGenVisitor &visitor) {
    return visitor.visit(*this);
}

llvm::Value* ReturnNode::accept(LLVMCodeGenVisitor &visitor) {
    return visitor.visit(*this);
}

llvm::Value* IfNode::accept(LLVMCodeGenVisitor &visitor) {
    return visitor.visit(*this);
}

llvm::Value* WhileNode::accept(LLVMCodeGenVisitor &visitor) {
    return visitor.visit(*this);
}

llvm::Value* ForNode::accept(LLVMCodeGenVisitor &visitor) {
    return visitor.visit(*this);
}

llvm::Value* IntegerLiteralNode::accept(LLVMCodeGenVisitor &visitor) {
    return visitor.visit(*this);
}
llvm::Value* StringLiteralNode::accept(LLVMCodeGenVisitor &visitor) {
    return visitor.visit(*this);
}
llvm::Value* StructInstantiationNode::accept(LLVMCodeGenVisitor &visitor) {
        return visitor.visit(*this);
}
llvm::Value* VariableReferenceNode::accept(LLVMCodeGenVisitor &visitor) {
        return visitor.visit(*this);
}

llvm::Value* ExpressionStatementNode::accept(LLVMCodeGenVisitor &visitor) {
        return visitor.visit(*this);
}
llvm::Value* Bitcast::accept(LLVMCodeGenVisitor &visitor) {
        return visitor.visit(*this);
}
llvm::Value* FunctionCallNode::accept(LLVMCodeGenVisitor &visitor) {
        return visitor.visit(*this);
}
llvm::Value* AssignmentNode::accept(LLVMCodeGenVisitor &visitor) {
        return visitor.visit(*this);
}
llvm::Value* BlockNode::accept(LLVMCodeGenVisitor &visitor) {
    return visitor.visit(*this);
}
llvm::Value* ArrayTypeNode::accept(LLVMCodeGenVisitor &visitor) {
    return visitor.visit(*this);
}

llvm::Value* VariableValueNode::accept(LLVMCodeGenVisitor &visitor) {
    return visitor.visit(*this);
}