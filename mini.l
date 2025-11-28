%{
#include <cstdio>
#include <cstring>
#include "tokens.h"
%}

%option noyywrap
%option nounput        
%option yylineno       

%%

[0-9]+\.?[0-9]* {
        yylval.str = strdup(yytext);
        return T_NUMBER;
}

[0-9]*\.[0-9]+ {
        yylval.str = strdup(yytext);
        return T_NUMBER;
}

[a-zA-Z_][a-zA-Z0-9_]* {
        // Palabras reservadas
        if (strcmp(yytext, "if") == 0) return T_IF;
        if (strcmp(yytext, "else") == 0) return T_ELSE;
        if (strcmp(yytext, "print") == 0) return T_PRINT;
        if (strcmp(yytext, "int") == 0) return T_INT;
        if (strcmp(yytext, "float") == 0) return T_FLOAT;
        if (strcmp(yytext, "string") == 0) return T_STRING;

        yylval.str = strdup(yytext);
        return T_ID;
}

\"[^\"]*\" {
        yylval.str = strdup(yytext);
        return T_STRLIT;
}

"==" { return T_EQ; }
"!=" { return T_NE; }
"<=" { return T_LE; }
">=" { return T_GE; }
"&&" { return T_AND; }
"||" { return T_OR; }

\n   { /*  */ }
[ \t\r]+ ; /* ignorar espacios */

.    { return yytext[0]; }

%%