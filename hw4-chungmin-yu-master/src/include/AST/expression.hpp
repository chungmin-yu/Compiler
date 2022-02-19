#ifndef AST_EXPRESSION_NODE_H
#define AST_EXPRESSION_NODE_H

#include "AST/PType.hpp"
#include "AST/ast.hpp"

class ExpressionNode : public AstNode {
  public:
    ~ExpressionNode() = default;
    ExpressionNode(const uint32_t line, const uint32_t col)
        : AstNode{line, col} {}

    PType getType() { return *type; }
    PTypeSharedPtr getTypeSharedPtr() { return type; }
    void setType(PTypeSharedPtr p_type){ type = p_type; }
    void setError() { error = 1; }
    int getError(){ return error; }

  protected:
    // for carrying type of result of an expression
    // TODO: for next assignment
    int error = 0;
    PTypeSharedPtr type = nullptr;    
};

#endif
