%{
#include <stdint.h>

#define LIST                concatenateString(yytext)
#define TOKEN(t)            { LIST; if (opt_tok) printf("<%s>\n", #t); }
#define TOKEN_CHAR(t)       { LIST; if (opt_tok) printf("<%c>\n", (t)); }
#define TOKEN_STRING(t, s)  { LIST; if (opt_tok) printf("<%s: %s>\n", #t, (s)); }
#define TOKEN_KEYWORD(t, s) { LIST; if (opt_tok) printf("<%s%s>\n", #t, (s)); }
#define MAX_LINE_LENG       512

uint32_t line_num = 1;
static char buffer[MAX_LINE_LENG];

static uint32_t opt_src = 1;
static uint32_t opt_tok = 1;
static char *buffer_ptr = buffer;

static void concatenateString(const char *yytext_ptr);

%}

space             [ \t]+
digit             [0-9]
letter            [a-zA-Z]
keyword           ("var"|"def"|"array"|"of"|"boolean"|"integer"|"real"|"string"|"true"|"false"|"while"|"do"|"if"|"then"|"else"|"for"|"to"|"begin"|"end"|"print"|"read"|"return")
identifier        {letter}({letter}|{digit})*
integer           0|[1-9][0-9]*
oct_integer       0[0-7]+
float             {integer}\.(0|[0-9]*[1-9])
scientific        ({integer}|{float})[Ee][\+\-]?{integer}
string            \"([^"\n]|\"\")*\"

%Start onelineCMNT
%Start comment


%%

<INITIAL>"/*" { LIST; BEGIN comment; }
<comment>. { LIST; }
<comment>\n {
    if (opt_src) {
        printf("%d: %s\n", line_num, buffer);
    }
    ++line_num;
    buffer[0] = '\0';
    buffer_ptr = buffer;
}
<comment>"*/" { LIST; BEGIN INITIAL; }
<INITIAL>"//&S+" { LIST; opt_src=1; BEGIN onelineCMNT; }
<INITIAL>"//&S-" { LIST; opt_src=0; BEGIN onelineCMNT; }
<INITIAL>"//&T+" { LIST; opt_tok=1; BEGIN onelineCMNT; }
<INITIAL>"//&T-" { LIST; opt_tok=0; BEGIN onelineCMNT; }
<INITIAL>"//" { LIST; BEGIN onelineCMNT; }
<onelineCMNT>. { LIST; }
<onelineCMNT>\n {
    if (opt_src) {
        printf("%d: %s\n", line_num, buffer);
    }
    ++line_num;
    buffer[0] = '\0';
    buffer_ptr = buffer;
    BEGIN INITIAL;
}

<INITIAL>"," { TOKEN_CHAR(','); }
<INITIAL>";" { TOKEN_CHAR(';'); }
<INITIAL>":" { TOKEN_CHAR(':'); }
<INITIAL>"(" { TOKEN_CHAR('('); }
<INITIAL>")" { TOKEN_CHAR(')'); }
<INITIAL>"[" { TOKEN_CHAR('['); }
<INITIAL>"]" { TOKEN_CHAR(']'); }
<INITIAL>"+" { TOKEN_CHAR('+'); }
<INITIAL>"-" { TOKEN_CHAR('-'); }
<INITIAL>"*" { TOKEN_CHAR('*'); }
<INITIAL>"/" { TOKEN_CHAR('/'); }
<INITIAL>"mod" { TOKEN(mod); }
<INITIAL>":=" { TOKEN(:=); }
<INITIAL>"<" { TOKEN_CHAR('<'); }
<INITIAL>"<=" { TOKEN(<=); }
<INITIAL>"<>" { TOKEN(<>); }
<INITIAL>">=" { TOKEN(>=); }
<INITIAL>">" { TOKEN_CHAR('>'); }
<INITIAL>"=" { TOKEN_CHAR('='); }
<INITIAL>"and" { TOKEN(and); }
<INITIAL>"or" { TOKEN(or); }
<INITIAL>"not" { TOKEN(not); }
<INITIAL>{keyword} { TOKEN_KEYWORD(KW, yytext); }
<INITIAL>{identifier} {TOKEN_STRING(id, yytext); }
<INITIAL>{integer} { TOKEN_STRING(integer, yytext); }
<INITIAL>{oct_integer} { TOKEN_STRING(oct_integer, yytext); }
<INITIAL>{float} { TOKEN_STRING(float, yytext); }
<INITIAL>{scientific} { TOKEN_STRING(scientific, yytext); }
<INITIAL>{string} {
  char str[yyleng];
  for (int i=1, j=1; i<yyleng; i++, j++) {
    if (yytext[i]=='\"' && yytext[i-1]=='\"'){ i++; }
    str[j] = yytext[i];
    if (i == yyleng-1){ str[j] = '\0';}
  }
  TOKEN_STRING(string, &str[1]); 
}

<INITIAL>\n {
    if (opt_src) {
        printf("%d: %s\n", line_num, buffer);
    }
    ++line_num;
    buffer[0] = '\0';
    buffer_ptr = buffer;
}

{space} {
    LIST;
}

    /* Catch the character which is not accepted by rules above */
. {
    printf("Error at line %d: bad character \"%s\"\n", line_num, yytext);
    exit(-1);
}

%%

static void concatenateString(const char *yytext_ptr) {
    while (*yytext_ptr) {
        *buffer_ptr = *yytext_ptr;
        ++buffer_ptr;
        ++yytext_ptr;
    }
    *buffer_ptr = '\0';
}

int main(int argc, const char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: ./scanner [filename]\n");
        exit(-1);
    }

    yyin = fopen(argv[1], "r");
    if (yyin == NULL) {
        fprintf(stderr, "Open file error\n");
        exit(-1);
    }

    yylex();

    return 0;
}
