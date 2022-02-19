#include "sema/SemanticAnalyzer.hpp"
#include "visitor/AstNodeInclude.hpp"

using namespace std;

// scanner.l
extern int32_t opt_symbol;
// parser.y
extern bool error;
extern bool inFunction;

void SemanticAnalyzer::visit(ProgramNode &p_program) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    //1
    SymbolTable* newtable = new SymbolTable(TABLE_PROGRAM);
    symbol_manager.pushScope(newtable);
    //2
    symbol_manager.addSymbol(p_program.getNameCString(), ENTRY_PROGRAM, PType::PrimitiveTypeEnum::kVoidType, "", 0);    
    vector<PType> tmp1;
    PType tmp2(PType::PrimitiveTypeEnum::kVoidType);
    function_manager.addFunction(p_program.getNameCString(), tmp1, tmp2);
    //3
    p_program.visitChildNodes(*this);
    //4 skip
    //5
    symbol_manager.popScope(opt_symbol);
}

void SemanticAnalyzer::visit(DeclNode &p_decl) {
    p_decl.visitChildNodes(*this);
}

void SemanticAnalyzer::visit(VariableNode &p_variable) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    int e = 0;
    int put = 0;
    int kind = 0;
    if(symbol_manager.lookup(p_variable.getNameCString(), false) != ENTRY_NOTFOUND || symbol_manager.Lookup(p_variable.getNameCString())){
        fprintf(stderr, "<Error> Found in line %d, column %d: ", p_variable.getLocation().line, p_variable.getLocation().col);
        fprintf(stderr, "symbol '%s' is redeclared\n", p_variable.getNameCString());
        printErrorCode(p_variable.getLocation().line, p_variable.getLocation().col);
        e = 1;
        error = 1;
    }
    vector<uint64_t> dims = p_variable.getType().getDimensions();
    for(uint32_t i = 0; i < dims.size(); i++){
        if(dims[i] <= 0){
            fprintf(stderr, "<Error> Found in line %d, column %d: ", p_variable.getLocation().line, p_variable.getLocation().col);
            fprintf(stderr, "'%s' declared as an array with an index that is not greater than 0\n", p_variable.getNameCString());
            printErrorCode(p_variable.getLocation().line, p_variable.getLocation().col);
            put = 1;
            error = 1;
        }
    }

    if(p_variable.m_constant_value_node_ptr != NULL){
        kind = ENTRY_CONSTANT;
    }else if(symbol_manager.getTopKind() == TABLE_FUNCTION){
        //in function declaration
        kind = ENTRY_PARAMETER;
    }else if(symbol_manager.getTopKind() == TABLE_FORLOOP){
        //in for loop varible
        kind = ENTRY_LOOPVAR;
    }else{
        kind = ENTRY_VARIABLE;
    }

    //1 skip
    //2
    if(e == 0 && put == 0){
        symbol_manager.addSymbol(p_variable.getNameCString(), kind, p_variable.getType(), p_variable.getConstantCString(), 0);
    }
    else if(e == 0 && put == 1) {
        symbol_manager.addSymbol(p_variable.getNameCString(), kind, p_variable.getType(), p_variable.getConstantCString(), 1);
    }
    //3
    p_variable.visitChildNodes(*this);
    //4 5 skip
}

void SemanticAnalyzer::visit(ConstantValueNode &p_constant_value) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    //1 skip
    //2 skip
    //3
    p_constant_value.visitChildNodes(*this);
    //4
    p_constant_value.setType(p_constant_value.getTypeSharedPtr());
    //5 skip
}

void SemanticAnalyzer::visit(FunctionNode &p_function) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    int e = 0;
    if(symbol_manager.lookup(p_function.getNameCString(), false) != ENTRY_NOTFOUND){
        fprintf(stderr, "<Error> Found in line %d, column %d: ", p_function.getLocation().line, p_function.getLocation().col);
        fprintf(stderr, "symbol '%s' is redeclared\n", p_function.getNameCString());
        printErrorCode(p_function.getLocation().line, p_function.getLocation().col);
        e = 1;
        error = 1;
    }

    function_manager.addFunction(p_function.getNameCString(), p_function.getParaType(), p_function.getReturnType());
    function_manager.addFuncNum();

    if(e == 0)
    {    
        symbol_manager.addSymbol(p_function.getNameCString(), ENTRY_FUNCTION, p_function.getReturnType(), p_function.getParatypeCString(), 0);
    }

    //1
    SymbolTable* newtable = new SymbolTable(TABLE_FUNCTION);
    symbol_manager.pushScope(newtable);
    //2 skip
    //3
    p_function.visitChildNodes(*this);
    //4 skip
    //5
    symbol_manager.popScope(opt_symbol);
}

