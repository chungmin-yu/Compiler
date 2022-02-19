#include "AST/program.hpp"
using namespace std;

// TODO
ProgramNode::ProgramNode(const uint32_t line, const uint32_t col,
                         const char *p_name, const char *p_ret, vector<AstNode *> * dl, vector<AstNode *> * fl, AstNode *cs)
    : AstNode{line, col}, name(p_name), ret(p_ret) {
        this->dl = dl;
        this->fl = fl;
        this->cs = cs;
    }

void ProgramNode::print() {
    // TODO
    // outputIndentationSpace();

    std::printf("program <line: %u, col: %u> %s %s\n",
                location.line, location.col,
                name.c_str(), "void");

    // TODO
    // incrementIndentation();
    // visitChildNodes();
    // decrementIndentation();    
}

void ProgramNode::accept(AstNodeVisitor &p_visitor) {
    // TODO
    p_visitor.visit(*this);    
}

void ProgramNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if(dl != NULL){
        for (auto &decl : *dl){
            decl->accept(p_visitor);
        }
    }
    if(fl != NULL){
        for (auto &func : *fl){
            func->accept(p_visitor);
        }
    }
    cs->accept(p_visitor);
}


