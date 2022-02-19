#include "AST/for.hpp"
#include "visitor/AstNodeVisitor.hpp"

// TODO
ForNode::ForNode(const uint32_t line, const uint32_t col,
                 AstNode* de, AstNode* as, AstNode* ex, AstNode* cs)
    : AstNode{line, col} {
        this->de = de;
        this->as = as;
        this->ex = ex;
        this->cs = cs;
    }

// TODO: You may use code snippets in AstDumper.cpp
void ForNode::print() {}

void ForNode::accept(AstNodeVisitor &p_visitor) {
    // TODO
    p_visitor.visit(*this);
}

void ForNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    de->accept(p_visitor);
    as->accept(p_visitor);
    ex->accept(p_visitor);
    cs->accept(p_visitor);
}


