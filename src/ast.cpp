#include "ast.hpp"
#include "parser.hpp"
#include <llvm/IR/Value.h>

#define DEFINE_ACCEPT_METHOD(NodeType) \
llvm::Value* NodeType::accept(LLVMCodeGenVisitor &visitor) { \
    return visitor.visit(*this); \
}

DEFINE_ACCEPT_METHOD(VariableDeclarationNode)
DEFINE_ACCEPT_METHOD(StructDeclarationNode)
DEFINE_ACCEPT_METHOD(FunctionNode)
DEFINE_ACCEPT_METHOD(FunctionInstantiationNode)
DEFINE_ACCEPT_METHOD(BinaryExpressionNode)
DEFINE_ACCEPT_METHOD(ReturnNode)
DEFINE_ACCEPT_METHOD(IfNode)
DEFINE_ACCEPT_METHOD(WhileNode)
DEFINE_ACCEPT_METHOD(ForNode)
DEFINE_ACCEPT_METHOD(IntegerLiteralNode)
DEFINE_ACCEPT_METHOD(StringLiteralNode)
DEFINE_ACCEPT_METHOD(StructInstantiationNode)
DEFINE_ACCEPT_METHOD(VariableReferenceNode)
DEFINE_ACCEPT_METHOD(ExpressionStatementNode)
DEFINE_ACCEPT_METHOD(Bitcast)
DEFINE_ACCEPT_METHOD(FunctionCallNode)
DEFINE_ACCEPT_METHOD(AssignmentNode)
DEFINE_ACCEPT_METHOD(BlockNode)
DEFINE_ACCEPT_METHOD(ArrayTypeNode)
DEFINE_ACCEPT_METHOD(VariableValueNode)
