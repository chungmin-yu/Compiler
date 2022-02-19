#include "AST/VariableReference.hpp"
using namespace std;

// TODO
VariableReferenceNode::VariableReferenceNode(const uint32_t line, const uint32_t col,
                                             string p_name, vector<AstNode *>* el)
    : ExpressionNode{line, col}, name(p_name) {
        this->el = el;
    }

// TODO: You may use code snippets in AstDumper.cpp
void VariableReferenceNode::print() {}

void VariableReferenceNode::accept(AstNodeVisitor &p_visitor) {
    // TODO
    p_visitor.visit(*this);    
}

void VariableReferenceNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if(el != NULL){
        for(auto &expr : *el){
            expr->accept(p_visitor);
        }
    }
}

