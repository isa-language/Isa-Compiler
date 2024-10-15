#include "parser.hpp"
#include "ast.hpp"
#include <alloca.h>
#include <iostream>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>

llvm::Type* LLVMCodeGenVisitor::getLLVMTypeFromASTType(const std::string& type) {
    if (type == "i8")       return llvm::Type::getInt8Ty(*context);
    else if (type == "i16") return llvm::Type::getInt16Ty(*context);
    else if (type == "i32") return llvm::Type::getInt32Ty(*context);
    else if (type == "i64") return llvm::Type::getInt64Ty(*context);
    else if (type == "u8")  return llvm::Type::getInt8PtrTy(*context, true);
    else if (type == "u16")  return llvm::Type::getInt16PtrTy(*context, true);
    else if (type == "u32") return llvm::Type::getInt32PtrTy(*context, true);
    else if (type == "u64")  return llvm::Type::getInt64PtrTy(*context, true);
    else if (type == "f16") return llvm::Type::getHalfTy(*context);
    else if (type == "f32") return llvm::Type::getFloatTy(*context);
    else if (type == "f64") return llvm::Type::getDoubleTy(*context);
    else if (type == "void") return llvm::Type::getVoidTy(*context);
    else if (type == "string") return llvm::Type::getInt8PtrTy(*context);
    return nullptr;
}

llvm::Type* LLVMCodeGenVisitor::getLLVMType(const std::string &type) {
    if (type == "i8")       return llvm::Type::getInt8Ty(*context);
    else if (type == "i16") return llvm::Type::getInt16Ty(*context);
    else if (type == "i32") return llvm::Type::getInt32Ty(*context);
    else if (type == "i64") return llvm::Type::getInt64Ty(*context);
    else if (type == "u8")  return llvm::Type::getInt8PtrTy(*context, true);
    else if (type == "u16")  return llvm::Type::getInt16PtrTy(*context, true);
    else if (type == "u32") return llvm::Type::getInt32PtrTy(*context, true);
    else if (type == "u64")  return llvm::Type::getInt64PtrTy(*context, true);
    else if (type == "f16") return llvm::Type::getHalfTy(*context);
    else if (type == "f32") return llvm::Type::getFloatTy(*context);
    else if (type == "f64") return llvm::Type::getDoubleTy(*context);

    else if (type == "string") return llvm::Type::getInt8PtrTy(*context);
    return nullptr;
}


llvm::Value* LLVMCodeGenVisitor::visit(VariableDeclarationNode &node) {
    llvm::Type* llvmType = getLLVMType(node.varType);
    
    if (!builder->GetInsertBlock()) {
        std::cerr << "Builder não inicializado corretamente" << std::endl;
        return nullptr;
    }

    // Verifique se o nome da variável é válido.
    if (node.varName.empty()) {
        std::cerr << "Nome da variável não pode ser vazio" << std::endl;
        return nullptr;
    }
    llvm::Value* alloca = builder->CreateAlloca(llvmType, nullptr, node.varName);
    
    if (node.initializer) {
        auto initVal = node.initializer->accept(*this);
        builder->CreateStore(initVal, alloca);
    }
    return alloca;
}



llvm::Value* LLVMCodeGenVisitor::visit(IntegerLiteralNode &node) {
    return llvm::ConstantInt::get(getLLVMType(node.type), node.value);
}
llvm::Value* LLVMCodeGenVisitor::visit(FunctionNode &node){
    llvm::Type *returnType = getLLVMTypeFromASTType(node.returnType);
    std::vector<llvm::Type*> paramTypes;

    for (const auto &param : node.parameters) {
        llvm::Type *paramType = getLLVMTypeFromASTType(param->varType);
        paramTypes.push_back(paramType);
    }

    llvm::FunctionType *funcType = llvm::FunctionType::get(returnType, paramTypes, false);

    llvm::Function *function = llvm::Function::Create(
        funcType,
        llvm::Function::ExternalLinkage,
        node.functionName,
        module
    );

    unsigned idx = 0;
    for (auto &arg : function->args()) {
        arg.setName(node.parameters[idx++]->varName);
    }

    llvm::BasicBlock *entryBlock = llvm::BasicBlock::Create(*context, "entry", function);
    builder->SetInsertPoint(entryBlock);


    for (const auto &stmt : node.functionBody) {
        stmt->accept(*this);
    }

    if (!entryBlock->getTerminator()) {
        if (returnType->isVoidTy()) {
            builder->CreateRetVoid(); 
        } else {
            builder->CreateRet(llvm::ConstantInt::get(returnType, 0));
        }
    }

    llvm::verifyFunction(*function);
    return function;
}

llvm::Value* LLVMCodeGenVisitor::visit(StructDeclarationNode &node){
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
