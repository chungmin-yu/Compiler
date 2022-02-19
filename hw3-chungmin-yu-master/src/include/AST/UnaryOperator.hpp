#ifndef __AST_UNARY_OPERATOR_NODE_H
#define __AST_UNARY_OPERATOR_NODE_H

#include "AST/expression.hpp"
#include "visitor/AstNodeVisitor.hpp"
using namespace std;

class UnaryOperatorNode : public ExpressionNode {
  public:
    UnaryOperatorNode(const uint32_t line, const uint32_t col,
                      string p_op, AstNode* ex
                      /* TODO: operator, expression */);
    ~UnaryOperatorNode() = default;

    const char *getOp() const { return op.c_str(); }

    void print() override;
    void accept(AstNodeVisitor &p_visitor)override;
    void visitChildNodes(AstNodeVisitor &p_visitor)override;    

  private:
    // TODO: operator, expression
    AstNode* ex;
    string op;
};

#endif
