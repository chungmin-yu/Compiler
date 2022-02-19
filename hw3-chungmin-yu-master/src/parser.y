%{
#include "AST/ast.hpp"
#include "AST/program.hpp"
#include "AST/decl.hpp"
#include "AST/variable.hpp"
#include "AST/ConstantValue.hpp"
#include "AST/function.hpp"
#include "AST/CompoundStatement.hpp"
#include "AST/print.hpp"
#include "AST/expression.hpp"
#include "AST/BinaryOperator.hpp"
#include "AST/UnaryOperator.hpp"
#include "AST/FunctionInvocation.hpp"
#include "AST/VariableReference.hpp"
#include "AST/assignment.hpp"
#include "AST/read.hpp"
#include "AST/if.hpp"
#include "AST/while.hpp"
#include "AST/for.hpp"
#include "AST/return.hpp"
#include "AST/AstDumper.hpp"

#include <cassert>
#include <errno.h>
#include <cstdlib>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>
using namespace std;

#define YYLTYPE yyltype

typedef struct YYLTYPE {
    uint32_t first_line;
    uint32_t first_column;
    uint32_t last_line;
    uint32_t last_column;
} yyltype;

extern int32_t line_num;  /* declared in scanner.l */
extern char buffer[];     /* declared in scanner.l */
extern FILE *yyin;        /* declared by lex */
extern char *yytext;      /* declared by lex */

static ProgramNode *root;
static AstDumper visitor;

extern "C" int yylex(void);
static void yyerror(const char *msg);
extern int yylex_destroy(void);
%}

%code requires {
    #include "AST/ast.hpp"
    #include "AST/program.hpp"
    #include "AST/decl.hpp"
    #include "AST/variable.hpp"
    #include "AST/ConstantValue.hpp"
    #include "AST/function.hpp"
    #include "AST/CompoundStatement.hpp"
    #include "AST/print.hpp"
    #include "AST/expression.hpp"
    #include "AST/BinaryOperator.hpp"
    #include "AST/UnaryOperator.hpp"
    #include "AST/FunctionInvocation.hpp"
    #include "AST/VariableReference.hpp"
    #include "AST/assignment.hpp"
    #include "AST/read.hpp"
    #include "AST/if.hpp"
    #include "AST/while.hpp"
    #include "AST/for.hpp"
    #include "AST/return.hpp"
    #include <vector>
    using namespace std;
}

    /* For yylval */
%union {
    /* basic semantic value */
    char                                      *identifier;
    int                                       int_val;
    double                                    float_val;
    bool                                      bool_val;

    AstNode                                   *node; 
    AssignmentNode                            *assign;
    BinaryOperatorNode                        *binary;
    CompoundStatementNode                     *compound;
    ConstantValueNode                         *constant;
    DeclNode                                  *decl;
    ExpressionNode                            *express;
    ForNode                                   *ffor;
    FunctionNode                              *func;
    FunctionInvocationNode                    *func_inv;
    IfNode                                    *iff;
    PrintNode                                 *pri;
    ProgramNode                               *prog;
    ReadNode                                  *read;
    ReturnNode                                *ret;
    UnaryOperatorNode                         *unary;
    VariableNode                              *vari;
    VariableReferenceNode                     *vari_ref;
    WhileNode                                 *whi; 

    vector<AstNode*>                          *node_list;
    vector<DeclNode*>                         *decl_list;
    vector<string>                            *str_list;
  
    struct constValue                         *c_val;
};


    /* Follow the order in scanner.l */

    /* Delimiter */
%token COMMA SEMICOLON COLON
%token L_PARENTHESIS R_PARENTHESIS
%token L_BRACKET R_BRACKET

    /* Operator */
%token ASSIGN
%left OR
%left AND
%right NOT
%left LESS LESS_OR_EQUAL EQUAL GREATER GREATER_OR_EQUAL NOT_EQUAL
%left PLUS MINUS
%left MULTIPLY DIVIDE MOD
%right UNARY_MINUS

    /* Keyword */
%token ARRAY BOOLEAN INTEGER REAL STRING
%token END BEGIN_ /* Use BEGIN_ since BEGIN is a keyword in lex */
%token DO ELSE FOR IF THEN WHILE
%token DEF OF TO RETURN VAR
%token FALSE TRUE
%token PRINT READ

    /* Identifier */