void SemanticAnalyzer::visit(CompoundStatementNode &p_compound_statement) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    int kind = symbol_manager.getTopKind();
    //1
    if(kind != 2){
        SymbolTable* newtable = new SymbolTable(TABLE_OTHER);
        symbol_manager.pushScope(newtable);
    }
    //2 skip
    symbol_manager.changeState(TABLE_OTHER);
    //3
    p_compound_statement.visitChildNodes(*this);
    //4 skip
    //5
    if(kind != 2)
        symbol_manager.popScope(opt_symbol);
}

void SemanticAnalyzer::visit(PrintNode &p_print) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    //1 skip
    //2 skip
    //3
    p_print.visitChildNodes(*this);
    //4
    int e = p_print.getExpressionError();
    if(e == 0){
        if(p_print.getExpressionType().getDimensions().size() != 0 || p_print.getExpressionType().getPrimitiveType() == PType::PrimitiveTypeEnum::kVoidType){
            fprintf(stderr, "<Error> Found in line %d, column %d: ", p_print.m_target->getLocation().line, p_print.m_target->getLocation().col);
            fprintf(stderr, "expression of print statement must be scalar type\n");
            printErrorCode(p_print.m_target->getLocation().line, p_print.m_target->getLocation().col);
            e = 1;
            error = 1;
        }
    }
    //5 skip
}

