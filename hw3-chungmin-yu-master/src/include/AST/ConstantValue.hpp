#ifndef __AST_CONSTANT_VALUE_NODE_H
#define __AST_CONSTANT_VALUE_NODE_H

#include "AST/expression.hpp"
#include "visitor/AstNodeVisitor.hpp"
using namespace std;

class ConstantValueNode : public ExpressionNode {
  public:
    ConstantValueNode(const uint32_t line, const uint32_t col, struct constValue *c_value);
    ~ConstantValueNode() = default;

    const char *getValue();

    void print() override;
    void accept(AstNodeVisitor &p_visitor) override;
    void visitChildNodes(AstNodeVisitor &p_visitor)override;

  private:
    // TODO: constant value
    string type;
    int integer_value;
    double real_value;
    string string_value;
    bool boolean_value ;    
    
};

#endif
