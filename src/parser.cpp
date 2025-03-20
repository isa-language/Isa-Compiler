#include "parser.hpp"
#include "ast.hpp"
#include <alloca.h>
#include <iostream>
#include <llvm-c/Core.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include </usr/include/llvm/IR/TypedPointerType.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>     
#include <llvm/IR/Module.h>        
#include <llvm/IR/Constants.h>      
#include <llvm/Support/raw_ostream.h> 
#include <memory>


llvm::Type* LLVMCodeGenVisitor::getLLVMType(const std::string &type) {
    static const std::unordered_map<std::string, std::function<llvm::Type*(llvm::LLVMContext&)>> baseTypes = {
        {"i8",  [](llvm::LLVMContext &ctx) { return llvm::Type::getInt8Ty(ctx); }},
        {"i16", [](llvm::LLVMContext &ctx) { return llvm::Type::getInt16Ty(ctx); }},
        {"i32", [](llvm::LLVMContext &ctx) { return llvm::Type::getInt32Ty(ctx); }},
        {"i64", [](llvm::LLVMContext &ctx) { return llvm::Type::getInt64Ty(ctx); }},
        {"f16", [](llvm::LLVMContext &ctx) { return llvm::Type::getHalfTy(ctx); }},
        {"f32", [](llvm::LLVMContext &ctx) { return llvm::Type::getFloatTy(ctx); }},
        {"f64", [](llvm::LLVMContext &ctx) { return llvm::Type::getDoubleTy(ctx); }},
        {"bool", [](llvm::LLVMContext &ctx) { return llvm::Type::getInt1Ty(ctx); }},
        {"void", [](llvm::LLVMContext &ctx) { return llvm::Type::getVoidTy(ctx); }},
        {"string", [](llvm::LLVMContext &ctx) { return llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0); }}
    };

    if (baseTypes.count(type)) {
        return baseTypes.at(type)(*context);
    }

    if (type.rfind("u", 0) == 0 && baseTypes.count("i" + type.substr(1))) {
        return llvm::PointerType::get(baseTypes.at("i" + type.substr(1))(*context), 0);
    }

    if (type.rfind("str[", 0) == 0) {
        size_t start = type.find('[') + 1;
        size_t end = type.find(']');
        int length = std::stoi(type.substr(start, end - start));
        return llvm::ArrayType::get(llvm::Type::getInt8Ty(*context), length);
    }

    if (type.front() == '[') {
        size_t start = 1;
        size_t comma = type.find(',');
        size_t end = type.find(']');
        std::string elementType = type.substr(start, comma - start);
        int arraySize = std::stoi(type.substr(comma + 1, end - comma - 1));
        llvm::Type* llvmElementType = getLLVMType(elementType);
        return llvmElementType ? llvm::ArrayType::get(llvmElementType, arraySize) : nullptr;
    }

    if (type.back() == '*' || type.back() == '&') {
        std::string baseType = type.substr(0, type.size() - 1);
        llvm::Type* llvmBaseType = getLLVMType(baseType);
        return llvmBaseType ? llvm::PointerType::get(llvmBaseType, 0) : nullptr;
    }

    if (type.rfind("struct", 0) == 0) {
        return llvm::StructType::getTypeByName(*context, type);
    }

    return nullptr;
}

llvm::Value* LLVMCodeGenVisitor::getInitValueForType(const std::string &type) {
    llvm::Type* llvmType = getLLVMType(type);
    if (!llvmType) return nullptr;

    if (llvmType->isIntegerTy()) {
        return llvm::ConstantInt::get(llvmType, 0);
    }
    if (llvmType->isFloatingPointTy()) {
        return llvm::ConstantFP::get(llvmType, 0.0);
    }
    if (llvmType->isPointerTy()) {
        return llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(llvmType));
    }
    if (llvmType->isArrayTy()) {
        llvm::ArrayType* arrayType = llvm::cast<llvm::ArrayType>(llvmType);
        llvm::Type* elementType = arrayType->getElementType();
        std::vector<llvm::Constant*> elements(arrayType->getNumElements(), static_cast<llvm::Constant*>(getInitValueForType(type.substr(1, type.find(',') - 1))));
        return llvm::ConstantArray::get(arrayType, elements);
    }

    return nullptr;
}


