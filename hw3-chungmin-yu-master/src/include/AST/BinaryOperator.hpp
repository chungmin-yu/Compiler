#ifndef __AST_BINARY_OPERATOR_NODE_H
#define __AST_BINARY_OPERATOR_NODE_H

#include "AST/expression.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <memory>
using namespace std;

class BinaryOperatorNode : public ExpressionNode {
  public:
    BinaryOperatorNode(const uint32_t line, const uint32_t col,
                       string p_op, AstNode* left, AstNode* right
                       /* TODO: operator, expressions */);
    ~BinaryOperatorNode() = default;

    const char *getOp() const { return op.c_str(); }
    void print() override;
    void accept(AstNodeVisitor &p_visitor)override;
    void visitChildNodes(AstNodeVisitor &p_visitor)override;
    
  private:
    // TODO: operator, expressions
    AstNode* left;
    AstNode* right;
    std::string op;
};

#endif
