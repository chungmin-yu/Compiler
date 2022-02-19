#include "AST/print.hpp"

// TODO
PrintNode::PrintNode(const uint32_t line, const uint32_t col,
                     AstNode *ex)
    : AstNode{line, col} {
        this->ex = ex;
    }

// TODO: You may use code snippets in AstDumper.cpp
void PrintNode::print() {}

void PrintNode::accept(AstNodeVisitor &p_visitor) {
    // TODO
    p_visitor.visit(*this);    
}

void PrintNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    ex->accept(p_visitor);
}

