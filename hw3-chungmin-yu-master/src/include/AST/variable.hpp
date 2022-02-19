#ifndef __AST_VARIABLE_NODE_H
#define __AST_VARIABLE_NODE_H

#include "AST/ast.hpp"
#include "visitor/AstNodeVisitor.hpp"
using namespace std;

class VariableNode : public AstNode {
  public:
    VariableNode(const uint32_t line, const uint32_t col, 
                 const char * p_name, vector<string> *p_type, AstNode* constant
                 /* TODO: variable name, type, constant value */);

    ~VariableNode() = default;

    const char *getName() { return name.c_str(); };
    const char *getType();

    void print() override;
    void accept(AstNodeVisitor &p_visitor)override;
    void visitChildNodes(AstNodeVisitor &p_visitor)override;    

  private:
    // TODO: variable name, type, constant value
    const string name;
    vector<string> *type;
    AstNode* constant;
};

#endif
