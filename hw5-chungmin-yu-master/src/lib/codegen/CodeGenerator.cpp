#include "codegen/CodeGenerator.hpp"
#include "visitor/AstNodeInclude.hpp"

#include <algorithm>
#include <cassert>
#include <cstdarg>
#include <cstdio>

CodeGenerator::CodeGenerator(const std::string source_file_name,
                             const std::string save_path,
                             const SymbolManager *const p_symbol_manager)
    : m_symbol_manager_ptr(p_symbol_manager),
      m_source_file_path(source_file_name) {
    // FIXME: assume that the source file is always xxxx.p
    const std::string &real_path =
        (save_path == "") ? std::string{"."} : save_path;
    auto slash_pos = source_file_name.rfind("/");
    auto dot_pos = source_file_name.rfind(".");

    if (slash_pos != std::string::npos) {
        ++slash_pos;
    } else {
        slash_pos = 0;
    }
    std::string output_file_path(
        real_path + "/" +
        source_file_name.substr(slash_pos, dot_pos - slash_pos) + ".S");
    m_output_file.reset(fopen(output_file_path.c_str(), "w"));
    assert(m_output_file.get() && "Failed to open output file");
}

static void dumpInstructions(FILE *p_out_file, const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(p_out_file, format, args);
    va_end(args);
}

void CodeGenerator::functionPrologue()
{
    offset = 0;
    offset -= 4;
    offset -= 4;
    dumpInstructions(m_output_file.get(), "    addi sp, sp, -128\n");     //move stack pointer to lower address to allocate a new stack
    dumpInstructions(m_output_file.get(), "    sw ra, 124(sp)\n");        //save return address of the caller function in the current stack
    dumpInstructions(m_output_file.get(), "    sw s0, 120(sp)\n");        //save frame pointer of the last stack in the current stack
    dumpInstructions(m_output_file.get(), "    addi s0, sp, 128\n");      //move frame pointer to the bottom of the current stack
}
void CodeGenerator::functionEpilogue(const char* f_name)
{
    dumpInstructions(m_output_file.get(), "    lw ra, 124(sp)\n");        //load return address saved in the current stack
    dumpInstructions(m_output_file.get(), "    lw s0, 120(sp)\n");        //move frame pointer back to the bottom of the last stack
    dumpInstructions(m_output_file.get(), "    addi sp, sp, 128\n");      //move stack pointer back to the top of the last stack
    dumpInstructions(m_output_file.get(), "    jr ra\n");                 //jump back to the caller function
    dumpInstructions(m_output_file.get(), "    .size %s, .-%s\n", f_name, f_name);
    offset += 4;
    offset += 4;
}

void CodeGenerator::visit(ProgramNode &p_program) {
    // Generate RISC-V instructions for program header
    // clang-format off
    //constexpr const char*const riscv_assembly_file_prologue =
    //    "    .file \"%s\"\n"
    //    "    .option nopic\n"
    //    ".section    .text\n"
    //    "    .align 2\n";
    // clang-format on
    //dumpInstructions(m_output_file.get(), riscv_assembly_file_prologue,
    //                 m_source_file_path.c_str());
    dumpInstructions(m_output_file.get(), "%s%s%s", "   .file \"", m_source_file_path.c_str(), "\"\n   .option nopic\n");

    // Reconstruct the hash table for looking up the symbol entry
    // Hint: Use symbol_manager->lookup(symbol_name) to get the symbol entry.
    m_symbol_manager_ptr->reconstructHashTableFromSymbolTable(
        p_program.getSymbolTable());

    

    auto visit_ast_node = [&](auto &ast_node) { ast_node->accept(*this); };
    for_each(p_program.getDeclNodes().begin(), p_program.getDeclNodes().end(),
             visit_ast_node);
    for_each(p_program.getFuncNodes().begin(), p_program.getFuncNodes().end(),
             visit_ast_node);

    dumpInstructions(m_output_file.get(), "\n.text\n");
    dumpInstructions(m_output_file.get(), "  .align  2\n");
    dumpInstructions(m_output_file.get(), "  .global main\n");
    dumpInstructions(m_output_file.get(), "  .type   main, @function\n");
  
    dumpInstructions(m_output_file.get(), "\nmain:\n");

    functionPrologue();
    const_cast<CompoundStatementNode &>(p_program.getBody()).accept(*this);
    functionEpilogue("main");

    if(str_var!=""){
        dumpInstructions(m_output_file.get(), "st:\n");
        dumpInstructions(m_output_file.get(), "%s%s%s", "  .string \"", str_var.c_str(), "\"\n");
    }

    // Remove the entries in the hash table
    m_symbol_manager_ptr->removeSymbolsFromHashTable(p_program.getSymbolTable());
}

