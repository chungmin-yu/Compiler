#ifndef __AST_PROGRAM_NODE_H
#define __AST_PROGRAM_NODE_H

#include "AST/ast.hpp"
#include "visitor/AstNodeVisitor.hpp"
using namespace std;

class ProgramNode : public AstNode {
  private:
    const string name;
    const string ret;
    vector<AstNode *> *dl;
    vector<AstNode *> *fl;
    AstNode *cs;
    // TODO: return type, declarations, functions, compound statement

  public:
    ProgramNode(const uint32_t line, const uint32_t col,
                const char *p_name, const char *p_ret, vector<AstNode *> * dl, vector<AstNode *> * fl, AstNode *cs
                /* TODO: return type, declarations, functions,
                 *       compound statement */);
    ~ProgramNode() = default;

    // visitor pattern version: 
    const char *getNameCString() const { return name.c_str(); }; 

    void print() override;
    void accept(AstNodeVisitor &p_visitor)override;
    void visitChildNodes(AstNodeVisitor &p_visitor)override;    

  
};

#endif
