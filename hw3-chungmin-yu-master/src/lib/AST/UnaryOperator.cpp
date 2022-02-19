#include "AST/UnaryOperator.hpp"
using namespace std;

// TODO
UnaryOperatorNode::UnaryOperatorNode(const uint32_t line, const uint32_t col,
                                     string p_op, AstNode* ex)
    : ExpressionNode{line, col}, op(p_op) {
        this->ex = ex;
    }

// TODO: You may use code snippets in AstDumper.cpp
void UnaryOperatorNode::print() {}

void UnaryOperatorNode::accept(AstNodeVisitor &p_visitor) {
    // TODO
    p_visitor.visit(*this);    
}

void UnaryOperatorNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    ex->accept(p_visitor);
}

