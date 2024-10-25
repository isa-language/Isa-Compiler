#pragma once 
#include <algorithm>
#include <llvm/IR/Type.h>
#ifndef IsaLLVM_AST
#define IsaLLVM_AST
#include <utility>
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
class VariableReferenceNode;
class VariableValueNode;
class ArrayTypeNode;
class IntegerLiteralNode;
class StringLiteralNode;
class StructDeclarationNode;
class StructInstantiationNode;
class FunctionNode;
class FunctionInstantiationNode;
class FunctionCallNode;
class ConstructorNode;
class MethodNode;
class BinaryExpressionNode;
class ReturnNode;
class IfNode;
class WhileNode;
class ForNode;
class ExpressionStatementNode;
class Bitcast;
class AssignmentNode;
class BlockNode;

class Visitor {
  virtual llvm::Value* visit(ExpressionStatementNode &node) = 0;
  virtual llvm::Value* visit(Bitcast &node) = 0;
  virtual llvm::Value* visit(VariableDeclarationNode &node) = 0;
  virtual llvm::Value* visit(VariableValueNode &node) = 0;

  virtual llvm::Value* visit(ArrayTypeNode &node) = 0;
  virtual llvm::Value* visit(VariableReferenceNode &node) = 0;
  virtual llvm::Value* visit(IntegerLiteralNode &node) = 0;
  virtual llvm::Value* visit(StringLiteralNode &node) = 0;
  virtual llvm::Value* visit(StructDeclarationNode &node) = 0;
  virtual llvm::Value* visit(StructInstantiationNode &node) = 0;
  virtual llvm::Value* visit(FunctionNode &node) = 0;
  virtual llvm::Value* visit(FunctionInstantiationNode &node) = 0;
  virtual llvm::Value* visit(FunctionCallNode &node) = 0;
  virtual llvm::Value* visit(ConstructorNode &node) = 0;
  virtual llvm::Value* visit(MethodNode &node) = 0;
  virtual llvm::Value* visit(BinaryExpressionNode &node) = 0;
  virtual llvm::Value* visit(ReturnNode &node) = 0;
  virtual llvm::Value* visit(IfNode &node) = 0;
  virtual llvm::Value* visit(WhileNode &node) = 0;
  virtual llvm::Value* visit(ForNode &node) = 0;
  virtual llvm::Value* visit(AssignmentNode &node) = 0;
  virtual llvm::Value* visit(BlockNode &node) = 0;  
};

class LLVMCodeGenVisitor : public Visitor {
public:
    llvm::IRBuilder<> *builder;
    llvm::LLVMContext *context;
    llvm::Module *module;
    std::unordered_map<std::string, llvm::StructType*> structTypes;
    std::unordered_map<std::string, llvm::Value*> allocatedStructs;
    std::unordered_map<std::string, llvm::Value*> symbolTable;

    std::unordered_map<std::string, llvm::Value*> variables;

  LLVMCodeGenVisitor(llvm::IRBuilder<> *builder, llvm::LLVMContext *context, llvm::Module *module) : builder(builder), context(context), module(module) {}

  llvm::Value* visit(ExpressionStatementNode &node) override;
  llvm::Value* visit(Bitcast &node) override;
  llvm::Value* visit(VariableDeclarationNode &node) override;
  llvm::Value* visit(VariableReferenceNode &node) override;
  llvm::Value* visit(VariableValueNode &node) override;
  llvm::Value* visit(ArrayTypeNode &node) override;
  llvm::Value* visit(IntegerLiteralNode &node) override;
  llvm::Value* visit(StringLiteralNode &node) override;
  llvm::Value* visit(StructDeclarationNode &node) override;
  llvm::Value* visit(StructInstantiationNode &node) override;
  llvm::Value* visit(FunctionNode &node) override;
  llvm::Value* visit(FunctionInstantiationNode &node) override;
  llvm::Value* visit(FunctionCallNode &node) override;
  llvm::Value* visit(ConstructorNode &node) override;
  llvm::Value* visit(MethodNode &node) override;
  llvm::Value* visit(BinaryExpressionNode &node) override;
  llvm::Value* visit(ReturnNode &node) override;
  llvm::Value* visit(IfNode &node) override;
  llvm::Value* visit(WhileNode &node) override;
  llvm::Value* visit(ForNode &node) override;
  llvm::Value* visit(AssignmentNode &node) override;
  llvm::Value* visit(BlockNode &node) override;

  
  llvm::Type* getLLVMType(const std::string &type);
  llvm::Type* getLLVMTypeFromASTType(const std::string &type);
  llvm::Value* getInitValueForType(const std::string &type);
  llvm::Value* lookupVariable(const std::string &name);
  llvm::Value* getVariable(const std::string &name);
  void addVariable(const std::string &name, llvm::Value *value);
};


class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual llvm::Value* accept(class LLVMCodeGenVisitor &visitor) = 0;
};

class ExpressionStatementNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> expression;

    explicit ExpressionStatementNode(std::unique_ptr<ASTNode> expr)
        : expression(std::move(expr)) {}

    llvm::Value* accept(LLVMCodeGenVisitor &visitor) override;
};

class Bitcast : public  ASTNode {
    public:
    //llvm::Type* srcType;
    llvm::Type* destType;
    std::unique_ptr<ASTNode> expr;

    Bitcast( llvm::Type* dest, std::unique_ptr<ASTNode> expr)
        :  destType(dest), expr(std::move(expr)) {}

    llvm::Value* accept(LLVMCodeGenVisitor &visitor) override;

};

class ArrayTypeNode : public ASTNode {
public:
    std::string arrayType; 
    int size;
    std::vector<std::unique_ptr<ASTNode>> initializer; 

    ArrayTypeNode(const std::string &type, int size, std::vector<std::unique_ptr<ASTNode>> init)
        : arrayType(type), size(size), initializer(std::move(init)) {}

    llvm::Value* accept(LLVMCodeGenVisitor &visitor) override;
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
    int index;
    
    std::unique_ptr<ASTNode> initializer;

    VariableDeclarationNode(const std::string &name, const std::string &type, std::unique_ptr<ASTNode> init)
        : varName(name), varType(type), initializer(std::move(init)) {}
    VariableDeclarationNode(const std::string &name, const std::string &type)
        : varName(name), varType(type), initializer(nullptr) {}

    VariableDeclarationNode(const std::string &name, const std::string &type,int index, std::unique_ptr<ASTNode> init)
        : varName(name), varType(type), index(index), initializer(std::move(init)) {}

    llvm::Value* accept(LLVMCodeGenVisitor &visitor) override;
};

class StringLiteralNode : public ASTNode {
public:
    std::string value;
    std::string name;
    bool isConstant; 

StringLiteralNode(const std::string &val, bool constant = false)
        : value(val), isConstant(constant) {}
    StringLiteralNode(const std::string &val, const std::string& name, bool constant)
        : value(val), name(name), isConstant(constant) {}

    const std::string& getValue() const {
        return value;
    }

    bool isConstantString() const {
        return isConstant;
    }

    llvm::Value* accept(LLVMCodeGenVisitor &visitor) override;
};


class VariableReferenceNode : public ASTNode {
public:
    std::string variableName;
    std::string varType;

    explicit VariableReferenceNode(const std::string& name) : variableName(name) {}
    explicit VariableReferenceNode(const std::string& name,const std::string& varType) : variableName(name), varType(varType) {}

    std::string getName() const {
        return variableName; 
    }
    llvm::Value* accept(LLVMCodeGenVisitor &visitor) override;
    std::string getType() const {
        return varType;
    }
};

class VariableValueNode : public ASTNode {
public:
    std::string variableName;
    std::string varType;

    explicit VariableValueNode(const std::string& name, const std::string& type)
        : variableName(name), varType(type) {}

    std::string getName() const {
        return variableName;
    }
    std::string getType() const {
        return varType;
    }

    llvm::Value* accept(LLVMCodeGenVisitor &visitor) override;
};


class FunctionCallNode : public ASTNode {
public:
    std::string functionName;
    std::vector<std::unique_ptr<ASTNode>> arguments;

    FunctionCallNode(const std::string &name, std::vector<std::unique_ptr<ASTNode>> args)
        : functionName(name), arguments(std::move(args)) {}
    
    llvm::Value* accept(LLVMCodeGenVisitor &visitor) override;
};


class StructInstantiationNode : public ASTNode {
public:
    std::string structName;  

    StructInstantiationNode(const std::string &name)
        : structName(name) {}

    llvm::Value* accept(LLVMCodeGenVisitor &visitor) override;
};


class StructDeclarationNode : public ASTNode {
public:
    std::string structName;
    std::vector<std::unique_ptr<VariableDeclarationNode>> members;
     std::vector<std::unique_ptr<FunctionNode>> methods;

    StructDeclarationNode(const std::string &name, std::vector<std::unique_ptr<VariableDeclarationNode>> member)
        : structName(name), members(std::move(member)) {}
    StructDeclarationNode(const std::string &name, std::vector<std::unique_ptr<VariableDeclarationNode>> member, std::vector<std::unique_ptr<FunctionNode>> methods)
        : structName(name), members(std::move(member)), methods(std::move(methods)) {}

    llvm::Value* accept(LLVMCodeGenVisitor &visitor) override;
};


class FunctionNode : public ASTNode {
public:
    std::string functionName;
    std::string returnType;
    bool isConstructor = false;
    bool isMethod = false;
    std::string associatedStruct;  
    std::vector<std::unique_ptr<VariableDeclarationNode>> parameters;
    std::vector<std::unique_ptr<ASTNode>> functionBody;

