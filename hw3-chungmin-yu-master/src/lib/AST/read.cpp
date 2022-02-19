#include "AST/read.hpp"

// TODO
ReadNode::ReadNode(const uint32_t line, const uint32_t col, 
                   AstNode *vr)
    : AstNode{line, col} {
        this->vr = vr;
    }

// TODO: You may use code snippets in AstDumper.cpp
void ReadNode::print() {}

void ReadNode::accept(AstNodeVisitor &p_visitor) {
    // TODO
    p_visitor.visit(*this);
}

void ReadNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    vr->accept(p_visitor);
}