%token ID

    /* Literal */
%token INT_LITERAL
%token REAL_LITERAL
%token STRING_LITERAL

    /* Nonterminal */
%type <identifier> ProgramName ID FunctionName STRING_LITERAL TRUE FALSE ReturnType ScalarType
%type <int_val> INT_LITERAL
%type <float_val> REAL_LITERAL
%type <bool_val> NegOrNot

%type <node_list>  DeclarationList Declarations FunctionList Functions
%type <node_list>  ExpressionList Expressions StatementList Statements 
%type <node_list>  ArrRefs ArrRefList 

%type <decl> FormalArg
%type <decl_list> FormalArgs FormalArgList
%type <str_list> Type ArrType ArrDecl IdList
%type <c_val>  LiteralConstant  StringAndBoolean IntegerAndReal

%type <node>  Function FunctionDeclaration FunctionDefinition
%type <node>  Declaration 
%type <node> Statement CompoundStatement Simple Condition ElseOrNot While For Return FunctionCall FunctionInvocation
%type <node> Expression VariableReference

%%

Program:
    ProgramName SEMICOLON
    /* ProgramBody */
    DeclarationList FunctionList CompoundStatement
    /* End of ProgramBody */
    END {
        root = new ProgramNode(@1.first_line, @1.first_column, $1, "void", $3, $4, $5);

        free($1);
    }
;

ProgramName:
    ID { $$ = $1; }
;

DeclarationList:
    Epsilon { $$ = NULL; }
    |
    Declarations { $$ = $1; }
;

Declarations:
    Declaration { $$ = new vector<AstNode *>; $$->push_back($1); }
    |
    Declarations Declaration { $$ = $1; $$->push_back($2);
    }
;

FunctionList:
    Epsilon { $$ = NULL; }
    |
    Functions { $$ = $1; }
;

Functions:
    Function { $$ = new vector<AstNode *>; $$->push_back($1); }
    |
    Functions Function { $$ = $1; $$->push_back($2);
    }
;

Function:
    FunctionDeclaration { $$ = $1; }
    |
    FunctionDefinition { $$ = $1; }
;

FunctionDeclaration:
    FunctionName L_PARENTHESIS FormalArgList R_PARENTHESIS ReturnType SEMICOLON
    { $$ = new FunctionNode(@1.first_line, @1.first_column, $1, $3, $5, NULL); }
;

FunctionDefinition:
    FunctionName L_PARENTHESIS FormalArgList R_PARENTHESIS ReturnType
    CompoundStatement
    END { $$ = new FunctionNode(@1.first_line, @1.first_column, $1, $3, $5, $6); }
;

FunctionName:
    ID { $$ = $1; }
;

FormalArgList:
    Epsilon { $$ = NULL; }
    |
    FormalArgs { $$ = $1; }
;

FormalArgs:
    FormalArg { $$ = new vector<DeclNode *>; $$->push_back($1); }
    |
    FormalArgs SEMICOLON FormalArg { $$ = $1; $$->push_back($3); }
;

FormalArg:
    IdList COLON Type { $$ = new DeclNode(@1.first_line, @1.first_column, $1, $3); }
;


IdList:
    ID { $$ = new vector<string> (); $$->push_back(string($1)); $$->push_back(to_string(@1.first_column)); }
    |
    IdList COMMA ID { $$ = $1; $$->push_back(string($3)); $$->push_back(to_string(@3.first_column)); }
;

ReturnType:
    COLON ScalarType { $$ = $2; }
    |
    Epsilon { $$ = "void"; }
;

    /*
       Data Types and Declarations
                                   */

Declaration:
    VAR IdList COLON Type SEMICOLON { $$ = new DeclNode(@1.first_line, @1.first_column, $2, $4); }
    |
    VAR IdList COLON LiteralConstant SEMICOLON { $$ = new DeclNode(@1.first_line, @1.first_column, $2, $4); }
;

Type:
    ScalarType { $$ = new vector<string> (); $$->push_back(string($1)); }
    |
    ArrType { $$ = $1; }
;

ScalarType:
    INTEGER { $$ = "integer"; }
    |
    REAL { $$ = "real"; }
    |
    STRING { $$ = "string"; }
    |
    BOOLEAN { $$ = "boolean"; }
;

ArrType:
    ArrDecl ScalarType { $1->insert($1->begin(), string($2)); $$ = $1; }
