#include "AST/assignment.hpp"


// TODO
AssignmentNode::AssignmentNode(const uint32_t line, const uint32_t col, 
                               AstNode *vr, AstNode *ex)
    : AstNode{line, col} {
        this->vr = vr;
        this->ex = ex;
    }

// TODO: You may use code snippets in AstDumper.cpp
void AssignmentNode::print() {}

void AssignmentNode::accept(AstNodeVisitor &p_visitor) {
    // TODO
    p_visitor.visit(*this);
}

void AssignmentNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    vr->accept(p_visitor);
    ex->accept(p_visitor);
}