void CodeGenerator::visit(DeclNode &p_decl) {
    p_decl.visitChildNodes(*this);
}

void CodeGenerator::visit(VariableNode &p_variable) {
    // global
    if(local == false){
        if(m_symbol_manager_ptr->lookup(p_variable.getName())->getKind() == SymbolEntry::KindEnum::kVariableKind){
            // global variable
            dumpInstructions(m_output_file.get(), "\n.comm %s, 4, 4\n", p_variable.getNameCString());
        }
        else if(m_symbol_manager_ptr->lookup(p_variable.getName())->getKind() == SymbolEntry::KindEnum::kConstantKind){
            // global constant
            dumpInstructions(m_output_file.get(), "\n.section    .rodata \n");
            dumpInstructions(m_output_file.get(), "    .align 2\n");
            dumpInstructions(m_output_file.get(), "    .globl %s\n", p_variable.getNameCString());
            dumpInstructions(m_output_file.get(), "    .type %s, @object\n", p_variable.getNameCString());
            dumpInstructions(m_output_file.get(), "%s:\n", p_variable.getNameCString());
            dumpInstructions(m_output_file.get(), "    .word %s \n", p_variable.getConstantPtr()->getConstantValueCString() );
        }
    }
    // local
    else{
        SymbolEntry *DeclEntry = (SymbolEntry *) m_symbol_manager_ptr->lookup(p_variable.getName());
        if(DeclEntry->getKind() == SymbolEntry::KindEnum::kVariableKind){
            // local variable
            offset -= 4;
            DeclEntry->setOffset(offset);
            //dumpInstructions(m_output_file.get(), "#Declare local const variable: %s, offset: %d\n", DeclEntry->getNameCString(), DeclEntry->getOffset());
        }
        else if(DeclEntry->getKind() == SymbolEntry::KindEnum::kConstantKind){
            // local constant
            offset -= 4;
            DeclEntry->setOffset(offset);
            //dumpInstructions(m_output_file.get(), "#Declare local const variable: %s, offset: %d\n", DeclEntry->getNameCString(), DeclEntry->getOffset());
            dumpInstructions(m_output_file.get(), "    addi t0, s0, %d\n", offset);
            dumpInstructions(m_output_file.get(), "    addi sp, sp, -4\n");
            dumpInstructions(m_output_file.get(), "    sw t0, 0(sp)\n");
            dumpInstructions(m_output_file.get(), "    li t0, %s\n", p_variable.getConstantPtr()->getConstantValueCString());
            dumpInstructions(m_output_file.get(), "    addi sp, sp, -4\n");
            dumpInstructions(m_output_file.get(), "    sw t0, 0(sp)\n");
            dumpInstructions(m_output_file.get(), "    lw t0, 0(sp)\n");
            dumpInstructions(m_output_file.get(), "    addi sp, sp, 4\n");
            dumpInstructions(m_output_file.get(), "    lw t1, 0(sp)\n");
            dumpInstructions(m_output_file.get(), "    addi sp, sp, 4\n");
            dumpInstructions(m_output_file.get(), "    sw t0, 0(t1)\n");
        }
        else if(DeclEntry->getKind() == SymbolEntry::KindEnum::kParameterKind){
            offset -= 4;
            DeclEntry->setOffset(offset);
        }
        else if(DeclEntry->getKind() == SymbolEntry::KindEnum::kLoopVarKind){
            offset -= 4;
            DeclEntry->setOffset(offset);
        }
    }
}

