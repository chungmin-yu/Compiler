#include "AST/if.hpp"

// TODO
IfNode::IfNode(const uint32_t line, const uint32_t col, 
               AstNode *ex, AstNode *ifcs, AstNode *elcs)
    : AstNode{line, col} {
        this->ex = ex;
        this->ifcs = ifcs;
        this->elcs = elcs;
    }

// TODO: You may use code snippets in AstDumper.cpp
void IfNode::print() {}

void IfNode::accept(AstNodeVisitor &p_visitor) {
    // TODO
    p_visitor.visit(*this);    
}

void IfNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    ex->accept(p_visitor);
    ifcs->accept(p_visitor);
    if(elcs != NULL)
        elcs->accept(p_visitor);
}

