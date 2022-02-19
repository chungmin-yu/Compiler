#ifndef AST_FOR_NODE_H
#define AST_FOR_NODE_H

#include "AST/ast.hpp"
#include "AST/decl.hpp"
#include "AST/assignment.hpp"
#include "AST/expression.hpp"
#include "AST/CompoundStatement.hpp"

class ForNode final : public AstNode {
  public:
    std::unique_ptr<DeclNode> m_loop_var_decl;
    std::unique_ptr<AssignmentNode> m_init_stmt;
    std::unique_ptr<ConstantValueNode> m_end_condition;
    std::unique_ptr<CompoundStatementNode> m_body;
    int m_left;

  public:
    ~ForNode() = default;
    ForNode(const uint32_t line, const uint32_t col,
            DeclNode *p_loop_var_decl, AssignmentNode *p_init_stmt,
            ConstantValueNode *p_end_condition, CompoundStatementNode *p_body, int p_left)
        : AstNode{line, col}, m_loop_var_decl(p_loop_var_decl),
          m_init_stmt(p_init_stmt), m_end_condition(p_end_condition),
          m_body(p_body), m_left(p_left) {}

    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;
};

#endif
