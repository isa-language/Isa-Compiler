#include "parser.hpp"
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>



llvm::Type* LLVMCodeGenVisitor::getLLVMType(const std::string &type) {
    if (type == "i8")       return llvm::Type::getInt8Ty(context);
    else if (type == "i16") return llvm::Type::getInt16Ty(context);
    else if (type == "i32") return llvm::Type::getInt32Ty(context);
    else if (type == "i64") return llvm::Type::getInt64Ty(context);
    else if (type == "u8")  return llvm::Type::getInt8PtrTy(context, true);
    else if (type == "u16")  return llvm::Type::getInt16PtrTy(context, true);
    else if (type == "u32") return llvm::Type::getInt32PtrTy(context, true);
    else if (type == "u64")  return llvm::Type::getInt64PtrTy(context, true);
    else if (type == "f16") return llvm::Type::getHalfTy(context);
    else if (type == "f32") return llvm::Type::getFloatTy(context);
    else if (type == "f64") return llvm::Type::getDoubleTy(context);

    else if (type == "string") return llvm::Type::getInt8PtrTy(context);
    return nullptr;
}


llvm::Value* LLVMCodeGenVisitor::visit(VariableDeclarationNode &node) {
    llvm::Type* llvmType = getLLVMType(node.varType);
    llvm::Value* alloca = builder.CreateAlloca(llvmType, nullptr, node.varName);
    
    if (node.initializer) {
        llvm::Value* initVal = node.initializer->accept(*this);
        builder.CreateStore(alloca, initVal);
    }

    return alloca;
}

llvm::Value* LLVMCodeGenVisitor::visit(StructDeclarationNode &node){
    return nullptr;
}
llvm::Value* LLVMCodeGenVisitor::visit(FunctionNode &node){
    return nullptr;
}
llvm::Value* LLVMCodeGenVisitor::visit(BinaryExpressionNode &node){
    return nullptr;
}
llvm::Value* LLVMCodeGenVisitor::visit(ReturnNode &node){
    return nullptr;
}
llvm::Value* LLVMCodeGenVisitor::visit(IfNode &node){
    return nullptr;
}
llvm::Value* LLVMCodeGenVisitor::visit(WhileNode &node){
    return nullptr;
}
llvm::Value* LLVMCodeGenVisitor::visit(ForNode &node){
    return nullptr;
}
