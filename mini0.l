%{
#include <stdio.h>
#include <string.h>
#include "tokens.h"

int linea = 1;
char lastTokenType[30];  // Para comunicar el tipo al parser
char lastTokenValue[100]; // Para guardar el valor del token

void guardarToken(const char* tipo, const char* valor);
%}

%%

"if"        { guardarToken("PALABRA_CLAVE", yytext); return TOKEN_IF; }
"else"      { guardarToken("PALABRA_CLAVE", yytext); return TOKEN_ELSE; }
"while"     { guardarToken("PALABRA_CLAVE", yytext); return TOKEN_WHILE; }
"for"       { guardarToken("PALABRA_CLAVE", yytext); return TOKEN_FOR; }
"int"       { guardarToken("PALABRA_CLAVE", yytext); return TOKEN_INT; }
"float"     { guardarToken("PALABRA_CLAVE", yytext); return TOKEN_FLOAT; }
"char"      { guardarToken("PALABRA_CLAVE", yytext); return TOKEN_CHAR; }
"return"    { guardarToken("PALABRA_CLAVE", yytext); return TOKEN_RETURN; }

[0-9]+                          { guardarToken("NUMERO_DECIMAL", yytext); return TOKEN_NUM_DEC; }
0[xX][0-9A-Fa-f]+               { guardarToken("NUMERO_HEX", yytext); return TOKEN_NUM_HEX; }
[0-9]+"."[0-9]+                 { guardarToken("NUMERO_REAL", yytext); return TOKEN_NUM_REAL; }

\"([^"\\]|\\.)*\"               { guardarToken("CADENA", yytext); return TOKEN_STRING; }
[a-zA-Z_][a-zA-Z0-9_]*          { guardarToken("IDENTIFICADOR", yytext); return TOKEN_ID; }

"=="        { guardarToken("IGUALDAD", yytext); return TOKEN_EQ; }
"!="        { guardarToken("DISTINTO", yytext); return TOKEN_NEQ; }
"<="        { guardarToken("MENOR_IGUAL", yytext); return TOKEN_LE; }
">="        { guardarToken("MAYOR_IGUAL", yytext); return TOKEN_GE; }
"="         { guardarToken("ASIGNACION", yytext); return TOKEN_ASSIGN; }
"<"         { guardarToken("MENOR", yytext); return TOKEN_LT; }
">"         { guardarToken("MAYOR", yytext); return TOKEN_GT; }

"+"         { guardarToken("MAS", yytext); return TOKEN_PLUS; }
"-"         { guardarToken("MENOS", yytext); return TOKEN_MINUS; }
"*"         { guardarToken("MULT", yytext); return TOKEN_MULT; }
"/"         { guardarToken("DIV", yytext); return TOKEN_DIV; }

";"         { guardarToken("PUNTO_Y_COMA", yytext); return TOKEN_SEMICOLON; }
","         { guardarToken("COMA", yytext); return TOKEN_COMMA; }
"("         { guardarToken("PAREN_IZQ", yytext); return TOKEN_LPAREN; }
")"         { guardarToken("PAREN_DER", yytext); return TOKEN_RPAREN; }
"{"         { guardarToken("LLAVE_IZQ", yytext); return TOKEN_LBRACE; }
"}"         { guardarToken("LLAVE_DER", yytext); return TOKEN_RBRACE; }

[ \t\r]+    ;    /* ignora espacios */
\n          { linea++; }

"//".*      ;    /* ignora comentarios de línea */

.           { guardarToken("ERROR", yytext); return TOKEN_ERROR; }

%%

void guardarToken(const char* tipo, const char* valor) {
    strcpy(lastTokenType, tipo);
    strcpy(lastTokenValue, valor);
    printf("[LINEA %d] %-15s %s\n", linea, tipo, valor);
}

int yywrap() { 
    return 1; 
}