;

ArrDecl:
    ARRAY INT_LITERAL OF { $$ = new vector<string> (); $$->push_back(to_string($2)); }
    |
    ArrDecl ARRAY INT_LITERAL OF { $1->push_back(to_string($3)); $$ = $1; }
;

LiteralConstant:
    NegOrNot INT_LITERAL
    {
        $$ = new struct constValue;
        $$->type = "integer";        
        if($1){
            $$->integer_value = -$2;
            $$->col = @1.first_column;
        }else{
            $$->integer_value = $2;
            $$->col = @2.first_column;
        }
    }
    |
    NegOrNot REAL_LITERAL
    {
        $$ = new struct constValue;
        $$->type = "real";        
        if($1){
            $$->real_value = -$2;
            $$->col = @1.first_column;
        }else{
            $$->real_value = $2;
            $$->col = @2.first_column;
        }
    }
    |
    StringAndBoolean { $$ = $1; }
;

NegOrNot:
    Epsilon { $$ = false; }
    |
    MINUS %prec UNARY_MINUS { $$ = true; }
;

StringAndBoolean:
    STRING_LITERAL { $$ = new struct constValue; $$->type = "string"; $$->string_value = $1; $$->col = @1.first_column; }
    |
    TRUE { $$ = new struct constValue; $$->type = "boolean"; $$->boolean_value = true; $$->col = @1.first_column; }
    |
    FALSE { $$ = new struct constValue; $$->type = "boolean"; $$->boolean_value = false; $$->col = @1.first_column; }
;

IntegerAndReal:
    INT_LITERAL { $$ = new struct constValue; $$->type = "integer"; $$->integer_value = $1; }
    |
    REAL_LITERAL { $$ = new struct constValue; $$->type = "real"; $$->real_value = $1; }
;

    /*
       Statements
                  */

Statement:
    CompoundStatement { $$ = $1; }
    |
    Simple { $$ = $1; }
    |
    Condition { $$ = $1; }
    |
    While { $$ = $1; }
    |
    For { $$ = $1; }
    |
    Return { $$ = $1; }
    |
    FunctionCall { $$ = $1; }
;

CompoundStatement:
    BEGIN_
    DeclarationList
    StatementList
    END { $$ = new CompoundStatementNode(@1.first_line, @1.first_column, $2, $3); }
;

Simple:
    VariableReference ASSIGN Expression SEMICOLON { $$ = new AssignmentNode(@2.first_line, @2.first_column, $1, $3); }
    |
    PRINT Expression SEMICOLON { $$ = new PrintNode(@1.first_line, @1.first_column, $2); }
    |
    READ VariableReference SEMICOLON { $$ = new ReadNode(@1.first_line, @1.first_column, $2); }
;

VariableReference:
    ID ArrRefList { $$ = new VariableReferenceNode(@1.first_line, @1.first_column, $1, $2); }
;

ArrRefList:
    Epsilon { $$ = NULL; }
    |
    ArrRefs { $$ = $1; }
;

ArrRefs:
    L_BRACKET Expression R_BRACKET { $$ = new vector<AstNode *>; $$->push_back($2); }
    |
    ArrRefs L_BRACKET Expression R_BRACKET { $$ = $1; $$->push_back($3); }
;

Condition:
    IF Expression THEN
    CompoundStatement
    ElseOrNot
    END IF { $$ = new IfNode(@1.first_line, @1.first_column, $2, $4, $5); }
;

ElseOrNot:
    ELSE
    CompoundStatement { $$ = $2; }
    |
    Epsilon { $$ = NULL; }
;

While:
    WHILE Expression DO
    CompoundStatement
    END DO { $$ = new WhileNode(@1.first_line, @1.first_column, $2, $4); }
;

For:
    FOR ID ASSIGN INT_LITERAL TO INT_LITERAL DO
    CompoundStatement
    END DO
    {
        vector<string> *id = new vector<string> ();
        id->push_back(string($2));
        id->push_back(to_string(@2.first_column));
        vector<string> *type = new vector<string> ();
        type->push_back("integer");
        AstNode *decl = new DeclNode(@2.first_line, @2.first_column, id, type);

        struct constValue *c1 = new struct constValue;
        c1->type = "integer";
        c1->integer_value = $4;        
        AstNode *ex1 =  new ConstantValueNode(@4.first_line, @4.first_column, c1);
        AstNode *vr1 = new VariableReferenceNode(@2.first_line, @2.first_column, $2, NULL);        
        AstNode *assign = new  AssignmentNode(@3.first_line, @3.first_column, vr1, ex1);

        struct constValue *c2 = new struct constValue;
        c2->type = "integer";
        c2->integer_value = $6;        
        AstNode *ex2 = new  ConstantValueNode(@6.first_line, @6.first_column, c2);
        
        $$ = new ForNode(@1.first_line, @1.first_column, decl, assign, ex2, $8);
    } 
