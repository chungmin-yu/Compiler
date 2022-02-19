#include "AST/ConstantValue.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <sstream>

// TODO
ConstantValueNode::ConstantValueNode(const uint32_t line, const uint32_t col, 
                                     struct constValue *c_value)
    : ExpressionNode{line, col} {
        type = c_value->type;
        if(c_value->type == "integer")
            integer_value = c_value->integer_value;
        else if(c_value->type == "real")
            real_value = c_value->real_value;
        else if(c_value->type == "string")
            string_value = c_value->string_value;
        else if(c_value->type == "boolean")
            boolean_value = c_value->boolean_value;
}



const char *ConstantValueNode::getValue(){
    if(type == "integer")
        return (std::to_string(integer_value).c_str());
    else if(type == "real"){
        return (std::to_string(real_value).c_str());
    }
    else if(type == "string"){
        return string_value.c_str();
    }
    else if(type == "boolean"){
        if(boolean_value)
            return "true";
        else
            return "false";            
    }
}

// TODO: You may use code snippets in AstDumper.cpp
void ConstantValueNode::print() {}

void ConstantValueNode::accept(AstNodeVisitor &p_visitor) {
    // TODO
    p_visitor.visit(*this);    
}

void ConstantValueNode::visitChildNodes(AstNodeVisitor &p_visitor) {}