void LLVMCodeGenVisitor::addVariable(const std::string &name, llvm::Value *value) {
    variables[name] = value;
}


llvm::Value* LLVMCodeGenVisitor::getVariable(const std::string &name) {
    auto it = variables.find(name);
    if (it != variables.end()) {
        return it->second;
    }
    return nullptr;  
}


llvm::Value* LLVMCodeGenVisitor::lookupVariable(const std::string &name) {
    auto it = symbolTable.find(name);
    if (it != symbolTable.end()) {
        return it->second;
    }
    return nullptr;
}


llvm::Value* LLVMCodeGenVisitor::visit(VariableDeclarationNode &node) {
    llvm::Type* llvmType = getLLVMType(node.varType);
    if (!builder->GetInsertBlock()) {
        std::cerr << "Builder não inicializado corretamente" << std::endl;
        return nullptr;
    }

    if (node.varName.empty()) {
        std::cerr << "Nome da variável não pode ser vazio" << std::endl;
        return nullptr;
    }
    llvm::Value* alloca = builder->CreateAlloca(llvmType, nullptr, node.varName);
    
    if (node.initializer) {
        auto initVal = node.initializer->accept(*this);
        builder->CreateStore(initVal, alloca);
    }
    symbolTable[node.varName] = alloca;
    return alloca;
}

llvm::Value* LLVMCodeGenVisitor::visit(ArrayTypeNode &node) {
    llvm::Type* llvmElementType = getLLVMType(node.arrayType);
    if (!llvmElementType) {
        throw std::runtime_error("Unsupported array element type: " + node.arrayType);
    }

    llvm::ArrayType* arrayType = llvm::ArrayType::get(llvmElementType, node.size);

    llvm::Value* arrayAlloc = builder->CreateAlloca(arrayType, nullptr, "array_alloc");

    if (node.initializer.empty()) {

        int count = 0;
        for (auto &a: node.initializer) {
           
            llvm::Value* value = a->accept(*this);
            llvm::Value* index = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context), count);
            llvm::Value* elementPtr = builder->CreateGEP(arrayType, arrayAlloc, index, "element_ptr");
            builder->CreateStore(value, elementPtr);
        }
    }

    return arrayAlloc; 
}



