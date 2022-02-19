#include "AST/FunctionInvocation.hpp"

#include <algorithm>

void FunctionInvocationNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    auto visit_ast_node = [&](auto &ast_node) { ast_node->accept(p_visitor); };

    for_each(m_args.begin(), m_args.end(), visit_ast_node);
}

std::vector<PType> FunctionInvocationNode::getParaType(){
    std::vector<PType> para;
    for (auto &argument : m_args) {
        para.push_back(argument->getType());
    }
    return para;
}