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

#define DEFINE_VISITOR_METHOD(NodeType) \
  virtual llvm::Value* visit(NodeType &node) = 0;


class Visitor {
  DEFINE_VISITOR_METHOD(ExpressionStatementNode)
  DEFINE_VISITOR_METHOD(Bitcast)
  DEFINE_VISITOR_METHOD(VariableDeclarationNode)
  DEFINE_VISITOR_METHOD(VariableValueNode)
  DEFINE_VISITOR_METHOD(ArrayTypeNode)
  DEFINE_VISITOR_METHOD(VariableReferenceNode)
  DEFINE_VISITOR_METHOD(IntegerLiteralNode)
  DEFINE_VISITOR_METHOD(StringLiteralNode)
  DEFINE_VISITOR_METHOD(StructDeclarationNode)
  DEFINE_VISITOR_METHOD(StructInstantiationNode)
  DEFINE_VISITOR_METHOD(FunctionNode)
  DEFINE_VISITOR_METHOD(FunctionInstantiationNode)
  DEFINE_VISITOR_METHOD(FunctionCallNode)
  DEFINE_VISITOR_METHOD(ConstructorNode)
  DEFINE_VISITOR_METHOD(MethodNode)
  DEFINE_VISITOR_METHOD(BinaryExpressionNode)
  DEFINE_VISITOR_METHOD(ReturnNode)
  DEFINE_VISITOR_METHOD(IfNode)
  DEFINE_VISITOR_METHOD(WhileNode)
  DEFINE_VISITOR_METHOD(ForNode)
  DEFINE_VISITOR_METHOD(AssignmentNode)
  DEFINE_VISITOR_METHOD(BlockNode)
};

#define DEFINE_VISITOR_IMPLEMENTATION(NodeType) \
  llvm::Value* visit(NodeType &node) override;

class LLVMCodeGenVisitor : public Visitor {
public:
    llvm::IRBuilder<> *builder;
    llvm::LLVMContext *context;
    llvm::Module *module;
    std::unordered_map<std::string, llvm::StructType*> structTypes;
    std::unordered_map<std::string, std::string> functionTypes;
    std::unordered_map<std::string, llvm::Value*> allocatedStructs;
    std::unordered_map<std::string, llvm::Value*> symbolTable;

    std::unordered_map<std::string, llvm::Value*> variables;

  LLVMCodeGenVisitor(llvm::IRBuilder<> *builder, llvm::LLVMContext *context, llvm::Module *module) : builder(builder), context(context), module(module) {}

  DEFINE_VISITOR_IMPLEMENTATION(ExpressionStatementNode)
  DEFINE_VISITOR_IMPLEMENTATION(Bitcast)
  DEFINE_VISITOR_IMPLEMENTATION(VariableDeclarationNode)
  DEFINE_VISITOR_IMPLEMENTATION(VariableReferenceNode)
  DEFINE_VISITOR_IMPLEMENTATION(VariableValueNode)
  DEFINE_VISITOR_IMPLEMENTATION(ArrayTypeNode)
  DEFINE_VISITOR_IMPLEMENTATION(IntegerLiteralNode)
  DEFINE_VISITOR_IMPLEMENTATION(StringLiteralNode)
  DEFINE_VISITOR_IMPLEMENTATION(StructDeclarationNode)
  DEFINE_VISITOR_IMPLEMENTATION(StructInstantiationNode)
  DEFINE_VISITOR_IMPLEMENTATION(FunctionNode)
  DEFINE_VISITOR_IMPLEMENTATION(FunctionInstantiationNode)
  DEFINE_VISITOR_IMPLEMENTATION(FunctionCallNode)
  DEFINE_VISITOR_IMPLEMENTATION(ConstructorNode)
  DEFINE_VISITOR_IMPLEMENTATION(MethodNode)
  DEFINE_VISITOR_IMPLEMENTATION(BinaryExpressionNode)
  DEFINE_VISITOR_IMPLEMENTATION(ReturnNode)
  DEFINE_VISITOR_IMPLEMENTATION(IfNode)
  DEFINE_VISITOR_IMPLEMENTATION(WhileNode)
  DEFINE_VISITOR_IMPLEMENTATION(ForNode)
  DEFINE_VISITOR_IMPLEMENTATION(AssignmentNode)
  DEFINE_VISITOR_IMPLEMENTATION(BlockNode)

  
  llvm::Type* getLLVMType(const std::string &type);
  llvm::Value* getInitValueForType(const std::string &type);
  llvm::Value* lookupVariable(const std::string &name);
  llvm::Value* getVariable(const std::string &name);
  void addVariable(const std::string &name, llvm::Value *value);
};


#define DECLARE_ACCEPT_VISITOR \
    llvm::Value* accept(LLVMCodeGenVisitor &visitor) override;

enum class NodeType { Function, Allocation, Transaction, Borrow, OwnershipTransfer };

class ASTNode {
public:
    virtual ~ASTNode() = default;
    NodeType type;
    virtual llvm::Value* accept(class LLVMCodeGenVisitor &visitor) = 0;
};


class ExpressionStatementNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> expression;

    explicit ExpressionStatementNode(std::unique_ptr<ASTNode> expr)
        : expression(std::move(expr)) {}

    DECLARE_ACCEPT_VISITOR
};

class Bitcast : public  ASTNode {
    public:
    //llvm::Type* srcType;
    llvm::Type* destType;
    std::unique_ptr<ASTNode> expr;

