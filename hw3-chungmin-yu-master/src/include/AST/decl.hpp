#ifndef __AST_DECL_NODE_H
#define __AST_DECL_NODE_H

#include "AST/ast.hpp"
#include "AST/variable.hpp"
#include "visitor/AstNodeVisitor.hpp"
using namespace std;

class DeclNode : public AstNode {
  public:
    // variable declaration
    DeclNode(const uint32_t line, const uint32_t col, 
             vector<string> *idl, vector<string> *p_type
             /* TODO: identifiers, type */);

    // constant variable declaration
    DeclNode(const uint32_t line, const uint32_t col, 
             vector<string> *idl, struct constValue *constant
             /* TODO: identifiers, constant */);

    ~DeclNode() = default;

    void print() override;
    void accept(AstNodeVisitor &p_visitor)override;
    void visitChildNodes(AstNodeVisitor &p_visitor)override;
    

    const char *getPrototypeCString(); 

    // TODO: variables
    vector<AstNode *> *vr;
    vector<VariableNode *> *vrfunc;
};

#endif
