#include "AST/while.hpp"

// TODO
WhileNode::WhileNode(const uint32_t line, const uint32_t col,
                     AstNode* ex, AstNode* cs)
    : AstNode{line, col} {
        this->ex = ex;
        this->cs = cs;
    }

// TODO: You may use code snippets in AstDumper.cpp
void WhileNode::print() {}

void WhileNode::accept(AstNodeVisitor &p_visitor) {
    // TODO
    p_visitor.visit(*this);    
}

void WhileNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    ex->accept(p_visitor);
    cs->accept(p_visitor);
}

