#ifndef __AST_WHILE_NODE_H
#define __AST_WHILE_NODE_H

#include "AST/ast.hpp"
#include "visitor/AstNodeVisitor.hpp"

class WhileNode : public AstNode {
  public:
    WhileNode(const uint32_t line, const uint32_t col,
              AstNode* ex, AstNode* cs
              /* TODO: expression, compound statement */);
    ~WhileNode() = default;

    void print() override;
    void accept(AstNodeVisitor &p_visitor)override;
    void visitChildNodes(AstNodeVisitor &p_visitor)override;
    

  private:
    // TODO: expression, compound statement
    AstNode* ex;
    AstNode* cs;
};

#endif