void SemanticAnalyzer::visit(BinaryOperatorNode &p_bin_op) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    //1 skip
    //2 skip
    //3
    p_bin_op.visitChildNodes(*this);
    
    string op(p_bin_op.getOpCString());
    PType left = p_bin_op.getLeftNodeType();
    PType right = p_bin_op.getRightNodeType();
    //4
    if(op == "+" || op =="-" || op == "*" || op == "/"){
        if((left.getPrimitiveType() != PType::PrimitiveTypeEnum::kIntegerType && left.getPrimitiveType() != PType::PrimitiveTypeEnum::kRealType) 
                || (right.getPrimitiveType() != PType::PrimitiveTypeEnum::kIntegerType && right.getPrimitiveType() != PType::PrimitiveTypeEnum::kRealType)
                    || left.getDimensions().size() > 0 || right.getDimensions().size() > 0){

            if(op != "+" || left.getPrimitiveType() != PType::PrimitiveTypeEnum::kStringType
                || right.getPrimitiveType() != PType::PrimitiveTypeEnum::kStringType
                    || left.getDimensions().size() > 0 || right.getDimensions().size() > 0){
                fprintf(stderr, "<Error> Found in line %d, column %d: ", p_bin_op.getLocation().line, p_bin_op.getLocation().col);
                fprintf(stderr, "invalid operands to binary operator '%s' ('%s' and '%s')\n", op.c_str(), left.getPTypeCString(), right.getPTypeCString());
                printErrorCode(p_bin_op.getLocation().line, p_bin_op.getLocation().col);
                error = 1;
                p_bin_op.setError();
            }
            PTypeSharedPtr tmp(new PType(PType::PrimitiveTypeEnum::kStringType));
            p_bin_op.setType(tmp);
        }
        else if(left.getPrimitiveType() == PType::PrimitiveTypeEnum::kIntegerType || right.getPrimitiveType() == PType::PrimitiveTypeEnum::kIntegerType){
            PTypeSharedPtr tmp(new PType(PType::PrimitiveTypeEnum::kIntegerType));
            p_bin_op.setType(tmp);
        }
        else if(left.getPrimitiveType() == PType::PrimitiveTypeEnum::kRealType || right.getPrimitiveType() == PType::PrimitiveTypeEnum::kRealType){
            PTypeSharedPtr tmp(new PType(PType::PrimitiveTypeEnum::kRealType));
            p_bin_op.setType(tmp);
        }        
    }
    else if(op == "mod"){
        if(left.getPrimitiveType() != PType::PrimitiveTypeEnum::kIntegerType
            || right.getPrimitiveType() != PType::PrimitiveTypeEnum::kIntegerType
                || left.getDimensions().size() > 0 || right.getDimensions().size() > 0){
            fprintf(stderr, "<Error> Found in line %d, column %d: ", p_bin_op.getLocation().line, p_bin_op.getLocation().col);
            fprintf(stderr, "invalid operands to binary operator '%s' ('%s' and '%s')\n", op.c_str(), left.getPTypeCString(), right.getPTypeCString());
            printErrorCode(p_bin_op.getLocation().line, p_bin_op.getLocation().col);
            error = 1;
            p_bin_op.setError();
        }
        PTypeSharedPtr tmp(new PType(PType::PrimitiveTypeEnum::kIntegerType));
        p_bin_op.setType(tmp);
    }
    else if(op == "<" || op == "<=" || op == "=" || op == ">=" || op == ">" || op == "<>"){
        if((left.getPrimitiveType() != PType::PrimitiveTypeEnum::kIntegerType && left.getPrimitiveType() != PType::PrimitiveTypeEnum::kRealType) 
                || (right.getPrimitiveType() != PType::PrimitiveTypeEnum::kIntegerType && right.getPrimitiveType() != PType::PrimitiveTypeEnum::kRealType)
                    || left.getDimensions().size() > 0 || right.getDimensions().size() > 0){
            fprintf(stderr, "<Error> Found in line %d, column %d: ", p_bin_op.getLocation().line, p_bin_op.getLocation().col);
            fprintf(stderr, "invalid operands to binary operator '%s' ('%s' and '%s')\n", op.c_str(), left.getPTypeCString(), right.getPTypeCString());
            printErrorCode(p_bin_op.getLocation().line, p_bin_op.getLocation().col);
            error = 1;
            p_bin_op.setError();
        }
        PTypeSharedPtr tmp(new PType(PType::PrimitiveTypeEnum::kBoolType));
        p_bin_op.setType(tmp);
    }
    else if(op == "and" || op == "or" || op == "not"){
        if(left.getPrimitiveType() != PType::PrimitiveTypeEnum::kBoolType
            || right.getPrimitiveType() != PType::PrimitiveTypeEnum::kBoolType
                || left.getDimensions().size() > 0 || right.getDimensions().size() > 0){
            fprintf(stderr, "<Error> Found in line %d, column %d: ", p_bin_op.getLocation().line, p_bin_op.getLocation().col);
            fprintf(stderr, "invalid operands to binary operator '%s' ('%s' and '%s')\n", op.c_str(), left.getPTypeCString(), right.getPTypeCString());
            printErrorCode(p_bin_op.getLocation().line, p_bin_op.getLocation().col);
            error = 1;
            p_bin_op.setError();
        }
        PTypeSharedPtr tmp(new PType(PType::PrimitiveTypeEnum::kBoolType));
        p_bin_op.setType(tmp);
    }
    //5 skip
}

void SemanticAnalyzer::visit(UnaryOperatorNode &p_un_op) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    //1 skip
    //2 skip
    //3
    p_un_op.visitChildNodes(*this);

    string op = p_un_op.getOpCString();
    PType oparand = p_un_op.getOparandType();
    //4
    if(op == "neg"){
        if(oparand.getPrimitiveType() == PType::PrimitiveTypeEnum::kIntegerType && oparand.getDimensions().size() == 0){
            p_un_op.setError();
            PTypeSharedPtr tmp(new PType(PType::PrimitiveTypeEnum::kIntegerType));
            p_un_op.setType(tmp);
        }
        else if(oparand.getPrimitiveType() == PType::PrimitiveTypeEnum::kRealType && oparand.getDimensions().size() == 0){
            p_un_op.setError();
            PTypeSharedPtr tmp(new PType(PType::PrimitiveTypeEnum::kRealType));
            p_un_op.setType(tmp);
        }
        else{
            fprintf(stderr, "<Error> Found in line %d, column %d: ", p_un_op.getLocation().line, p_un_op.getLocation().col);
            fprintf(stderr, "invalid operand to unary operator '%s' ('%s')\n", op.c_str(), oparand.getPTypeCString());
            printErrorCode(p_un_op.getLocation().line, p_un_op.getLocation().col);
            error = 1;
            p_un_op.setError();
            PTypeSharedPtr tmp(new PType(PType::PrimitiveTypeEnum::kRealType));
            p_un_op.setType(tmp);
        }
    }
    else if(op == "not"){
        if(oparand.getPrimitiveType() == PType::PrimitiveTypeEnum::kBoolType && oparand.getDimensions().size() == 0){
            PTypeSharedPtr tmp(new PType(PType::PrimitiveTypeEnum::kBoolType));
            p_un_op.setType(tmp);
        }
        else{
            fprintf(stderr, "<Error> Found in line %d, column %d: ", p_un_op.getLocation().line, p_un_op.getLocation().col);
            fprintf(stderr, "invalid operand to unary operator '%s' ('%s')\n", op.c_str(), oparand.getPTypeCString());
            printErrorCode(p_un_op.getLocation().line, p_un_op.getLocation().col);
            error = 1;
            p_un_op.setError();
            PTypeSharedPtr tmp(new PType(PType::PrimitiveTypeEnum::kBoolType));
            p_un_op.setType(tmp);
        }
    }
    //5 skip
}

