#include "AST/CompoundStatement.hpp"
#include "visitor/AstNodeVisitor.hpp"
using namespace std;

// TODO
CompoundStatementNode::CompoundStatementNode(const uint32_t line, const uint32_t col,
                                             vector<AstNode *> *dl, std::vector<AstNode *> *sl)
    : AstNode{line, col} {
        this->dl = dl;
        this->sl = sl;
    }

// TODO: You may use code snippets in AstDumper.cpp
void CompoundStatementNode::print() {}

void CompoundStatementNode::accept(AstNodeVisitor &p_visitor) {
    // TODO
    p_visitor.visit(*this);    
}

void CompoundStatementNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if(dl != NULL){
        for (auto &decl : *dl){
            decl->accept(p_visitor);
        }
    }
    
    if(sl != NULL){
        for (auto &stat : *sl){
            stat->accept(p_visitor);
        }
    }
}


