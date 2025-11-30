%{
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "tokens.h"

int tokenLinea = 1;

#define RETURN_TOKEN(tok) { tokenLinea = yylineno; return (tok); }
%}

%option noyywrap
%option nounput
%option noinput
%option yylineno

%x COMMENT_BLOCK

%%

    /* ============================================ */
    /* COMENTARIOS                                  */
    /* ============================================ */

"//".*              { /* Comentario de línea - ignorar */ }

"/*"                { BEGIN(COMMENT_BLOCK); }
<COMMENT_BLOCK>"*/" { BEGIN(INITIAL); }
<COMMENT_BLOCK>\n   { /* Contar líneas dentro del comentario */ }
<COMMENT_BLOCK>.    { /* Ignorar contenido */ }
<COMMENT_BLOCK><<EOF>> { RETURN_TOKEN(T_ERROR); }

    /* ============================================ */
    /* SALTOS DE LÍNEA (Importantes en Mini-0)     */
    /* ============================================ */

\n[ \t]*            { RETURN_TOKEN(T_NL); }

    /* ============================================ */
    /* ESPACIOS Y TABS (ignorar)                   */
    /* ============================================ */

[ \t\r]+            { /* Ignorar espacios horizontales */ }

    /* ============================================ */
    /* PALABRAS RESERVADAS                         */
    /* ============================================ */

"if"                { RETURN_TOKEN(T_IF); }
"else"              { RETURN_TOKEN(T_ELSE); }
"end"               { RETURN_TOKEN(T_END); }
"while"             { RETURN_TOKEN(T_WHILE); }
"loop"              { RETURN_TOKEN(T_LOOP); }
"fun"               { RETURN_TOKEN(T_FUN); }
"return"            { RETURN_TOKEN(T_RETURN); }
"new"               { RETURN_TOKEN(T_NEW); }
"string"            { RETURN_TOKEN(T_STRING); }
"int"               { RETURN_TOKEN(T_INT); }
"char"              { RETURN_TOKEN(T_CHAR); }
"bool"              { RETURN_TOKEN(T_BOOL); }
"true"              { RETURN_TOKEN(T_TRUE); }
"false"             { RETURN_TOKEN(T_FALSE); }
"and"               { RETURN_TOKEN(T_AND); }
"or"                { RETURN_TOKEN(T_OR); }
"not"               { RETURN_TOKEN(T_NOT); }

    /* ============================================ */
    /* LITERALES NUMÉRICOS                         */
    /* ============================================ */

0[xX][0-9a-fA-F]+   { 
    yylval.num = (int)strtol(yytext, NULL, 16);
    RETURN_TOKEN(T_LITNUMERAL); 
}

[0-9]+              { 
    yylval.num = atoi(yytext);
    RETURN_TOKEN(T_LITNUMERAL); 
}

    /* ============================================ */
    /* LITERALES STRING                            */
    /* ============================================ */

\"([^\"\\]|\\.)*\"  { 
    yylval.str = strdup(yytext);
    RETURN_TOKEN(T_LITSTRING); 
}

\"([^\"\\]|\\.)*    {
    yylval.str = strdup(yytext);
    RETURN_TOKEN(T_ERROR);
}

    /* ============================================ */
    /* IDENTIFICADORES                             */
    /* ============================================ */

[a-zA-Z_][a-zA-Z0-9_]*  {
    yylval.str = strdup(yytext);
    RETURN_TOKEN(T_ID);
}

    /* ============================================ */
    /* OPERADORES MULTI-CARÁCTER                   */
    /* ============================================ */

">="                { RETURN_TOKEN(T_GE); }
"<="                { RETURN_TOKEN(T_LE); }
"<>"                { RETURN_TOKEN(T_NE); }

    /* ============================================ */
    /* OPERADORES Y PUNTUACIÓN (un carácter)       */
    /* ============================================ */

"("                 { RETURN_TOKEN('('); }
")"                 { RETURN_TOKEN(')'); }
","                 { RETURN_TOKEN(','); }
":"                 { RETURN_TOKEN(':'); }
">"                 { RETURN_TOKEN('>'); }
"<"                 { RETURN_TOKEN('<'); }
"="                 { RETURN_TOKEN('='); }
"["                 { RETURN_TOKEN('['); }
"]"                 { RETURN_TOKEN(']'); }
"+"                 { RETURN_TOKEN('+'); }
"-"                 { RETURN_TOKEN('-'); }
"*"                 { RETURN_TOKEN('*'); }
"/"                 { RETURN_TOKEN('/'); }

    /* ============================================ */
    /* CARÁCTER NO RECONOCIDO                      */
    /* ============================================ */

.                   { RETURN_TOKEN(T_ERROR); }

<<EOF>>             { return T_EOF; }

%%