void SemanticAnalyzer::visit(FunctionInvocationNode &p_func_invocation) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    //1 skip
    //2 skip
    //3
    p_func_invocation.visitChildNodes(*this);

    int e = 0;
    int symbolKind = symbol_manager.lookup(p_func_invocation.getNameCString(), true);
    //4
    if(symbolKind == ENTRY_NOTFOUND){
        fprintf(stderr, "<Error> Found in line %d, column %d: ", p_func_invocation.getLocation().line, p_func_invocation.getLocation().col);
        fprintf(stderr, "use of undeclared symbol '%s'\n", p_func_invocation.getNameCString());
        printErrorCode(p_func_invocation.getLocation().line, p_func_invocation.getLocation().col);
        e = 1;
        error = 1;
        p_func_invocation.setError();        
    }
    else if(symbolKind != ENTRY_FUNCTION){
        fprintf(stderr, "<Error> Found in line %d, column %d: ", p_func_invocation.getLocation().line, p_func_invocation.getLocation().col);
        fprintf(stderr, "call of non-function symbol '%s'\n", p_func_invocation.getNameCString());
        printErrorCode(p_func_invocation.getLocation().line, p_func_invocation.getLocation().col);
        e = 1;
        error = 1;
        p_func_invocation.setError();        
    }
    else {
        FunctionEntry functionEntry = function_manager.Lookup(p_func_invocation.getNameCString());
        vector<PType> parameterList = p_func_invocation.getParaType();
        if(functionEntry.para_type.size() != parameterList.size()){
            fprintf(stderr, "<Error> Found in line %d, column %d: ", p_func_invocation.getLocation().line, p_func_invocation.getLocation().col);
            fprintf(stderr, "too few/much arguments provided for function '%s'\n", p_func_invocation.getNameCString());
            printErrorCode(p_func_invocation.getLocation().line, p_func_invocation.getLocation().col);
            e = 1;
            error = 1;
            p_func_invocation.setError();            
        }
        else{
            for(uint32_t i = 0; i < parameterList.size(); i++){
                if(functionEntry.para_type[i] != parameterList[i]){
                    fprintf(stderr, "<Error> Found in line %d, column %d: ", p_func_invocation.m_args[i]->getLocation().line, p_func_invocation.m_args[i]->getLocation().col);
                    fprintf(stderr, "incompatible type passing '%s' to parameter of type '%s'\n", parameterList[i].getPTypeCString(), functionEntry.para_type[i].getPTypeCString());
                    printErrorCode(p_func_invocation.m_args[i]->getLocation().line, p_func_invocation.m_args[i]->getLocation().col);
                    p_func_invocation.setError();
                    e = 1;
                    error = 1;
                    break;
                }
            }
        }
    }
    
    if(e != 1 && symbol_manager.getSymbolEntry(p_func_invocation.getNameCString()).pass == 0){
        PType* tmp = new PType(symbol_manager.getSymbolEntry(p_func_invocation.getNameCString()).type);
        vector<uint64_t> dim;
        tmp->setDimensions(dim);
        PTypeSharedPtr sharedtmp = (PTypeSharedPtr) tmp;
        p_func_invocation.setType(sharedtmp);
    }
    //5 skip
}

