#include "AST/BinaryOperator.hpp"
#include "visitor/AstNodeVisitor.hpp"
using namespace std;

// TODO
BinaryOperatorNode::BinaryOperatorNode(const uint32_t line, const uint32_t col,
                                       string p_op, AstNode* left, AstNode* right)
    : ExpressionNode{line, col}, op(p_op){
        this->left = left;
        this->right = right;
    }

// TODO: You may use code snippets in AstDumper.cpp
void BinaryOperatorNode::print() {}

void BinaryOperatorNode::accept(AstNodeVisitor &p_visitor) {
    // TODO
    p_visitor.visit(*this);
}

void BinaryOperatorNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    left->accept(p_visitor);
    right->accept(p_visitor);
}

