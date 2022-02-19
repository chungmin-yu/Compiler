#ifndef __AST_FUNCTION_INVOCATION_NODE_H
#define __AST_FUNCTION_INVOCATION_NODE_H

#include "AST/expression.hpp"
#include "visitor/AstNodeVisitor.hpp"
using namespace std;

class FunctionInvocationNode : public ExpressionNode {
  public:
    FunctionInvocationNode(const uint32_t line, const uint32_t col,
                           string p_name, vector<AstNode *>* el
                           /* TODO: function name, expressions */);
    ~FunctionInvocationNode() = default;

    const char *getName() const { return name.c_str(); }

    void print() override;
    void accept(AstNodeVisitor &p_visitor)override;
    void visitChildNodes(AstNodeVisitor &p_visitor)override;    

  private:
    // TODO: function name, expressions
    string name;
    vector<AstNode *>* el;
};

#endif