void SemanticAnalyzer::visit(VariableReferenceNode &p_variable_ref) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    //1 skip
    //2 skip
    //3
    p_variable_ref.visitChildNodes(*this);

    int e = p_variable_ref.getErrors();
    int symbolKind = symbol_manager.lookup(p_variable_ref.getNameCString(), true);
    //4
    if(symbolKind == ENTRY_NOTFOUND){
        fprintf(stderr, "<Error> Found in line %d, column %d: ", p_variable_ref.getLocation().line, p_variable_ref.getLocation().col);
        fprintf(stderr, "use of undeclared symbol '%s'\n", p_variable_ref.getNameCString());
        printErrorCode(p_variable_ref.getLocation().line, p_variable_ref.getLocation().col);
        e = 1;
        error = 1;
        p_variable_ref.setError();        
    }
    else if(symbolKind == ENTRY_PROGRAM || symbolKind == ENTRY_FUNCTION){
        fprintf(stderr, "<Error> Found in line %d, column %d: ", p_variable_ref.getLocation().line, p_variable_ref.getLocation().col);
        fprintf(stderr, "use of non-variable symbol '%s'\n", p_variable_ref.getNameCString());
        printErrorCode(p_variable_ref.getLocation().line, p_variable_ref.getLocation().col);
        e = 1;
        error = 1;
        p_variable_ref.setError();        
    }
    else if(symbol_manager.getSymbolEntry(p_variable_ref.getNameCString()).pass == 1){
        p_variable_ref.setError();
    }


    if(e != 1 && symbol_manager.getSymbolEntry(p_variable_ref.getNameCString()).pass == 0){

        vector<uint64_t> dim;
        vector<uint64_t> tmpDim = symbol_manager.getSymbolEntry(p_variable_ref.getNameCString()).type.getDimensions();

        for(uint32_t i = p_variable_ref.getDimensions(); i < tmpDim.size(); i++){
            dim.push_back(tmpDim[i]);
        }

        PType* tmp = new PType(symbol_manager.getSymbolEntry(p_variable_ref.getNameCString()).type);
        tmp->setDimensions(dim);
        PTypeSharedPtr sharedtmp = (PTypeSharedPtr) tmp;
        p_variable_ref.setType(sharedtmp);

        vector<PType::PrimitiveTypeEnum> expressions = p_variable_ref.getExpressions();
        for(uint32_t i = 0; i < expressions.size(); i++){
            if(expressions[i] != PType::PrimitiveTypeEnum::kIntegerType){
                fprintf(stderr, "<Error> Found in line %d, column %d: ", p_variable_ref.getLocation().line, p_variable_ref.m_indices[i]->getLocation().col);
                fprintf(stderr, "index of array reference must be an integer\n");
                printErrorCode(p_variable_ref.getLocation().line, p_variable_ref.m_indices[i]->getLocation().col);
                e = 1;
                error = 1;
                p_variable_ref.setError();                
            }
        }

        vector<uint64_t> tmpDim2 = symbol_manager.getSymbolEntry(p_variable_ref.getNameCString()).type.getDimensions();
        if(p_variable_ref.getDimensions() > tmpDim2.size()){
            fprintf(stderr, "<Error> Found in line %d, column %d: ", p_variable_ref.getLocation().line, p_variable_ref.getLocation().col);
            fprintf(stderr, "there is an over array subscript on '%s'\n", p_variable_ref.getNameCString());
            printErrorCode(p_variable_ref.getLocation().line, p_variable_ref.getLocation().col);
            e = 1;
            error = 1;
            p_variable_ref.setError();            
        }
    }    
    //5 skip
}

