#ifndef __AST_FUNCTION_NODE_H
#define __AST_FUNCTION_NODE_H

#include "AST/ast.hpp"
#include "AST/decl.hpp"
#include "visitor/AstNodeVisitor.hpp"
using namespace std;

class FunctionNode : public AstNode {
  public:
    FunctionNode(const uint32_t line, const uint32_t col, char *p_name,
                 vector<DeclNode *> * dl, char *p_ret , AstNode *cs
                 /* TODO: name, declarations, return type,
                  *       compound statement (optional) */);
    ~FunctionNode() = default;

    void print() override;
    void accept(AstNodeVisitor &p_visitor)override;
    void visitChildNodes(AstNodeVisitor &p_visitor)override;
    
    const char *getName() const { return name.c_str(); }; 
    const char *getType() const; 
    

  private:
    // TODO: name, declarations, return type, compound statement
    string name;
    vector<DeclNode *> *dl;
    char* ret;
    AstNode *cs;
};

#endif