llvm::Value* LLVMCodeGenVisitor::visit(ExpressionStatementNode &node) {
    llvm::Value* exprValue = node.expression->accept(*this);
    return nullptr;
}
llvm::Value* LLVMCodeGenVisitor::visit(VariableValueNode &node) {
    llvm::Value* variable = lookupVariable(node.getName());
    if (!variable) {
        variable = variables[node.variableName];
    }

    if (!variable) {
        throw std::runtime_error("Variable not found: " + node.getName());
    }

    /*LLVMTypeRef leftTypeRef = LLVMTypeOf(reinterpret_cast<LLVMValueRef>(variable));
    if (LLVMGetTypeKind(leftTypeRef) == LLVMPointerTypeKind) {
        LLVMTypeRef elementTypeLeft = LLVMGetElementType(leftTypeRef);
        llvm::Type* llvmElementTypeLeft = reinterpret_cast<llvm::Type*>(elementTypeLeft);
        llvm::Value *loadedValue = builder->CreateLoad(llvmElementTypeLeft, variable, node.getName() + "_val");
        return loadedValue;
    }*/
    LLVMTypeRef leftTypeRef = LLVMTypeOf(reinterpret_cast<LLVMValueRef>(variable));
    if (LLVMGetTypeKind(leftTypeRef) == LLVMPointerTypeKind) {
        LLVMTypeRef elementTypeLeft = LLVMGetElementType(leftTypeRef);
        llvm::Type* pointerType = reinterpret_cast<llvm::Type*>(leftTypeRef);
        return builder->CreateLoad(pointerType, variable, node.getName() + "_val");
    } else {
        LLVMValueRef llvmVarRef = reinterpret_cast<LLVMValueRef>(variable);
        LLVMTypeRef llvmPointerType = LLVMTypeOf(llvmVarRef);
        LLVMTypeRef llvmValueTypeRef = LLVMGetElementType(llvmPointerType);


        llvm::Type* valueType = reinterpret_cast<llvm::Type*>(llvmValueTypeRef);


        llvm::Value *loadedValue = builder->CreateLoad(valueType, variable, node.getName() + "_val2");
    }

    //LLVMValueRef llvmVarRef = reinterpret_cast<LLVMValueRef>(variable);
    //LVMTypeRef llvmPointerType = LLVMTypeOf(llvmVarRef);
    //LLVMTypeRef llvmValueTypeRef = LLVMGetElementType(llvmPointerType);

    //llvm::Type* valueType = reinterpret_cast<llvm::Type*>(llvmValueTypeRef);

    //llvm::Value *loadedValue = builder->CreateLoad(valueType, variable, node.getName() + "_val");

    return nullptr;
}


llvm::Value* LLVMCodeGenVisitor::visit(IntegerLiteralNode &node) {
    return llvm::ConstantInt::get(getLLVMType(node.type), node.value);
    //else return llvm::ConstantInt::get(getLLVMType(node.type), node.value);
}
llvm::Value* LLVMCodeGenVisitor::visit(StringLiteralNode &node) {
    if (node.isConstantString()) {
        llvm::ArrayType *arrayType = llvm::ArrayType::get(llvm::Type::getInt8Ty(*context), node.getValue().length() + 1);
        llvm::GlobalVariable *stringVar = new llvm::GlobalVariable(
            *module,
            arrayType,
            node.isConstant,
            llvm::GlobalValue::PrivateLinkage,
            llvm::ConstantDataArray::getString(*context, node.value, true), 
            node.name 
        );

        std::vector<llvm::Constant*> chars;
        for (char c : node.getValue()) {
            chars.push_back(llvm::ConstantInt::get(llvm::Type::getInt8Ty(*context), c));
        }
        chars.push_back(llvm::ConstantInt::get(llvm::Type::getInt8Ty(*context), 0));

        llvm::Constant *initializer = llvm::ConstantArray::get(arrayType, chars);
        stringVar->setInitializer(initializer);


        llvm::Value *bitcastedStr = builder->CreateBitCast(stringVar, llvm::Type::getInt8Ty(*context)->getPointerTo());

        return bitcastedStr;
    } else {

        return builder->CreateGlobalStringPtr(node.getValue(), "string_literal");
    }
}

llvm::Value* LLVMCodeGenVisitor::visit(FunctionNode &node) {
    llvm::Type *returnType = getLLVMType(node.returnType);

    std::vector<llvm::Type*> paramTypes;
    for (const auto &param : node.parameters) {
        llvm::Type *paramType = getLLVMType(param->varType);
        paramTypes.push_back(paramType);
    }
    llvm::FunctionType *funcType = llvm::FunctionType::get(returnType, paramTypes, false);

    llvm::Function *function = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, node.functionName, module);

    llvm::BasicBlock *entryBlock = llvm::BasicBlock::Create(*context, "entry", function);
    builder->SetInsertPoint(entryBlock);

    unsigned idx = 0;
    for (auto &arg : function->args()) {
        arg.setName(node.parameters[idx]->varName);

        llvm::AllocaInst *alloca = builder->CreateAlloca(arg.getType(), nullptr, arg.getName());

        builder->CreateStore(&arg, alloca);

        symbolTable[arg.getName().str()] = alloca;

        idx++;
    }

    for (const auto &stmt : node.functionBody) {
        stmt->accept(*this);
    }

    if (!entryBlock->getTerminator()) {
        if (returnType->isVoidTy()) {
            builder->CreateRetVoid(); 
        } else {
            builder->CreateRet(llvm::Constant::getNullValue(returnType));
        }
    }


    llvm::verifyFunction(*function);

    return function;
}


