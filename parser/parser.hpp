#include "../lexer/lexer.hpp"
#include <memory>
#include <string>
#include <vector>


enum class ExprAstType {
    PROGRAM, VARIABLE_DECLARATION, FUNCTION_DECLARATION, STRUCT_DECLARATION, 
    RETURN_STATEMENT, IF_STATEMENT, WHILE_STATEMENT, FOR_STATEMENT, SWITCH_STATEMENT, 
    PRINT_STATEMENT, ASSIGNMENT_STATEMENT, BINARY_EXPRESSION, LITERAL, IDENTIFIER, FUNCTION_CALL, CASE
};


class ExprAST {
    public:
    virtual ~ExprAST() = default;
    TokenType typo;
};

class NumberExprAST : public ExprAST {
    std::string val;
    public: 
    NumberExprAST( const std::string &val ) : val(val) { typo = TokenType::INTEGER_LITERAL; }
};

class VariableExprAST : public ExprAST {
    std::string val;
    public:
    VariableExprAST( const std::string &val ) : val(val) { typo =  TokenType::LET;  }
};

class BinaryExprAST : public ExprAST {
  char Op;
  std::unique_ptr<ExprAST> LHS, RHS;

public:
    BinaryExprAST(char Op, std::unique_ptr<ExprAST> LHS, std::unique_ptr<ExprAST> RHS) : Op(Op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}
};

class CallExprAST : public ExprAST {
    std::string Callee;
    std::vector<std::unique_ptr<ExprAST>> Args;

    public:
    CallExprAST(const std::string &Callee, std::vector<std::unique_ptr<ExprAST>> Args) : Callee(Callee), Args(std::move(Args)) { typo = TokenType::FUNCTION_CALL; }
};

class PrototypeAST : public ExprAST {
    std::string Name;
    std::vector<std::unique_ptr<ExprAST>> Args;

    public:
    PrototypeAST( const std::string &Name, std::vector<std::unique_ptr<ExprAST>> Args) : Name(Name), Args(std::move(Args)) { typo = TokenType::PROTOTYPEFUN; }

    const std::string &getName() const { return Name; }
};

class FunctionAST : public ExprAST {
    public:
    std::unique_ptr<PrototypeAST> Proto;
    std::unique_ptr<ExprAST> Body;
    TokenType returnType;
    FunctionAST(std::unique_ptr<PrototypeAST> Proto, std::unique_ptr<ExprAST> Body) : Proto(std::move(Proto)), Body(std::move(Body)) { typo = TokenType::FN; }
};

class StructDeclarationExprAST : public ExprAST {
    public: 
    std::string val;
    std::vector<std::unique_ptr<ExprAST>> membros;
    std::unique_ptr<FunctionAST> constructor;
    std::vector<std::unique_ptr<FunctionAST>> function;  
    StructDeclarationExprAST( const std::string &val, std::vector<std::unique_ptr<ExprAST>> Args ) : val(val) { typo = TokenType::STRUCT; }
};

class ReturnStatementNode : public ExprAST {
public:
    ReturnStatementNode() { typo = TokenType::RETURN;}
    std::shared_ptr<ExprAST> expression;
};