    FunctionNode(const std::string &name, const std::string &retType, std::vector<std::unique_ptr<VariableDeclarationNode>> params, 
                 std::vector<std::unique_ptr<ASTNode>> body)
        : functionName(name), returnType(retType), parameters(std::move(params)), functionBody(std::move(body)) {}

    FunctionNode(const std::string &name, const std::string &retType, std::vector<std::unique_ptr<VariableDeclarationNode>> params, 
                 std::vector<std::unique_ptr<ASTNode>> body, const std::string &structName, bool isMethod = false, bool isConstructor = false)
        : functionName(name), returnType(retType), isMethod(isMethod), isConstructor(isConstructor), associatedStruct(structName), 
          parameters(std::move(params)), functionBody(std::move(body)) {}

    FunctionNode(const std::string &name, const std::string &retType)
        : functionName(name), returnType(retType) {}
    llvm::Value* accept(LLVMCodeGenVisitor &visitor) override;
};

class FunctionInstantiationNode : public ASTNode {
public:
    std::string name;
    std::string returnType;
    std::vector<std::unique_ptr<VariableDeclarationNode>> parameters;  
    bool isExtern; 
    bool hasVarArgs; 

    FunctionInstantiationNode(
        const std::string &name,
        const std::string &returnType,
        std::vector<std::unique_ptr<VariableDeclarationNode>> params,
        bool isExtern = false,
        bool hasVarArgs = false
    )
        : name(name),
          returnType(returnType),
          parameters(std::move(params)),
          isExtern(isExtern),
          hasVarArgs(hasVarArgs) {}

    llvm::Value* accept(LLVMCodeGenVisitor &visitor) override;
};


class ConstructorNode : public FunctionNode {
public:
    ConstructorNode(const std::string &name, std::vector<std::unique_ptr<VariableDeclarationNode>> parameters, std::vector<std::unique_ptr<ASTNode>> functionBody)
        : FunctionNode(name, "void", std::move(parameters), std::move(functionBody)) {
    }

    llvm::Value* accept(LLVMCodeGenVisitor &visitor) override {
        return visitor.visit(*this);
    }

    bool isConstructor() {
        return true;
    }
};

class MethodNode : public FunctionNode {
public:
    MethodNode(const std::string &methodName, const std::string &returnType,
               std::vector<std::unique_ptr<VariableDeclarationNode>> params,
               std::vector<std::unique_ptr<ASTNode>> body,
               const std::string &structName)
        : FunctionNode(methodName, returnType, std::move(params), std::move(body), structName, true) {
        isMethod = true; 
    }

    llvm::Value* accept(LLVMCodeGenVisitor &visitor) override {
        return visitor.visit(*this);
    }
};


class BinaryExpressionNode : public ASTNode {
public:

    std::unique_ptr<ASTNode> left;   
    std::string op;                 
    std::unique_ptr<ASTNode> right; 

    BinaryExpressionNode(std::unique_ptr<ASTNode> l, const std::string& o, std::unique_ptr<ASTNode> r)
        : left(std::move(l)), op(o), right(std::move(r)) {}

    llvm::Type* getType();
    llvm::Value* accept(LLVMCodeGenVisitor& visitor) override;
};


class ReturnNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> returnValue;

    ReturnNode(std::unique_ptr<ASTNode> retVal)
        : returnValue(std::move(retVal)) {}

    llvm::Value* accept(LLVMCodeGenVisitor &visitor) override;
    const ASTNode* getReturnValue() const {
        return returnValue.get();
    }
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
    std::unique_ptr<VariableDeclarationNode> initializer;
    std::unique_ptr<BinaryExpressionNode> condition;
    std::unique_ptr<BinaryExpressionNode> increment;
    std::unique_ptr<BlockNode> body;

    ForNode(std::unique_ptr<VariableDeclarationNode> init, std::unique_ptr<BinaryExpressionNode> cond, std::unique_ptr<BinaryExpressionNode> incr, std::unique_ptr<BlockNode> b)
        : initializer(std::move(init)), condition(std::move(cond)), increment(std::move(incr)), body(std::move(b)) {}

    llvm::Value* accept(LLVMCodeGenVisitor &visitor) override;
};

class AssignmentNode : public ASTNode {
public:
    std::string variableName;
    std::unique_ptr<BinaryExpressionNode> expression;

    AssignmentNode(const std::string &varName, std::unique_ptr<BinaryExpressionNode> expr)
        : variableName(varName), expression(std::move(expr)) {}

    llvm::Value* accept(LLVMCodeGenVisitor &visitor) override;
};

class BlockNode : public ASTNode {
public:
    std::vector<std::unique_ptr<ASTNode>> statements;

    BlockNode(std::vector<std::unique_ptr<ASTNode>> stmts)
        : statements(std::move(stmts)) {}

    llvm::Value* accept(LLVMCodeGenVisitor &visitor) override;
};


#endif // !IsaLLVM_AST