void CodeGenerator::visit(ConstantValueNode &p_constant_value) {
    if(p_constant_value.getTypePtr()->isInteger()){
        dumpInstructions(m_output_file.get(), "    li t0, %s\n", p_constant_value.getConstantValueCString());
        dumpInstructions(m_output_file.get(), "    addi sp, sp, -4\n");
        dumpInstructions(m_output_file.get(), "    sw t0, 0(sp)\n");
    }
    else if(p_constant_value.getTypePtr()->isBool()){
        if(p_constant_value.getConstantValueCString() == std::string("true")) {
            dumpInstructions(m_output_file.get(), "    li t0, 1\n");
        }
        else {
            dumpInstructions(m_output_file.get(), "    li t0, 0\n");
        }
        dumpInstructions(m_output_file.get(), "    addi sp, sp, -4\n");
        dumpInstructions(m_output_file.get(), "    sw t0, 0(sp)\n");
    }
    else if(p_constant_value.getTypePtr()->isString()){
        str_var = p_constant_value.getConstantValueCString();
        dumpInstructions(m_output_file.get(), "%s%s%s", "    lui t0, ", "%", "hi(st)\n");
        dumpInstructions(m_output_file.get(), "%s%s%s", "    addi a0, t0, ", "%", "lo(st)\n");
        dumpInstructions(m_output_file.get(), "    addi sp, sp, -4\n");
        dumpInstructions(m_output_file.get(), "    sw a0, 0(sp)\n");
    }
    else if(p_constant_value.getTypePtr()->isReal()){
        //dumpInstructions(m_output_file.get(), "%s%s%s", "    lui t0, ", "%", "hi(rv)\n");
        //dumpInstructions(m_output_file.get(), "%s%s%s", "    addi ft0,  ", "%", "lo(rv)(t0)\n");
        //dumpInstructions(m_output_file.get(), "    addi sp, sp, -4\n");
        //dumpInstructions(m_output_file.get(), "    fsw ft0, 0(sp)\n");
    } 
    
}

void CodeGenerator::visit(FunctionNode &p_function) {
    // Reconstruct the hash table for looking up the symbol entry
    m_symbol_manager_ptr->reconstructHashTableFromSymbolTable(
        p_function.getSymbolTable());

    local= true;
    dumpInstructions(m_output_file.get(), "\n.text\n");
    dumpInstructions(m_output_file.get(), "  .align  2\n");
    dumpInstructions(m_output_file.get(), "  .global %s\n", p_function.getNameCString());
    dumpInstructions(m_output_file.get(), "  .type   %s, @function\n", p_function.getNameCString());
    dumpInstructions(m_output_file.get(), "\n%s:\n", p_function.getNameCString());

    functionPrologue();
    int parametersNumber = 0;    
    for (auto &parameter : p_function.m_parameters) {
        parameter->accept(*this);
        parametersNumber += parameter->getVariables().size();
    }
    //dumpInstructions(m_output_file.get(), "#Function parameters number: %d\n", parametersNumber);
    if(parametersNumber <= 8){
        for(int i = 0; i < parametersNumber; i++){
            std::string parameterName = p_function.getSymbolTable()->getEntries().at(i)->getName();
            SymbolEntry *ParameterEntry = (SymbolEntry *) m_symbol_manager_ptr->lookup(parameterName);
            dumpInstructions(m_output_file.get(), "    sw a%d, %d(s0)\n", i, ParameterEntry->getOffset());
        }
    }
    else{
        int overEightSize = 4 * parametersNumber;
        for(int i = 0; i < parametersNumber; i++){
            std::string parameterName = p_function.getSymbolTable()->getEntries().at(i)->getName();
            SymbolEntry *ParameterEntry = (SymbolEntry *) m_symbol_manager_ptr->lookup(parameterName);
            // load parameter from FP
            dumpInstructions(m_output_file.get(), "    lw t0, %d(s0)\n", overEightSize - 4);
            dumpInstructions(m_output_file.get(), "    sw t0, %d(s0)\n", ParameterEntry->getOffset());
            overEightSize -= 4;
        }
    }
    if (p_function.m_body) {
        p_function.m_body->accept(*this);
    }
    functionEpilogue(p_function.getNameCString());

    //p_function.visitChildNodes(*this);

    // Remove the entries in the hash table
    local = false;
    m_symbol_manager_ptr->removeSymbolsFromHashTable(p_function.getSymbolTable());
}

