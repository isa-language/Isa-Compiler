#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <memory>

enum class ASTNodeType {
    PROGRAM,
    DECLARATION,
    VARIABLE_DECLARATION,
    FUNCTION_DECLARATION,
    STRUCT_DECLARATION,
    STATEMENT,
    EXPRESSION,
    LITERAL,
    IDENTIFIER,
    BINARY_EXPRESSION,
    FUNCTION_CALL,
    IF_STATEMENT,
    WHILE_STATEMENT,
    FOR_STATEMENT,
    SWITCH_STATEMENT,
    CASE,
    RETURN_STATEMENT,
    PRINT_STATEMENT,
    ASSIGNMENT_STATEMENT,
    STRUCT_EXPRESSION,
    ARRAY_EXPRESSION,
    UNKNOWN
};

class ASTNode {
public:
    ASTNodeType type;
    ASTNode(ASTNodeType t) : type(t) {}
    virtual ~ASTNode() = default;
};

class ProgramNode : public ASTNode {
public:
    std::vector<std::shared_ptr<ASTNode>> declarations;
    ProgramNode() : ASTNode(ASTNodeType::PROGRAM) {}
};

class DeclarationNode : public ASTNode {
public:
    std::shared_ptr<ASTNode> declaration;
    DeclarationNode() : ASTNode(ASTNodeType::DECLARATION) {}
};

class VariableDeclarationNode : public ASTNode {
public:
    std::string varType;
    std::string identifier;
    std::shared_ptr<ASTNode> expression;
    VariableDeclarationNode() : ASTNode(ASTNodeType::VARIABLE_DECLARATION) {}
};

class FunctionDeclarationNode : public ASTNode {
public:
    std::string returnType;
    std::string identifier;
    std::vector<std::shared_ptr<ASTNode>> parameters;
    std::vector<std::shared_ptr<ASTNode>> body;
    FunctionDeclarationNode() : ASTNode(ASTNodeType::FUNCTION_DECLARATION) {}
};

class StructDeclarationNode : public ASTNode {
public:
    std::string identifier;
    std::vector<std::shared_ptr<ASTNode>> members;
    std::shared_ptr<ASTNode> constructor;
    StructDeclarationNode() : ASTNode(ASTNodeType::STRUCT_DECLARATION) {}
};

class ExpressionNode : public ASTNode {
public:
    ExpressionNode() : ASTNode(ASTNodeType::EXPRESSION) {}
};

class LiteralNode : public ASTNode {
public:
    std::string value;
    LiteralNode(const std::string& val) : ASTNode(ASTNodeType::LITERAL), value(val) {}
};

class IdentifierNode : public ASTNode {
public:
    std::string name;
    IdentifierNode(const std::string& n) : ASTNode(ASTNodeType::IDENTIFIER), name(n) {}
};

class BinaryExpressionNode : public ASTNode {
public:
    std::shared_ptr<ASTNode> left;
    std::string op;
    std::shared_ptr<ASTNode> right;
    BinaryExpressionNode() : ASTNode(ASTNodeType::BINARY_EXPRESSION) {}
};

class FunctionCallNode : public ASTNode {
public:
    std::string identifier;
    std::vector<std::shared_ptr<ASTNode>> arguments;
    FunctionCallNode() : ASTNode(ASTNodeType::FUNCTION_CALL) {}
};

class IfStatementNode : public ASTNode {
public:
    std::shared_ptr<ASTNode> condition;
    std::vector<std::shared_ptr<ASTNode>> thenBranch;
    std::vector<std::shared_ptr<ASTNode>> elseBranch;
    IfStatementNode() : ASTNode(ASTNodeType::IF_STATEMENT) {}
};

class WhileStatementNode : public ASTNode {
public:
    std::shared_ptr<ASTNode> condition;
    std::vector<std::shared_ptr<ASTNode>> body;
    WhileStatementNode() : ASTNode(ASTNodeType::WHILE_STATEMENT) {}
};

class ForStatementNode : public ASTNode {
public:
    std::shared_ptr<ASTNode> initialization;
    std::shared_ptr<ASTNode> condition;
    std::shared_ptr<ASTNode> update;
    std::vector<std::shared_ptr<ASTNode>> body;
    ForStatementNode() : ASTNode(ASTNodeType::FOR_STATEMENT) {}
};

class SwitchStatementNode : public ASTNode {
public:
    std::shared_ptr<ASTNode> expression;
    std::vector<std::shared_ptr<ASTNode>> cases;
    SwitchStatementNode() : ASTNode(ASTNodeType::SWITCH_STATEMENT) {}
};

class CaseNode : public ASTNode {
public:
    std::shared_ptr<ASTNode> expression;
    std::vector<std::shared_ptr<ASTNode>> body;
    CaseNode() : ASTNode(ASTNodeType::CASE) {}
};

class ReturnStatementNode : public ASTNode {
public:
    std::shared_ptr<ASTNode> expression;
    ReturnStatementNode() : ASTNode(ASTNodeType::RETURN_STATEMENT) {}
};

class PrintStatementNode : public ASTNode {
public:
    std::shared_ptr<ASTNode> expression;
    PrintStatementNode() : ASTNode(ASTNodeType::PRINT_STATEMENT) {}
};

class AssignmentStatementNode : public ASTNode {
public:
    std::string identifier;
    std::shared_ptr<ASTNode> expression;
    AssignmentStatementNode() : ASTNode(ASTNodeType::ASSIGNMENT_STATEMENT) {}
};

class StructExpressionNode : public ASTNode {
public:
    std::string identifier;
    std::vector<std::pair<std::string, std::shared_ptr<ASTNode>>> fields;
    StructExpressionNode() : ASTNode(ASTNodeType::STRUCT_EXPRESSION) {}
};

class ArrayExpressionNode : public ASTNode {
public:
    std::vector<std::shared_ptr<ASTNode>> elements;
    ArrayExpressionNode() : ASTNode(ASTNodeType::ARRAY_EXPRESSION) {}
};