llvm::Value* LLVMCodeGenVisitor::visit(FunctionInstantiationNode &node) {
    functionTypes[node.name] = node.returnType;
    if (node.isExtern) {
        llvm::FunctionType *funcType;
        if (node.hasVarArgs) {
            std::vector<llvm::Type*> paramTypes;
            for (const auto &param : node.parameters) {
                paramTypes.push_back(getLLVMType(param->varType));
            }
            funcType = llvm::FunctionType::get(
                getLLVMType(node.returnType),
                paramTypes,
                true 
            );
        } else {
            std::vector<llvm::Type*> paramTypes;
            for (const auto &param : node.parameters) {
                paramTypes.push_back(getLLVMType(param->varType));
            }
            funcType = llvm::FunctionType::get(
                getLLVMType(node.returnType),
                paramTypes,
                false
            );
        }

        llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, node.name, module);
        return nullptr; 
    }
    std::vector<llvm::Type*> paramTypes;
    for (const auto &param : node.parameters) {
        paramTypes.push_back(getLLVMType(param->varType));
    }
    llvm::FunctionType *funcType = llvm::FunctionType::get(
        getLLVMType(node.returnType),
        paramTypes,
        false 
    );

    llvm::Function *function = llvm::Function::Create(
        funcType, 
        llvm::Function::ExternalLinkage, 
        node.name, 
        module
    );

    unsigned idx = 0;
    for (auto &arg : function->args()) {
        arg.setName(node.parameters[idx]->varName);
        idx++;
    }

    return function;
}




llvm::Value* LLVMCodeGenVisitor::visit(Bitcast &node) {
    llvm::Value* value = node.expr->accept(*this);

    if (!value) {
        throw std::runtime_error("Bitcast source value is null.");
    }

    if (!node.destType) {
        throw std::runtime_error("Destination type for bitcast cannot be null.");
    }
    return builder->CreateBitCast(value, node.destType, "fmt_ptr");
}



llvm::Value* LLVMCodeGenVisitor::visit(FunctionCallNode &node) {
    auto it = functionTypes.find(node.functionName);
    
    if (it != functionTypes.end()) {
        llvm::Function *function = module->getFunction(node.functionName);
        if (!function) {
            throw std::runtime_error("Function not found: " + node.functionName);
        }
        
        std::vector<llvm::Value*> argValues;
        for (const auto& arg : node.arguments) {
            argValues.push_back(arg->accept(*this));
        }
        if (it->second == "void") {
            
            return builder->CreateCall(function, argValues);
        } else {
            return builder->CreateCall(function, argValues, "calltmp");
        }
    }
    return nullptr;
}




llvm::Value* LLVMCodeGenVisitor::visit(StructDeclarationNode &node) {
    if (structTypes.find(node.structName) != structTypes.end()) {
        return nullptr;
    }

    std::vector<llvm::Type*> memberTypes;
    for (const auto &member : node.members) {
        llvm::Type* llvmType = getLLVMType(member->varType);
        memberTypes.push_back(llvmType);
    }

    llvm::StructType *structType = llvm::StructType::create(*context, node.structName);
    structType->setBody(memberTypes, /*isPacked=*/false);


    structTypes[node.structName] = structType;

    return nullptr; 
}
llvm::Value* LLVMCodeGenVisitor::visit(StructInstantiationNode &node) {
    if (structTypes.find(node.structName) == structTypes.end()) {
        std::cerr << "Struct não declarada: " << node.structName << std::endl;
        return nullptr;
    }

    llvm::StructType *structType = structTypes[node.structName];
    
    llvm::AllocaInst *allocaInst = builder->CreateAlloca(structType, nullptr, node.structName);
    
    return allocaInst;
}




