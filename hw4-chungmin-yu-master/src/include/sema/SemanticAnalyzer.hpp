#ifndef SEMA_SEMANTIC_ANALYZER_H
#define SEMA_SEMANTIC_ANALYZER_H

#include "visitor/AstNodeVisitor.hpp"
#include "sema/SymbolTable.hpp"

#define MAX_LINE_LENG       512
#define MAX_LINE_NUM        256
// scanner.l
extern char code[MAX_LINE_NUM][MAX_LINE_LENG];

class SemanticAnalyzer final : public AstNodeVisitor {
  private:
    // TODO: something like symbol manager (manage symbol tables)
    //       context manager, return type manager
    SymbolManager symbol_manager;
    FunctionManager function_manager;

  public:
    ~SemanticAnalyzer() = default;
    SemanticAnalyzer() = default;

    void visit(ProgramNode &p_program) override;
    void visit(DeclNode &p_decl) override;
    void visit(VariableNode &p_variable) override;
    void visit(ConstantValueNode &p_constant_value) override;
    void visit(FunctionNode &p_function) override;
    void visit(CompoundStatementNode &p_compound_statement) override;
    void visit(PrintNode &p_print) override;
    void visit(BinaryOperatorNode &p_bin_op) override;
    void visit(UnaryOperatorNode &p_un_op) override;
    void visit(FunctionInvocationNode &p_func_invocation) override;
    void visit(VariableReferenceNode &p_variable_ref) override;
    void visit(AssignmentNode &p_assignment) override;
    void visit(ReadNode &p_read) override;
    void visit(IfNode &p_if) override;
    void visit(WhileNode &p_while) override;
    void visit(ForNode &p_for) override;
    void visit(ReturnNode &p_return) override;

    // print error code function
    void printErrorCode(const uint32_t line, const uint32_t col)
    {
        fprintf(stderr, "    %s\n    ", code[line]);
        for(uint32_t i = 0; i < col - 1; i++)
            fprintf(stderr, " ");
        fprintf(stderr, "^\n");
    }

};

#endif
