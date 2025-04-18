#pragma once

#ifndef IsaLLVM_AST_RENDER_TYPE
#define IsaLLVM_AST_RENDER_TYPE
#include "frontend/parser/ast.hpp"
#include <memory>
#include <cstdlib>

/*
#define CREATE_VAR_DECL_NODE(VAR_NAME, VAR_TYPE, ...) \
    std::make_unique<VariableDeclarationNode>(VAR_NAME, VAR_TYPE, __VA_ARGS__)
*/


#define CREATE_VAR_DECL_NODE(VAR_NAME, VAR_TYPE, INITIALISE) \
    return std::make_unique<VariableDeclarationNode>(VAR_NAME, VAR_TYPE, std::move(INITIALISE));

#define CREATE_VAR_NODE(VAR_NAME, VAR_TYPE) \
    return std::make_unique<VariableDeclarationNode>(VAR_NAME, VAR_TYPE);

#define CREATE_FUNC_NODE(FUNC_NAME, RET_TYPE, PARAMS, BODY) \
    std::make_unique<FunctionNode>(FUNC_NAME, RET_TYPE, std::move(PARAMS), std::move(BODY));

#define CREATE_STRING_LITERAL(VAL, NAME, CONSTANT) \
    std::make_unique<StringLiteralNode>(VAL, NAME, CONSTANT);

#endif  