void SemanticAnalyzer::visit(AssignmentNode &p_assignment) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    //1 skip
    //2 skip
    //3
    p_assignment.visitChildNodes(*this);
    
    int e1 = p_assignment.m_lvalue->getError();
    int e2 = p_assignment.m_expr->getError();
    int symbolKind = symbol_manager.lookup(p_assignment.m_lvalue->getNameCString(), true);
    //4
    if(e1 == 0){
        if(p_assignment.m_lvalue->getType().getDimensions().size() != 0){
            fprintf(stderr, "<Error> Found in line %d, column %d: ", p_assignment.m_lvalue->getLocation().line, p_assignment.m_lvalue->getLocation().col);
            fprintf(stderr, "array assignment is not allowed\n");
            printErrorCode(p_assignment.m_lvalue->getLocation().line, p_assignment.m_lvalue->getLocation().col);
            e1 = 1;
            error = 1;
        }
        else if(symbolKind == ENTRY_LOOPVAR && symbol_manager.getTopKind() != TABLE_FORLOOP){
            fprintf(stderr, "<Error> Found in line %d, column %d: ", p_assignment.m_lvalue->getLocation().line, p_assignment.m_lvalue->getLocation().col);
            fprintf(stderr, "the value of loop variable cannot be modified inside the loop body\n");
            printErrorCode(p_assignment.m_lvalue->getLocation().line, p_assignment.m_lvalue->getLocation().col);
            e1 = 1;
            error = 1;
        }
        else if(symbolKind == ENTRY_CONSTANT){
            fprintf(stderr, "<Error> Found in line %d, column %d: ", p_assignment.m_lvalue->getLocation().line, p_assignment.m_lvalue->getLocation().col);
            fprintf(stderr, "cannot assign to variable '%s' which is a constant\n", p_assignment.m_lvalue->getNameCString());
            printErrorCode(p_assignment.m_lvalue->getLocation().line, p_assignment.m_lvalue->getLocation().col);
            e1 = 1;
            error = 1;
        }        
    }
    if(e1 == 0 && e2 == 0){
        PType lvalueType = p_assignment.m_lvalue->getType();
        PType exprType = p_assignment.m_expr->getType();
        if(p_assignment.m_expr->getType().getDimensions().size() != 0){
            fprintf(stderr, "<Error> Found in line %d, column %d: ", p_assignment.m_expr->getLocation().line, p_assignment.m_expr->getLocation().col);
            fprintf(stderr, "array assignment is not allowed\n");
            printErrorCode(p_assignment.m_expr->getLocation().line, p_assignment.m_expr->getLocation().col);
            e1 = 1;
            error = 1;
        }
        else if((lvalueType.getPrimitiveType() == PType::PrimitiveTypeEnum::kStringType && exprType.getPrimitiveType() != PType::PrimitiveTypeEnum::kStringType)
            || (lvalueType.getPrimitiveType() == PType::PrimitiveTypeEnum::kBoolType && exprType.getPrimitiveType() != PType::PrimitiveTypeEnum::kBoolType)
                || (lvalueType.getPrimitiveType() == PType::PrimitiveTypeEnum::kIntegerType && exprType.getPrimitiveType() != PType::PrimitiveTypeEnum::kIntegerType)
                    ||(lvalueType.getPrimitiveType() == PType::PrimitiveTypeEnum::kRealType && (exprType.getPrimitiveType() != PType::PrimitiveTypeEnum::kRealType) && (exprType.getPrimitiveType() != PType::PrimitiveTypeEnum::kIntegerType)) ){
            fprintf(stderr, "<Error> Found in line %d, column %d: ", p_assignment.getLocation().line, p_assignment.getLocation().col);
            fprintf(stderr, "assigning to '%s' from incompatible type '%s'\n", lvalueType.getPTypeCString(), exprType.getPTypeCString());
            printErrorCode(p_assignment.getLocation().line, p_assignment.getLocation().col);
            e1 = 1;
            error = 1;
        }
    }
    //5 skip
}

void SemanticAnalyzer::visit(ReadNode &p_read) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    //1 skip
    //2 skip
    //3
    p_read.visitChildNodes(*this);

    int e = p_read.getExpressionError();
    int symbolKind = symbol_manager.lookup(p_read.m_target->getNameCString(), true);
    //4
    if(e == 0){
        if(p_read.getExpressionType().getDimensions().size() != 0){
            fprintf(stderr, "<Error> Found in line %d, column %d: ", p_read.m_target->getLocation().line, p_read.m_target->getLocation().col);
            fprintf(stderr, "variable reference of read statement must be scalar type\n");
            printErrorCode(p_read.m_target->getLocation().line, p_read.m_target->getLocation().col);
            e = 1;
            error = 1;
        }
        else if(symbolKind == ENTRY_LOOPVAR || symbolKind == ENTRY_CONSTANT){
            fprintf(stderr, "<Error> Found in line %d, column %d: ", p_read.m_target->getLocation().line, p_read.m_target->getLocation().col);
            fprintf(stderr, "variable reference of read statement cannot be a constant or loop variable\n");
            printErrorCode(p_read.m_target->getLocation().line, p_read.m_target->getLocation().col);
            e = 1;
            error = 1;
        }
    }
    //5 skip
}