void CodeGenerator::visit(CompoundStatementNode &p_compound_statement) {
    // Reconstruct the hash table for looking up the symbol entry
    m_symbol_manager_ptr->reconstructHashTableFromSymbolTable(
        p_compound_statement.getSymbolTable());

    local = true;

    p_compound_statement.visitChildNodes(*this);

    // Remove the entries in the hash table
    m_symbol_manager_ptr->removeSymbolsFromHashTable(
        p_compound_statement.getSymbolTable());
}

void CodeGenerator::visit(PrintNode &p_print) {
    //dumpInstructions(m_output_file.get(), "#Print start\n");
    p_print.visitChildNodes(*this);
    dumpInstructions(m_output_file.get(), "    lw a0, 0(sp)\n");
    dumpInstructions(m_output_file.get(), "    addi sp, sp, 4\n");
    if (p_print.m_target->getInferredType()->isInteger()){
        dumpInstructions(m_output_file.get(), "    jal ra, printInt\n");
    }
    else if (p_print.m_target->getInferredType()->isString()){
        dumpInstructions(m_output_file.get(), "    jal ra, printString\n");
    }
    else if (p_print.m_target->getInferredType()->isReal()){
        // flw
        //dumpInstructions(m_output_file.get(), "    jal ra, printReal\n");
    }
    //dumpInstructions(m_output_file.get(), "#Print end\n");
}