llvm::Value* LLVMCodeGenVisitor::visit(ConstructorNode &node) {
    llvm::StructType* structType = structTypes[node.associatedStruct];

    std::vector<llvm::Type*> paramTypes;
    for (const auto &param : node.parameters) {
        llvm::Type* llvmParamType = getLLVMType(param->varType);
        paramTypes.push_back(llvmParamType);
    }

    llvm::FunctionType *funcType = llvm::FunctionType::get(
        llvm::Type::getVoidTy(*context), paramTypes, /*isVarArg=*/false);

    llvm::Function *llvmFunc = llvm::Function::Create(
        funcType, llvm::Function::ExternalLinkage, node.functionName, module);

    llvmFunc->setName(node.associatedStruct + ".constructor");

    llvm::BasicBlock *entry = llvm::BasicBlock::Create(*context, "entry", llvmFunc);
    builder->SetInsertPoint(entry);

    llvm::Value* structInstance = allocatedStructs[node.associatedStruct];
    builder->CreateAlloca(structType, nullptr);

    for (size_t i = 0; i < node.parameters.size(); ++i) {
        llvm::Value* paramValue = node.parameters[i]->accept(*this);
        llvm::Value* memberPtr = builder->CreateStructGEP(structType, structInstance, i);
        builder->CreateStore(paramValue, memberPtr);
    }

    builder->CreateRetVoid();
    return llvmFunc;
}
llvm::Value* LLVMCodeGenVisitor::visit(MethodNode &node){
    return nullptr;
}

