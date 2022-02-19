#include "AST/return.hpp"

// TODO
ReturnNode::ReturnNode(const uint32_t line, const uint32_t col,
                       AstNode *ex)
    : AstNode{line, col} {
        this->ex = ex;
    }

// TODO: You may use code snippets in AstDumper.cpp
void ReturnNode::print() {}

void ReturnNode::accept(AstNodeVisitor &p_visitor) {
    // TODO
    p_visitor.visit(*this);    
}

void ReturnNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if(ex != NULL)
        ex->accept(p_visitor);
}


