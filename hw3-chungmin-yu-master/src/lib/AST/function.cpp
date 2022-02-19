#include "AST/function.hpp"
#include "visitor/AstNodeVisitor.hpp"

// TODO
FunctionNode::FunctionNode(const uint32_t line, const uint32_t col, 
                          char *p_name, vector<DeclNode *> *dl, char *p_ret , AstNode *cs)
    : AstNode{line, col}, name(p_name), ret(p_ret) {
        this->dl = dl;
        this->cs = cs;
    }

// TODO: You may use code snippets in AstDumper.cpp
void FunctionNode::print() {}

const char *FunctionNode::getType() const {
    char * temp = new char [512];
    strcat(temp, ret);
    strcat(temp, " (");
    if(dl != NULL){
        for(int i = 0; i < dl->size(); i++){
                if(i != 0){
                    strcat(temp, ", ");
                }
                strcat(temp, dl->at(i)->getPrototypeCString());
            }
    }
    strcat(temp, ")");
    return temp;
}

void FunctionNode::accept(AstNodeVisitor &p_visitor) {
    // TODO
    p_visitor.visit(*this);    
}

void FunctionNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if(dl != NULL){
        for (auto &decl : *dl){
            decl->accept(p_visitor);
        }
    }
    if(cs != NULL)
        cs->accept(p_visitor);
}



