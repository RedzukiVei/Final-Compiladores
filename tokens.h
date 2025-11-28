#ifndef TOKENS_H
#define TOKENS_H

#include <cstdio>   // ← NECESARIO para FILE*

enum Tokens {
    T_INT = 256,
    T_FLOAT,
    T_STRING,
    T_IF,
    T_ELSE,
    T_PRINT,

    T_ID,
    T_NUMBER,
    T_STRLIT,

    T_EQ,
    T_NE,
    T_LE,
    T_GE,
    T_AND,
    T_OR,

    T_ERROR
};

extern int yylineno;
extern char* yytext;
extern FILE* yyin;

typedef union {
    int num;       // si luego quieres guardar números
    char* str;     // si guardas strings
} YYSTYPE;

extern YYSTYPE yylval;

#endif
