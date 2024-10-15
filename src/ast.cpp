#include "ast.hpp"
#include "parser.hpp"

llvm::Value* VariableDeclarationNode::accept(LLVMCodeGenVisitor &visitor) {
    return visitor.visit(*this);
}

llvm::Value* StructDeclarationNode::accept(LLVMCodeGenVisitor &visitor) {
    return visitor.visit(*this);
}

llvm::Value* FunctionNode::accept(LLVMCodeGenVisitor &visitor) {
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

