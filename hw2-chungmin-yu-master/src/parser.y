%{
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

extern int32_t line_num;  /* declared in scanner.l */
extern char buffer[];     /* declared in scanner.l */
extern FILE *yyin;        /* declared by lex */
extern char *yytext;      /* declared by lex */

extern int yylex(void); 
static void yyerror(const char *msg);
%}

    /* operator */
%token MOD ASSIGN LE NE GE AND OR NOT

    /* keyword */
%token ARRAY BEG TYPE DEF DO ELSE END FALSE FOR IF OF PRINT READ THEN TO TRUE RETURN VAR WHILE

    /* other token */
%token IDENTIFIER INTEGER FLOATINGPOINT SCIENTIFIC STRING OCTAL

%left NOT
%left AND
%left OR
%left NE GE LE GT LT EQ
%left '*' '/' MOD
%left '+' '-'
%left NEGATIVE

%%
/* program */
Program			: IDENTIFIER ';' Variables Functions Compound END
			;

/* function */
Functions		: Functions Function
			| Function
			;

Function 		: Function_declaration
			| Function_definition
			|
			;

Function_declaration 	: IDENTIFIER '(' Argument_list ')' ':' TYPE ';' 
			| IDENTIFIER '(' Argument_list ')' ';'
			;

Function_definition 	: IDENTIFIER '(' Argument_list ')' ':' TYPE Compound END 
			| IDENTIFIER '(' Argument_list ')' Compound END
			;

/* identifier list */
Identifier_list     	: Identifiers
                   	| 
                    	;

Identifiers   		: Identifiers ',' IDENTIFIER
                    	| IDENTIFIER
                    	;

/* argument list */
Argument_list		: Arguments
			|
			;

Arguments 		: Identifier_list ':' TYPE ';' Arguments
			| Identifier_list ':' TYPE 
                	| Identifier_list ':' ARRAY INTEGER OF ARRAY_TYPES ';' Arguments
                	| Identifier_list ':' ARRAY INTEGER OF ARRAY_TYPES                                 
               		;

/* variable */
Variables      		: Variable
                	|
                	;

Variable		: Variable Variable_declaration
                        | Variable_declaration
                        ;

Variable_declaration	: VAR Identifier_list ':' TYPE ';'
			| VAR Identifier_list ':' ARRAY INTEGER OF ARRAY_TYPES ';'
			| VAR Identifier_list ':' Literal_constant ';'
			;

ARRAY_TYPES 		: ARRAY INTEGER OF ARRAY_TYPES
			| TYPE
			;

Literal_constant 	: INTEGER | OCTAL | FLOATINGPOINT | SCIENTIFIC  
			| STRING
			| TRUE
			| FALSE
			;

/* statements */
Statements      	: nonEmptyStatements
                	|
               		;

nonEmptyStatements      : nonEmptyStatements Statement
                        | Statement
                        ;

Statement       	: Compound
                	| Simple
                	| Conditional
               		| While
               		| For
               		| Return
               		| Function_call ';'
               		| Expression
               		;

/* compound */
Compound 		: BEG Variables Statements END 
			;

/* simple */
Simple 			: Variable_reference ASSIGN Expression ';'
            		| PRINT Expression ';'
            		| PRINT Variable_reference ';'
        		| READ Variable_reference ';'
            		;

Variable_reference	: IDENTIFIER
			| ARRAY_reference
			;

ARRAY_reference     	: IDENTIFIER Bracket_expression
                    	;

Bracket_expression	: '[' Expression ']' Bracket_expression
                        | '[' Expression ']'
                        ;

/* conditional */
Conditional     	: IF Expression THEN Compound ELSE Compound END IF
			| IF Expression THEN Compound END IF
                	;

/* while */
While 			: WHILE Expression DO Compound END DO
			;

/* for */
For 			: FOR IDENTIFIER ASSIGN INTEGER TO INTEGER DO Compound END DO
			;

/* return */
Return 			: RETURN Expression ';'
			;

/* function call */
Function_call 		: IDENTIFIER '(' Expression_list ')'
			| IDENTIFIER '(' ')'
			;

/* expression */
Expression_list 	: Expression ',' Expression_list
			| Expression
			;
					
Expression      	: '-' Expression %prec NEGATIVE
                	| Expression GT Expression
               		| Expression LT Expression
               		| Expression EQ Expression
               		| Expression LE Expression
               		| Expression GE Expression
               		| Expression NE Expression
               		| Expression AND Expression
               		| Expression OR Expression
               		| NOT Expression %prec NOT
               		| Expression '+' Expression
               		| Expression '-' Expression
               		| Expression '*' Expression
               		| Expression '/' Expression
               		| Expression MOD Expression
                	| '(' Expression ')' %prec '*'
               		| Function_call
               		| Variable_reference
               		| Literal_constant
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
    if (argc != 2) {
        fprintf(stderr, "Usage: ./parser <filename>\n");
        exit(-1);
    }

    yyin = fopen(argv[1], "r");
    if (yyin == NULL) {
        perror("fopen() failed:");
    }

    yyparse();

    printf("\n"
           "|--------------------------------|\n"
           "|  There is no syntactic error!  |\n"
           "|--------------------------------|\n");
    return 0;
}