void CodeGenerator::visit(BinaryOperatorNode &p_bin_op) {
    p_bin_op.visitChildNodes(*this);
    // RHS
    dumpInstructions(m_output_file.get(), "    lw t0, 0(sp)\n");
    dumpInstructions(m_output_file.get(), "    addi sp, sp, 4\n");
    // LHS
    dumpInstructions(m_output_file.get(), "    lw t1, 0(sp)\n");
    dumpInstructions(m_output_file.get(), "    addi sp, sp, 4\n");

    int labelOut, labelFalse, labelTrue;
    switch (p_bin_op.getOp()) {
    case Operator::kPlusOp:
        dumpInstructions(m_output_file.get(), "    add t0, t1, t0 \n");
        break;
    case Operator::kMinusOp:
        dumpInstructions(m_output_file.get(), "    sub t0, t1, t0 \n");
        break;
    case Operator::kMultiplyOp:
        dumpInstructions(m_output_file.get(), "    mul t0, t1, t0 \n");
        break;
    case Operator::kDivideOp:
        dumpInstructions(m_output_file.get(), "    div t0, t1, t0 \n");
        break;
    case Operator::kModOp:
        dumpInstructions(m_output_file.get(), "    rem t0, t1, t0 \n");
        break;
    case Operator::kAndOp:
        labelOut = label++;
        labelFalse = label++;
        // LHS is false
        dumpInstructions(m_output_file.get(), "    beqz t1, L%d\n", labelFalse);
        // RHS is false
        dumpInstructions(m_output_file.get(), "    beqz t0, L%d\n", labelFalse);

        dumpInstructions(m_output_file.get(), "    li t0, 1\n");
        dumpInstructions(m_output_file.get(), "    j L%d\n", labelOut);
        // label false:
        dumpInstructions(m_output_file.get(), "L%d:\n", labelFalse);
        dumpInstructions(m_output_file.get(), "    li t0, 0\n");
        // label out:
        dumpInstructions(m_output_file.get(), "L%d:\n", labelOut);
        break;
    case Operator::kOrOp:
        labelOut = label++;
        labelTrue = label++;
        // LHS is true
        dumpInstructions(m_output_file.get(), "    bneqz t1, L%d\n", labelTrue);
        // RHS is true
        dumpInstructions(m_output_file.get(), "    bneqz t0, L%d\n", labelTrue);

        dumpInstructions(m_output_file.get(), "    li t0, 0\n");
        dumpInstructions(m_output_file.get(), "    j L%d\n", labelOut);
        // label true:
        dumpInstructions(m_output_file.get(), "L%d:\n", labelTrue);
        dumpInstructions(m_output_file.get(), "    li t0, 1\n");
        // label out:
        dumpInstructions(m_output_file.get(), "L%d:\n", labelOut);
        break;
    case Operator::kLessOp:
        // t0 = (t0 < t1) ? 1 : 0
        labelOut = label++;
        labelTrue = label++;
        dumpInstructions(m_output_file.get(), "    blt t1, t0, L%d\n", labelTrue);
        dumpInstructions(m_output_file.get(), "    li t0, 0\n");
        dumpInstructions(m_output_file.get(), "    j L%d\n", labelOut);
        dumpInstructions(m_output_file.get(), "L%d:\n", labelTrue);
        dumpInstructions(m_output_file.get(), "    li t0, 1\n");
        dumpInstructions(m_output_file.get(), "L%d:\n", labelOut);
        break;
    case Operator::kLessOrEqualOp:
        // t0 = (t0 < =t1) ? 1 : 0
        labelOut = label++;
        labelTrue = label++;
        dumpInstructions(m_output_file.get(), "    ble t1, t0, L%d\n", labelTrue);
        dumpInstructions(m_output_file.get(), "    li t0, 0\n");
        dumpInstructions(m_output_file.get(), "    j L%d\n", labelOut);
        dumpInstructions(m_output_file.get(), "L%d:\n", labelTrue);
        dumpInstructions(m_output_file.get(), "    li t0, 1\n");
        dumpInstructions(m_output_file.get(), "L%d:\n", labelOut);
        break;
    case Operator::kEqualOp:
        // t0 = (t0 = t1) ? 1 : 0
        labelOut = label++;
        labelTrue = label++;
        dumpInstructions(m_output_file.get(), "    beq t0, t1, L%d\n", labelTrue);
        dumpInstructions(m_output_file.get(), "    li t0, 0\n");
        dumpInstructions(m_output_file.get(), "    j L%d\n", labelOut);
        dumpInstructions(m_output_file.get(), "L%d:\n", labelTrue);
        dumpInstructions(m_output_file.get(), "    li t0, 1\n");
        dumpInstructions(m_output_file.get(), "L%d:\n", labelOut);
        break;
    case Operator::kGreaterOp:
        // t0 = (t0 > t1) ? 1 : 0
        labelOut = label++;
        labelTrue = label++;
        dumpInstructions(m_output_file.get(), "    bgt t1, t0, L%d\n", labelTrue);
        dumpInstructions(m_output_file.get(), "    li t0, 0\n");
        dumpInstructions(m_output_file.get(), "    j L%d\n", labelOut);
        dumpInstructions(m_output_file.get(), "L%d:\n", labelTrue);
        dumpInstructions(m_output_file.get(), "    li t0, 1\n");
        dumpInstructions(m_output_file.get(), "L%d:\n", labelOut);
        break;
    case Operator::kGreaterOrEqualOp:
        // t0 = (t0 >= t1) ? 1 : 0
        labelOut = label++;
        labelTrue = label++;
        dumpInstructions(m_output_file.get(), "    bge t1, t0, L%d\n", labelTrue);
        dumpInstructions(m_output_file.get(), "    li t0, 0\n");
        dumpInstructions(m_output_file.get(), "    j L%d\n", labelOut);
        dumpInstructions(m_output_file.get(), "L%d:\n", labelTrue);
        dumpInstructions(m_output_file.get(), "    li t0, 1\n");
        dumpInstructions(m_output_file.get(), "L%d:\n", labelOut);
        break;
    case Operator::kNotEqualOp:
        // t0 = (t0 <> t1) ? 1 : 0
        labelOut = label++;
        labelTrue = label++;
        dumpInstructions(m_output_file.get(), "    bne t0, t1, L%d\n", labelTrue);
        dumpInstructions(m_output_file.get(), "    li t0, 0\n");
        dumpInstructions(m_output_file.get(), "    j L%d\n", labelOut);
        dumpInstructions(m_output_file.get(), "L%d:\n", labelTrue);
        dumpInstructions(m_output_file.get(), "    li t0, 1\n");
        dumpInstructions(m_output_file.get(), "L%d:\n", labelOut);
        break;
    default:
        break;
    }

    dumpInstructions(m_output_file.get(), "    addi sp, sp, -4\n");
    dumpInstructions(m_output_file.get(), "    sw t0, 0(sp)\n");
   
}