llvm::Value* LLVMCodeGenVisitor::visit(BinaryExpressionNode &node) {
    llvm::Value* left = node.left->accept(*this);
    llvm::Value* right = node.right->accept(*this);

    if (left->getType()->isPointerTy()) {
        auto varDecl = dynamic_cast<VariableReferenceNode*>(node.left.get());
        left = builder->CreateLoad(getLLVMType(varDecl->getType()), left, "load_left");
    }

    if (right->getType()->isPointerTy()) {
        auto varDecl = dynamic_cast<VariableReferenceNode*>(node.right.get());
        right = builder->CreateLoad(getLLVMType(varDecl->getType()), right, "load_right");
    }


    if (left->getType()->isHalfTy()) {
        llvm::Type* floatType = llvm::Type::getFloatTy(*context);
        left = builder->CreateFPExt(left, floatType, "promote_left");
    }

    if (right->getType()->isHalfTy()) {
        llvm::Type* floatType = llvm::Type::getFloatTy(*context);
        right = builder->CreateFPExt(right, floatType, "promote_right");
    }

    llvm::Type* leftType = left->getType();
    llvm::Type* rightType = right->getType();

    if (node.op == "+") {
        if (leftType->isFloatTy() && rightType->isFloatTy()) {
            return builder->CreateFAdd(left, right, "addtmp");
        } else if (leftType->isIntegerTy() && rightType->isIntegerTy()) {
            return builder->CreateAdd(left, right, "addtmp");
        }
    } else if (node.op == "-") {
        if (leftType->isFloatTy() && rightType->isFloatTy()) {
            return builder->CreateFSub(left, right, "subtmp");
        } else if (leftType->isIntegerTy() && rightType->isIntegerTy()) {
            return builder->CreateSub(left, right, "subtmp");
        }
    } else if (node.op == "*") {
        if (leftType->isFloatTy() && rightType->isFloatTy()) {
            return builder->CreateFMul(left, right, "multmp");
        } else if (leftType->isIntegerTy() && rightType->isIntegerTy()) {
            return builder->CreateMul(left, right, "multmp");
        }
    } else if (node.op == "/") {
        if (leftType->isFloatTy() && rightType->isFloatTy()) {
            return builder->CreateFDiv(left, right, "divtmp");
        } else if (leftType->isIntegerTy() && rightType->isIntegerTy()) {
            return builder->CreateSDiv(left, right, "divtmp");
        }
    } else if (node.op == "<") {
        if (leftType->isIntegerTy() && rightType->isIntegerTy()) {
            return builder->CreateICmpSLT(left, right, "cmp_lt");
        } else if (leftType->isFloatTy() && rightType->isFloatTy()) {
            return builder->CreateFCmpULT(left, right, "cmp_lt");
        }
    }  else if (node.op == ">") {
        if (leftType->isIntegerTy() && rightType->isIntegerTy()) {
            return builder->CreateICmpSGT(left, right, "cmp_gt");
        } else if (leftType->isFloatTy() && rightType->isFloatTy()) {
            return builder->CreateFCmpUGT(left, right, "cmp_gt");
        }
    } else if (node.op == "==") {
        if (leftType->isIntegerTy() && rightType->isIntegerTy()) {
            return builder->CreateICmpEQ(left, right, "cmp_eq");
        } else if (leftType->isFloatTy() && rightType->isFloatTy()) {
            return builder->CreateFCmpOEQ(left, right, "cmp_eq");
        }
    } else if (node.op == "!=") {
        if (leftType->isIntegerTy() && rightType->isIntegerTy()) {
            return builder->CreateICmpNE(left, right, "cmp_neq");
        } else if (leftType->isFloatTy() && rightType->isFloatTy()) {
            return builder->CreateFCmpONE(left, right, "cmp_neq");
        }
    } else if (node.op == "<=") {
        if (leftType->isIntegerTy() && rightType->isIntegerTy()) {
            return builder->CreateICmpSLE(left, right, "cmp_le");
        } else if (leftType->isFloatTy() && rightType->isFloatTy()) {
            return builder->CreateFCmpOLE(left, right, "cmp_le");
        }
    } else if (node.op == ">=") {
        if (leftType->isIntegerTy() && rightType->isIntegerTy()) {
            return builder->CreateICmpSGE(left, right, "cmp_ge");
        } else if (leftType->isFloatTy() && rightType->isFloatTy()) {
            return builder->CreateFCmpOGE(left, right, "cmp_ge");
        }
    } else if (node.op == "+=") {
        auto varDecl = dynamic_cast<VariableReferenceNode*>(node.left.get());
        if (varDecl) {
            llvm::Value* ptr = lookupVariable(varDecl->getName()); 
            if (ptr) {
                llvm::Value* currentValue = builder->CreateLoad(getLLVMType(varDecl->getType()), ptr, "current_value");
                llvm::Value* updatedValue;
                if (currentValue->getType()->isIntegerTy() && right->getType()->isIntegerTy()) {
                    updatedValue = builder->CreateAdd(currentValue, right, "updated_value");
                } else if (currentValue->getType()->isFloatTy() && right->getType()->isFloatTy()) {
                    updatedValue = builder->CreateFAdd(currentValue, right, "updated_value"); 
                } else {
                    throw std::runtime_error("Incompatible types for '+=' operation");
                }
                builder->CreateStore(updatedValue, ptr);

                return updatedValue;
            } else {
                throw std::runtime_error("Variable not found: " + varDecl->getName());
            }
        }
    }

    throw std::runtime_error("Invalid operand types for operation: " + node.op);
}





llvm::Value* LLVMCodeGenVisitor::visit(VariableReferenceNode &node) {
    llvm::Value* variableValue = lookupVariable(node.variableName);
    if (!variableValue) {
        throw std::runtime_error("Undefined variable: " + node.variableName);
    }

    return variableValue;
}

