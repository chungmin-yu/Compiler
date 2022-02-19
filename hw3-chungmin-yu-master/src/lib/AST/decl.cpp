#include "AST/decl.hpp"
#include "AST/variable.hpp"
#include "AST/ConstantValue.hpp"
#include "visitor/AstNodeVisitor.hpp"
using namespace std;

DeclNode::DeclNode(const uint32_t line, const uint32_t col, 
                   vector<string> *idl, vector<string> *p_type)
    : AstNode{line, col} {
        // TODO
        vr = new vector<AstNode *>;
        vrfunc = new vector<VariableNode *>;
        for (int i = 0; i < idl->size(); i+=2){
            AstNode *NewNode = new VariableNode(line, stoi(idl->at(i+1)), idl->at(i).c_str(), p_type, NULL);
            vr->push_back(NewNode);
            VariableNode *NewNodefunc = new VariableNode(line, stoi(idl->at(i+1)), idl->at(i).c_str(), p_type, NULL);
            vrfunc->push_back(NewNodefunc);
        }
    }

DeclNode::DeclNode(const uint32_t line, const uint32_t col, 
                   vector<string> *idl, struct constValue *constant)
    : AstNode{line, col} {
        // TODO
        vr = new vector<AstNode *>;
        for (int i = 0; i < idl->size(); i+=2){
            AstNode *Newconst = new ConstantValueNode(line, constant->col, constant);
            vector<string> *temp = new vector<string> ();
            temp->push_back(constant->type);
            AstNode *NewNode = new VariableNode(line, stoi(idl->at(i+1)), idl->at(i).c_str(), temp, Newconst);            
            vr->push_back(NewNode);
        }
    }


const char *DeclNode::getPrototypeCString(){
    char * temp = new char [100];
    if(vrfunc != NULL){
        for(int i = 0; i < vrfunc->size(); i++){
                if(i != 0){
                    strcat(temp, ", ");
                }
                strcat(temp, vrfunc->at(i)->getType());
            }
    }
    return temp;
}

// TODO: You may use code snippets in AstDumper.cpp
void DeclNode::print() {}

void DeclNode::accept(AstNodeVisitor &p_visitor) {
    // TODO
    p_visitor.visit(*this);    
}

void DeclNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    for (auto &vari : *vr){
        vari->accept(p_visitor);
    }
}