    Bitcast( llvm::Type* dest, std::unique_ptr<ASTNode> expr)
        :  destType(dest), expr(std::move(expr)) {}

    DECLARE_ACCEPT_VISITOR

};

class ArrayTypeNode : public ASTNode {
public:
    std::string arrayType; 
    int size;
    std::vector<std::unique_ptr<ASTNode>> initializer; 

    ArrayTypeNode(const std::string &type, int size, std::vector<std::unique_ptr<ASTNode>> init)
        : arrayType(type), size(size), initializer(std::move(init)) {}

    DECLARE_ACCEPT_VISITOR
};



class IntegerLiteralNode : public ASTNode {
public:
    std::string type;
    int value;
    bool isInt = true;

    IntegerLiteralNode(const std::string &type, int val) : type(type),value(val) {}
    IntegerLiteralNode(const std::string &type = "i32") : type(type) , isInt(false){}

    DECLARE_ACCEPT_VISITOR
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

    DECLARE_ACCEPT_VISITOR
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

    DECLARE_ACCEPT_VISITOR
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
    DECLARE_ACCEPT_VISITOR
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
    explicit VariableValueNode(const std::string& name)
        : variableName(name) {}
    std::string getName() const {
        return variableName;
    }
    std::string getType() const {
        return varType;
    }

    DECLARE_ACCEPT_VISITOR
};


class FunctionCallNode : public ASTNode {
public:
    std::string functionName;
    std::vector<std::unique_ptr<ASTNode>> arguments;

    FunctionCallNode(const std::string &name, std::vector<std::unique_ptr<ASTNode>> args)
        : functionName(name), arguments(std::move(args)) {}
    FunctionCallNode(const std::string &name)
        : functionName(name) {}
    
    DECLARE_ACCEPT_VISITOR
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

    DECLARE_ACCEPT_VISITOR
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
        : functionName(name), returnType(retType), isConstructor(isConstructor), isMethod(isMethod), associatedStruct(structName), 
          parameters(std::move(params)), functionBody(std::move(body)) {}

    FunctionNode(const std::string &name, const std::string &retType)
        : functionName(name), returnType(retType) {}
    DECLARE_ACCEPT_VISITOR
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
    FunctionInstantiationNode(
        const std::string &name,
        const std::string &returnType,
        bool isExtern = false,
        bool hasVarArgs = false
    )
        : name(name),
          returnType(returnType),
          isExtern(isExtern),
          hasVarArgs(hasVarArgs) {}
          

    DECLARE_ACCEPT_VISITOR
};
/*
class ArgsFunctionNode : public ASTNode {
    std::vector<std::unique_ptr<VariableDeclarationNode>> arguments;
    llvm::Value* accept(LLVMCodeGenVisitor &visitor) override;
};
*/
class ConstructorNode : public FunctionNode {
public:
    ConstructorNode(const std::string &name, std::vector<std::unique_ptr<VariableDeclarationNode>> parameters, std::vector<std::unique_ptr<ASTNode>> functionBody)
        : FunctionNode(name, "void", std::move(parameters), std::move(functionBody)) {
    }

    DECLARE_ACCEPT_VISITOR

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

    DECLARE_ACCEPT_VISITOR
};


class BinaryExpressionNode : public ASTNode {
public:

    std::unique_ptr<ASTNode> left;   
    std::string op;                 
    std::unique_ptr<ASTNode> right; 

    BinaryExpressionNode(std::unique_ptr<ASTNode> l, const std::string& o, std::unique_ptr<ASTNode> r)
        : left(std::move(l)), op(o), right(std::move(r)) {}

    llvm::Type* getType();
    DECLARE_ACCEPT_VISITOR
};


class ReturnNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> returnValue;

    ReturnNode(std::unique_ptr<ASTNode> retVal)
        : returnValue(std::move(retVal)) {}

    DECLARE_ACCEPT_VISITOR;
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

    DECLARE_ACCEPT_VISITOR
};


class WhileNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> body;

    WhileNode(std::unique_ptr<ASTNode> cond, std::unique_ptr<ASTNode> b)
        : condition(std::move(cond)), body(std::move(b)) {}

    DECLARE_ACCEPT_VISITOR
};


class ForNode : public ASTNode {
public:
    std::unique_ptr<VariableDeclarationNode> initializer;
    std::unique_ptr<BinaryExpressionNode> condition;
    std::unique_ptr<BinaryExpressionNode> increment;
    std::unique_ptr<BlockNode> body;

    ForNode(std::unique_ptr<VariableDeclarationNode> init, std::unique_ptr<BinaryExpressionNode> cond, std::unique_ptr<BinaryExpressionNode> incr, std::unique_ptr<BlockNode> b)
        : initializer(std::move(init)), condition(std::move(cond)), increment(std::move(incr)), body(std::move(b)) {}

    DECLARE_ACCEPT_VISITOR
};

class AssignmentNode : public ASTNode {
public:
    std::string variableName;
    std::unique_ptr<BinaryExpressionNode> expression;

    AssignmentNode(const std::string &varName, std::unique_ptr<BinaryExpressionNode> expr)
        : variableName(varName), expression(std::move(expr)) {}

    DECLARE_ACCEPT_VISITOR
};

class BlockNode : public ASTNode {
public:
    std::vector<std::unique_ptr<ASTNode>> statements;

    BlockNode(std::vector<std::unique_ptr<ASTNode>> stmts)
        : statements(std::move(stmts)) {}

    DECLARE_ACCEPT_VISITOR
};


#endif // !IsaLLVM_AST