void CodeGenerator::visit(UnaryOperatorNode &p_un_op) {
    p_un_op.visitChildNodes(*this);
    dumpInstructions(m_output_file.get(), "    lw t0, 0(sp)\n");
    dumpInstructions(m_output_file.get(), "    addi sp, sp, 4\n");
    switch (p_un_op.getOp()) {
    case Operator::kNegOp:
        dumpInstructions(m_output_file.get(), "    li t1, 0\n");
        dumpInstructions(m_output_file.get(), "    sub t0, t1, t0 \n");
        break;
    case Operator::kNotOp:
        dumpInstructions(m_output_file.get(), "    li t1, 1\n");
        dumpInstructions(m_output_file.get(), "    sub t0, t1, t0 \n");
        break;
    default:
        break;
    }
    dumpInstructions(m_output_file.get(), "    addi sp, sp, -4\n");
    dumpInstructions(m_output_file.get(), "    sw t0, 0(sp)\n");

}

void CodeGenerator::visit(FunctionInvocationNode &p_func_invocation) {
    //dumpInstructions(m_output_file.get(), "#Function invocation start\n");
    p_func_invocation.visitChildNodes(*this);    
    int argc = p_func_invocation.getArguments().size();
    int overEightSize = 0;
    if(argc <= 8){
        for(int i = argc; i > 0; i--){
            dumpInstructions(m_output_file.get(), "    lw a%d, 0(sp)\n", i-1);
            dumpInstructions(m_output_file.get(), "    addi sp, sp, 4\n");
        }
    }
    else{
        overEightSize = argc * 4;
    }

    dumpInstructions(m_output_file.get(), "    jal ra, %s\n", p_func_invocation.getNameCString());

    if(overEightSize > 0) {
        dumpInstructions(m_output_file.get(), "    addi sp, sp, %d\n", overEightSize);
    }

    SymbolEntry* FuncCallEntry = (SymbolEntry*)m_symbol_manager_ptr->lookup(p_func_invocation.getName());
    if (!FuncCallEntry->getTypePtr()->isVoid()){
        dumpInstructions(m_output_file.get(), "    mv t0, a0\n");
        dumpInstructions(m_output_file.get(), "    addi sp, sp, -4\n");
        dumpInstructions(m_output_file.get(), "    sw t0, 0(sp)\n");
    }    
    //dumpInstructions(m_output_file.get(), "#Function invocation end\n");
}

