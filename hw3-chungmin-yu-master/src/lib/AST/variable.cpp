#include "AST/variable.hpp"
using namespace std;

// TODO
VariableNode::VariableNode(const uint32_t line, const uint32_t col, 
                           const char * p_name, vector<string> *p_type, AstNode* constant)
    : AstNode{line, col}, name(p_name) {
        type = p_type;
        this->constant = constant;
    }

// TODO: You may use code snippets in AstDumper.cpp
void VariableNode::print() {}

const char *VariableNode::getType(){ 
    char * temp = new char [512];
    for(int i = 0; i < type->size(); i++){
        if (i == 0 && type->size() > 1){
            strcat(temp, (*type)[i].c_str());
            strcat(temp, " ");
        }else if(i == 0){
            strcat(temp, (*type)[i].c_str());
        }else{
            strcat(temp, "[");
            strcat(temp, (*type)[i].c_str());
            strcat(temp, "]");
        }
    }
    return temp;
}

void VariableNode::accept(AstNodeVisitor &p_visitor) {
    // TODO
    p_visitor.visit(*this);    
}

void VariableNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if(constant != NULL)
        constant->accept(p_visitor);
}