;

Return:
    RETURN Expression SEMICOLON { $$ = new ReturnNode(@1.first_line, @1.first_column, $2); }
;

FunctionCall:
    FunctionInvocation SEMICOLON { $$ = $1; }
;

FunctionInvocation:
    ID L_PARENTHESIS ExpressionList R_PARENTHESIS { $$ = new FunctionInvocationNode(@1.first_line, @1.first_column, $1, $3); }
;

ExpressionList:
    Epsilon { $$ = NULL; }
    |
    Expressions { $$ = $1; }
;

Expressions:
    Expression { $$ = new vector<AstNode *>; $$->push_back($1); }
    |
    Expressions COMMA Expression { $$ = $1; $$->push_back($3); }
;

StatementList:
    Epsilon { $$ = NULL; }
    |
    Statements { $$ = $1; }
;

Statements:
    Statement { $$ = new vector<AstNode *>; $$->push_back($1); }
    |
    Statements Statement { $$ = $1; $$->push_back($2); }
;

Expression:
    L_PARENTHESIS Expression R_PARENTHESIS { $$ = $2; }
    |
    MINUS Expression %prec UNARY_MINUS { $$ = new UnaryOperatorNode(@1.first_line, @1.first_column, "neg", $2); }
    |
    Expression MULTIPLY Expression { $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, "*", $1, $3); }
    |
    Expression DIVIDE Expression { $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, "/", $1, $3); }
    |
    Expression MOD Expression { $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, "mod", $1, $3); }
    |
    Expression PLUS Expression { $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, "+", $1, $3); }
    |
    Expression MINUS Expression { $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, "-", $1, $3); }
    |
    Expression LESS Expression { $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, "<", $1, $3); }
    |
    Expression LESS_OR_EQUAL Expression { $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, "<=", $1, $3); }
    |
    Expression GREATER Expression { $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, ">", $1, $3); }
    |
    Expression GREATER_OR_EQUAL Expression { $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, ">=", $1, $3); }
    |
    Expression EQUAL Expression { $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, "=", $1, $3); }
    |
    Expression NOT_EQUAL Expression { $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, "<>", $1, $3); }
    |
    NOT Expression { $$ = new UnaryOperatorNode(@1.first_line, @1.first_column, "not", $2); }
    |
    Expression AND Expression { $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, "and", $1, $3); }
    |
    Expression OR Expression { $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, "or", $1, $3); }
    |
    IntegerAndReal {$$ = new ConstantValueNode(@1.first_line, @1.first_column, $1);}
    |
    StringAndBoolean {$$ = new ConstantValueNode(@1.first_line, @1.first_column, $1);  free($1);}
    |
    VariableReference { $$ = $1; }
    |
    FunctionInvocation { $$ = $1; }
;

    /*
       misc
            */
Epsilon:
;

%%

void yyerror(const char *msg) {
    fprintf(stderr,
            "\n"
            "|-----------------------------------------------------------------"
            "---------\n"
            "| Error found in Line #%d: %s\n"
            "|\n"
            "| Unmatched token: %s\n"
            "|-----------------------------------------------------------------"
            "---------\n",
            line_num, buffer, yytext);
    exit(-1);
}

int main(int argc, const char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: ./parser <filename> [--dump-ast]\n");
        exit(-1);
    }

    yyin = fopen(argv[1], "r");
    if (yyin == NULL) {
        perror("fopen() failed:");
    }

    yyparse();

    if (argc >= 3 && strcmp(argv[2], "--dump-ast") == 0) {
        //root->print();
        visitor.visit(*root);
    }

    printf("\n"
           "|--------------------------------|\n"
           "|  There is no syntactic error!  |\n"
           "|--------------------------------|\n");

    delete root;
    fclose(yyin);
    yylex_destroy();
    return 0;
}
