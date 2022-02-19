#ifndef __AST_COMPOUND_STATEMENT_NODE_H
#define __AST_COMPOUND_STATEMENT_NODE_H

#include "AST/ast.hpp"
#include "visitor/AstNodeVisitor.hpp"
using namespace std;

class CompoundStatementNode : public AstNode {
  public:
    CompoundStatementNode(const uint32_t line, const uint32_t col,
                          vector<AstNode *> *dl, std::vector<AstNode *> *sl
                          /* TODO: declarations, statements */);
    ~CompoundStatementNode() = default;

    void print() override;
    void accept(AstNodeVisitor &p_visitor)override;
    void visitChildNodes(AstNodeVisitor &p_visitor)override;    

  private:
    // TODO: declarations, statements
    vector<AstNode *> *dl;
    vector<AstNode *> *sl;
};

#endif