void SemanticAnalyzer::visit(IfNode &p_if) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    //1 skip
    //2 skip
    //3
    p_if.visitChildNodes(*this);

    int e = p_if.m_condition->getError();
    //4
    if(e == 0){
        if(p_if.m_condition->getType().getPrimitiveType() != PType::PrimitiveTypeEnum::kBoolType || p_if.m_condition->getType().getDimensions().size() != 0){
            fprintf(stderr, "<Error> Found in line %d, column %d: ", p_if.m_condition->getLocation().line, p_if.m_condition->getLocation().col);
            fprintf(stderr, "the expression of condition must be boolean type\n");
            printErrorCode(p_if.m_condition->getLocation().line, p_if.m_condition->getLocation().col);
            e = 1;
            error = 1;
        }
    }
    //5 skip
}

void SemanticAnalyzer::visit(WhileNode &p_while) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    //1 skip
    //2 skip
    //3
    p_while.visitChildNodes(*this);

    int e = p_while.m_condition->getError();
    //4
    if(e == 0){
        if(p_while.m_condition->getType().getPrimitiveType() != PType::PrimitiveTypeEnum::kBoolType || p_while.m_condition->getType().getDimensions().size() != 0){
            fprintf(stderr, "<Error> Found in line %d, column %d: ", p_while.m_condition->getLocation().line, p_while.m_condition->getLocation().col);
            fprintf(stderr, "the expression of condition must be boolean type\n");
            printErrorCode(p_while.m_condition->getLocation().line, p_while.m_condition->getLocation().col);
            e = 1;
            error = 1;
        }
    }
    //5 skip
}

void SemanticAnalyzer::visit(ForNode &p_for) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    //1
    SymbolTable* newtable = new SymbolTable(TABLE_FORLOOP);
    symbol_manager.pushScope(newtable);
    //2 skip
    //3
    p_for.visitChildNodes(*this);

    //4
    int leftNum = p_for.m_left;
    int rightNum = p_for.m_end_condition->m_constant_ptr->m_value.integer;
    if(leftNum > rightNum){
        fprintf(stderr, "<Error> Found in line %d, column %d: ", p_for.getLocation().line, p_for.getLocation().col);
        fprintf(stderr, "the lower bound and upper bound of iteration count must be in the incremental order\n");
        printErrorCode(p_for.getLocation().line, p_for.getLocation().col);
        error = 1;
    }
    //5
    symbol_manager.popScope(opt_symbol);
}

void SemanticAnalyzer::visit(ReturnNode &p_return) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    //1 skip
    //2 skip
    //3
    p_return.visitChildNodes(*this);

    int e = 0;
    int fnum = function_manager.getFuncNum();
    if(inFunction == 0){
        fnum = 0;
    }
    //4
    FunctionEntry functionEntry = function_manager.lookup(fnum);
    if(functionEntry.ret_type.getPrimitiveType() == PType::PrimitiveTypeEnum::kVoidType && p_return.m_ret_val->getTypeSharedPtr() != nullptr){
        fprintf(stderr, "<Error> Found in line %d, column %d: ", p_return.getLocation().line, p_return.getLocation().col);
        fprintf(stderr, "program/procedure should not return a value\n");
        printErrorCode(p_return.getLocation().line, p_return.getLocation().col);
        e = 1;
        error = 1;
    }
    if(e == 0){
        if((functionEntry.ret_type.getPrimitiveType() == PType::PrimitiveTypeEnum::kIntegerType
                && (p_return.m_ret_val->getType().getPrimitiveType() != PType::PrimitiveTypeEnum::kIntegerType
                    && p_return.m_ret_val->getType().getPrimitiveType() != PType::PrimitiveTypeEnum::kRealType) )
                        || functionEntry.ret_type != p_return.m_ret_val->getType() ){
            fprintf(stderr, "<Error> Found in line %d, column %d: ", p_return.m_ret_val->getLocation().line, p_return.m_ret_val->getLocation().col);
            fprintf(stderr, "return '%s' from a function with return type '%s'\n", p_return.m_ret_val->getType().getPTypeCString(), functionEntry.ret_type.getPTypeCString());
            printErrorCode(p_return.m_ret_val->getLocation().line, p_return.m_ret_val->getLocation().col);
            e = 1;
            error = 1;
        }
    }
    //5 skip
}

