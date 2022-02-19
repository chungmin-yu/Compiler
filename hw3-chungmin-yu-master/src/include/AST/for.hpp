#ifndef __AST_FOR_NODE_H
#define __AST_FOR_NODE_H

#include "AST/ast.hpp"
#include "visitor/AstNodeVisitor.hpp"

class ForNode : public AstNode {
  public:
    ForNode(const uint32_t line, const uint32_t col,
            AstNode* de, AstNode* as, AstNode* ex, AstNode* cs
            /* TODO: declaration, assignment, expression,
             *       compound statement */);
    ~ForNode() = default;

    void print() override;
    void accept(AstNodeVisitor &p_visitor)override;
    void visitChildNodes(AstNodeVisitor &p_visitor)override;    

  private:
    // TODO: declaration, assignment, expression, compound statement
    AstNode* de;
    AstNode* as;
    AstNode* ex;
    AstNode* cs;
};

#endif
