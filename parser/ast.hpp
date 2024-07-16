#pragma once
#include <vector>
#include <memory>
#include <string>
#include <iostream>

class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void print(int indent = 0) const = 0;
};

class ProgramNode : public ASTNode {
public:
    std::vector<std::unique_ptr<ASTNode>> declarations;

    void print(int indent = 0) const override {
        for (const auto& decl : declarations) {
            decl->print(indent);
        }
    }
};

class VariableDeclarationNode : public ASTNode {
public:
    std::string type;
    std::string identifier;
    std::unique_ptr<ASTNode> expression;

    VariableDeclarationNode(const std::string& type, const std::string& identifier, std::unique_ptr<ASTNode> expression)
        : type(type), identifier(identifier), expression(std::move(expression)) {}

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "VariableDeclaration: " << type << " " << identifier << std::endl;
        if (expression) {
            expression->print(indent + 2);
        }
    }
};

class FunctionDeclarationNode : public ASTNode {
public:
    std::string identifier;
    std::string returnType;
    std::unique_ptr<ASTNode> body;

    FunctionDeclarationNode(const std::string& identifier, const std::string& returnType, std::unique_ptr<ASTNode> body)
        : identifier(identifier), returnType(returnType), body(std::move(body)) {}

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "FunctionDeclaration: " << identifier << " -> " << returnType << std::endl;
        if (body) {
            body->print(indent + 2);
        }
    }
};

class StructDeclarationNode : public ASTNode {
public:
    std::string identifier;

    StructDeclarationNode(const std::string& identifier) : identifier(identifier) {}

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "StructDeclaration: " << identifier << std::endl;
    }
};

class BlockNode : public ASTNode {
public:
    std::vector<std::unique_ptr<ASTNode>> statements;

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "Block" << std::endl;
        for (const auto& stmt : statements) {
            stmt->print(indent + 2);
        }
    }
};
