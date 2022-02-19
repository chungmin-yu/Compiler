#include "AST/program.hpp"
#include "AST/AstDumper.hpp"
#include "AST/CompoundStatement.hpp"

#include <algorithm>

extern bool inFunction;

void ProgramNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    auto visit_ast_node = [&](auto &ast_node) { ast_node->accept(p_visitor); };

    for_each(m_decl_nodes.begin(), m_decl_nodes.end(), visit_ast_node);
    inFunction = true;
    for_each(m_func_nodes.begin(), m_func_nodes.end(), visit_ast_node);
    inFunction = false;
    visit_ast_node(m_body);
}