void CodeGenerator::visit(VariableReferenceNode &p_variable_ref) {
    p_variable_ref.visitChildNodes(*this);
    SymbolEntry* VariableEntry = (SymbolEntry*)m_symbol_manager_ptr->lookup(p_variable_ref.getName());
    //dumpInstructions(m_output_file.get(), "#Variable reference: %s, offset: %d\n", VariableEntry->getNameCString(), VariableEntry->getOffset());
    if(isRead == true){
        // write something => load address
        if(VariableEntry->getLevel() == 0){
            // global
            dumpInstructions(m_output_file.get(), "    la t0, %s\n", p_variable_ref.getNameCString());
            dumpInstructions(m_output_file.get(), "    addi sp, sp, -4\n");
            dumpInstructions(m_output_file.get(), "    sw t0, 0(sp)\n");
        }
        else{
            // local
            dumpInstructions(m_output_file.get(), "    addi t0, s0, %d\n", VariableEntry->getOffset() );
            dumpInstructions(m_output_file.get(), "    addi sp, sp, -4\n", p_variable_ref.getNameCString());
            dumpInstructions(m_output_file.get(), "    sw t0, 0(sp)\n");
        }
    }
    else{
        // read something => load value
        if(VariableEntry->getLevel() == 0){
            // global
            dumpInstructions(m_output_file.get(), "    la t0, %s\n", p_variable_ref.getNameCString());
            dumpInstructions(m_output_file.get(), "    lw t1, 0(t0)\n");
            dumpInstructions(m_output_file.get(), "    mv t0, t1\n");
            dumpInstructions(m_output_file.get(), "    addi sp, sp, -4\n");
            dumpInstructions(m_output_file.get(), "    sw t0, 0(sp)\n");
        }
        else{
            // local
            dumpInstructions(m_output_file.get(), "    lw t0, %d(s0)\n", VariableEntry->getOffset());
            dumpInstructions(m_output_file.get(), "    addi sp, sp, -4 \n");
            dumpInstructions(m_output_file.get(), "    sw t0, 0(sp)\n");
        }
    }
}

void CodeGenerator::visit(AssignmentNode &p_assignment) {
    //dumpInstructions(m_output_file.get(), "#Assignment start\n");
    isRead = true;
    p_assignment.m_lvalue->accept(*this);
    isRead = false;
    p_assignment.m_expr->accept(*this);

    dumpInstructions(m_output_file.get(), "    lw t0, 0(sp)\n");
    dumpInstructions(m_output_file.get(), "    addi sp, sp, 4\n");
    dumpInstructions(m_output_file.get(), "    lw t1, 0(sp)\n");
    dumpInstructions(m_output_file.get(), "    addi sp, sp, 4\n");
    dumpInstructions(m_output_file.get(), "    sw t0, 0(t1)\n");
    //dumpInstructions(m_output_file.get(), "#Assignment end\n");
}

void CodeGenerator::visit(ReadNode &p_read) {
    //dumpInstructions(m_output_file.get(), "#Read start\n");
    isRead = true;
    p_read.visitChildNodes(*this);
    isRead = false;
    dumpInstructions(m_output_file.get(), "    jal ra, readInt\n");
    dumpInstructions(m_output_file.get(), "    lw t0, 0(sp)\n");
    dumpInstructions(m_output_file.get(), "    addi sp, sp, 4\n");
    dumpInstructions(m_output_file.get(), "    sw a0, 0(t0)\n");
    //dumpInstructions(m_output_file.get(), "#Read end\n");
}

