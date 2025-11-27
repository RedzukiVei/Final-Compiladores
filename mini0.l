%{
#include <stdio.h>
#include <string.h>
#include "tokens.h"

int linea = 1;
extern int agregarToken(const char* tipo, const char* valor, int linea);
%}

%%

"if"        { agregarToken("PALABRA_CLAVE", yytext, linea); }
"else"      { agregarToken("PALABRA_CLAVE", yytext, linea); }
"while"     { agregarToken("PALABRA_CLAVE", yytext, linea); }
"for"       { agregarToken("PALABRA_CLAVE", yytext, linea); }
"int"       { agregarToken("PALABRA_CLAVE", yytext, linea); }
"float"     { agregarToken("PALABRA_CLAVE", yytext, linea); }
"char"      { agregarToken("PALABRA_CLAVE", yytext, linea); }
"string"    { agregarToken("PALABRA_CLAVE", yytext, linea); }
"return"    { agregarToken("PALABRA_CLAVE", yytext, linea); }
"print"     { agregarToken("PALABRA_CLAVE", yytext, linea); }

"&&"        { agregarToken("AND", yytext, linea); }
"||"        { agregarToken("OR", yytext, linea); }

0[xX][0-9A-Fa-f]+               { agregarToken("NUMERO_HEX", yytext, linea); }
[0-9]+"."[0-9]+                 { agregarToken("NUMERO_REAL", yytext, linea); }
[0-9]+                          { agregarToken("NUMERO_DECIMAL", yytext, linea); }

\"([^"\\]|\\.)*\"               { agregarToken("CADENA", yytext, linea); }
[a-zA-Z_][a-zA-Z0-9_]*          { agregarToken("IDENTIFICADOR", yytext, linea); }

"=="        { agregarToken("IGUALDAD", yytext, linea); }
"!="        { agregarToken("DISTINTO", yytext, linea); }
"<="        { agregarToken("MENOR_IGUAL", yytext, linea); }
">="        { agregarToken("MAYOR_IGUAL", yytext, linea); }
"="         { agregarToken("ASIGNACION", yytext, linea); }
"<"         { agregarToken("MENOR", yytext, linea); }
">"         { agregarToken("MAYOR", yytext, linea); }

"+"         { agregarToken("MAS", yytext, linea); }
"-"         { agregarToken("MENOS", yytext, linea); }
"*"         { agregarToken("MULT", yytext, linea); }
"/"         { agregarToken("DIV", yytext, linea); }

";"         { agregarToken("PUNTO_Y_COMA", yytext, linea); }
","         { agregarToken("COMA", yytext, linea); }
"("         { agregarToken("PAREN_IZQ", yytext, linea); }
")"         { agregarToken("PAREN_DER", yytext, linea); }
"{"         { agregarToken("LLAVE_IZQ", yytext, linea); }
"}"         { agregarToken("LLAVE_DER", yytext, linea); }

[ \t\r]+    ;    
\n          { linea++; }

.           { agregarToken("ERROR", yytext, linea); }

%%

int yywrap() { return 1; }