llvm::Value* LLVMCodeGenVisitor::visit(ReturnNode &node) {

    llvm::Value* returnValue = nullptr;
    if (node.returnValue) {
        returnValue = node.returnValue->accept(*this);
    }

    builder->CreateRet(returnValue);
    
    return nullptr; 
}


llvm::Value* LLVMCodeGenVisitor::visit(IfNode &node) {
    llvm::Function *function = builder->GetInsertBlock()->getParent();

    llvm::BasicBlock *thenBB = llvm::BasicBlock::Create(*context, "then", function);
    llvm::BasicBlock *elseBB = llvm::BasicBlock::Create(*context, "else", function);
    llvm::BasicBlock *afterBB = llvm::BasicBlock::Create(*context, "after_if", function);


    llvm::Value *condValue = node.condition->accept(*this);
    builder->CreateCondBr(condValue, thenBB, elseBB);

    builder->SetInsertPoint(thenBB);
    node.thenBlock->accept(*this);
    builder->CreateBr(afterBB);


    builder->SetInsertPoint(elseBB);
    if (node.elseBlock) {
        node.elseBlock->accept(*this);
    }
    builder->CreateBr(afterBB);


    builder->SetInsertPoint(afterBB);

    return nullptr; 
}


llvm::Value* LLVMCodeGenVisitor::visit(WhileNode &node) {
    llvm::Function *function = builder->GetInsertBlock()->getParent();

    llvm::BasicBlock *entryBB = builder->GetInsertBlock();
    llvm::BasicBlock *loopBB = llvm::BasicBlock::Create(*context, "loop", function);
    llvm::BasicBlock *bodyBB = llvm::BasicBlock::Create(*context, "body", function);
    llvm::BasicBlock *afterBB = llvm::BasicBlock::Create(*context, "after_loop", function);

    builder->CreateBr(loopBB);
    builder->SetInsertPoint(loopBB);

    llvm::Value *condValue = node.condition->accept(*this);
    builder->CreateCondBr(condValue, bodyBB, afterBB);

    builder->SetInsertPoint(bodyBB);
    node.body->accept(*this);

    builder->CreateBr(loopBB);

    builder->SetInsertPoint(afterBB);

    return nullptr; 
}


llvm::Value* LLVMCodeGenVisitor::visit(ForNode &node) {
    llvm::Function *function = builder->GetInsertBlock()->getParent();
    
    llvm::BasicBlock *entryBB = builder->GetInsertBlock();
    llvm::BasicBlock *loopBB = llvm::BasicBlock::Create(*context, "loop", function);
    llvm::BasicBlock *bodyBB = llvm::BasicBlock::Create(*context, "body", function);
    llvm::BasicBlock *afterBB = llvm::BasicBlock::Create(*context, "after_loop", function);

    node.initializer->accept(*this);

    builder->CreateBr(loopBB);
    builder->SetInsertPoint(loopBB);

    node.increment->accept(*this);

    llvm::Value *condValue = node.condition->accept(*this);
    builder->CreateCondBr(condValue, bodyBB, afterBB);

    builder->SetInsertPoint(bodyBB);
    node.body->accept(*this);


    builder->CreateBr(loopBB);

    builder->SetInsertPoint(afterBB);

    return nullptr;
}





llvm::Value* LLVMCodeGenVisitor::visit(AssignmentNode &node) {
    llvm::Value* exprValue = node.expression->accept(*this);

    llvm::Value* varValue = getVariable(node.variableName);

    if (!varValue) {
        throw std::runtime_error("Variable not found: " + node.variableName);
    }

    return builder->CreateStore(exprValue, varValue);
}

llvm::Value* LLVMCodeGenVisitor::visit(BlockNode &node) {
    llvm::Value* lastValue = nullptr;
    for (auto &stmt : node.statements) {
        lastValue = stmt->accept(*this);  
    }
    return lastValue;
}