void CodeGenerator::visit(IfNode &p_if) {
    int labelIf = label++;
    int labelElse = label++;
    int labelOut = label++;

    p_if.m_condition->accept(*this);

    dumpInstructions(m_output_file.get(), "    lw t0, 0(sp)\n");
    dumpInstructions(m_output_file.get(), "    addi sp, sp, 4\n");
    dumpInstructions(m_output_file.get(), "    beqz t0, L%d\n", labelElse);

    //dumpInstructions(m_output_file.get(), "#Label If\n");
    dumpInstructions(m_output_file.get(), "L%d:\n", labelIf);
    p_if.m_body->accept(*this);
    dumpInstructions(m_output_file.get(), "    j L%d\n", labelOut);

    //dumpInstructions(m_output_file.get(), "#Label Else\n");
    dumpInstructions(m_output_file.get(), "L%d:\n", labelElse);
    if (p_if.m_else_body) {
        p_if.m_else_body->accept(*this);
    }
    //dumpInstructions(m_output_file.get(), "#Label Out\n");
    dumpInstructions(m_output_file.get(), "L%d:\n", labelOut);
}

void CodeGenerator::visit(WhileNode &p_while) {
    int labelLoop = label++;
    int labelOut = label++;
    // label loop:
    dumpInstructions(m_output_file.get(), "L%d:\n", labelLoop);
    p_while.m_condition->accept(*this);
    dumpInstructions(m_output_file.get(), "    lw t0, 0(sp)\n");
    dumpInstructions(m_output_file.get(), "    addi sp, sp, 4\n");
    dumpInstructions(m_output_file.get(), "    beqz t0, L%d\n", labelOut);
    p_while.m_body->accept(*this);
    dumpInstructions(m_output_file.get(), "    j L%d\n", labelLoop);
    // label out:
    dumpInstructions(m_output_file.get(), "L%d:\n", labelOut);
}

void CodeGenerator::visit(ForNode &p_for) {
    // Reconstruct the hash table for looking up the symbol entry
    m_symbol_manager_ptr->reconstructHashTableFromSymbolTable(
        p_for.getSymbolTable());

    int labelLoop = label++;
    int labelOut = label++;
    SymbolEntry* LoopVarEntry = (SymbolEntry *) m_symbol_manager_ptr->lookup(p_for.m_loop_var_decl->getVariables()[0]->getNameCString());
    //dumpInstructions(m_output_file.get(), "#For start\n");
    //dumpInstructions(m_output_file.get(), "#loop var name: %s\n", LoopVarEntry->getNameCString());

    p_for.m_loop_var_decl->accept(*this);
    p_for.m_init_stmt->accept(*this);

    dumpInstructions(m_output_file.get(), "L%d:\n", labelLoop);
    p_for.m_end_condition->accept(*this);
    dumpInstructions(m_output_file.get(), "    lw t0, 0(sp)\n");
    dumpInstructions(m_output_file.get(), "    addi sp, sp, 4\n");

    dumpInstructions(m_output_file.get(), "    lw t1, %d(s0)\n", LoopVarEntry->getOffset());
    dumpInstructions(m_output_file.get(), "    bge t1, t0, L%d\n", labelOut);

    p_for.m_body->accept(*this);

    dumpInstructions(m_output_file.get(), "    lw t1, %d(s0)\n", LoopVarEntry->getOffset());
    dumpInstructions(m_output_file.get(), "    addi t1, t1, 1\n");
    dumpInstructions(m_output_file.get(), "    sw t1, %d(s0)\n", LoopVarEntry->getOffset());
    dumpInstructions(m_output_file.get(), "    j L%d\n", labelLoop);

    //dumpInstructions(m_output_file.get(), "#For end\n");
    dumpInstructions(m_output_file.get(), "L%d:\n", labelOut);

    //p_for.visitChildNodes(*this);

    // Remove the entries in the hash table
    m_symbol_manager_ptr->removeSymbolsFromHashTable(p_for.getSymbolTable());
}

void CodeGenerator::visit(ReturnNode &p_return) {
    p_return.visitChildNodes(*this);
    //dumpInstructions(m_output_file.get(), "#Return start\n");
    dumpInstructions(m_output_file.get(), "    lw t0, 0(sp)\n");
    dumpInstructions(m_output_file.get(), "    addi sp, sp, 4\n");
    dumpInstructions(m_output_file.get(), "    mv a0, t0\n");
    //dumpInstructions(m_output_file.get(), "#Return end\n");
}
