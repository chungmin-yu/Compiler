#include "AST/VariableReference.hpp"

#include <algorithm>

void VariableReferenceNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    auto visit_ast_node = [&](auto &ast_node) { ast_node->accept(p_visitor); };

    for_each(m_indices.begin(), m_indices.end(), visit_ast_node);
}

int VariableReferenceNode::getErrors()
{
    for(uint32_t i = 0; i < m_indices.size(); i++){
        if(m_indices[i]->getError() == 1){
            this->setError();
            return 1;
        }
    }
    return 0;
}

std::vector<PType::PrimitiveTypeEnum> VariableReferenceNode::getExpressions()
{
    std::vector<PType::PrimitiveTypeEnum> exprs;
    for(uint32_t i = 0; i < m_indices.size(); i++){
        exprs.push_back(m_indices[i]->getType().getPrimitiveType());
    }
    return exprs;
}