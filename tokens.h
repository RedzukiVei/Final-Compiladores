#ifndef TOKENS_H
#define TOKENS_H

#include <cstdio>

enum Tokens {
    // Fin de archivo
    T_EOF = 0,
    
    // Salto de línea (MUY IMPORTANTE en Mini-0)
    T_NL = 258,
    
    // Palabras reservadas
    T_IF,
    T_ELSE,
    T_END,
    T_WHILE,
    T_LOOP,
    T_FUN,
    T_RETURN,
    T_NEW,
    T_STRING,
    T_INT,
    T_CHAR,
    T_BOOL,
    T_TRUE,
    T_FALSE,
    T_AND,
    T_OR,
    T_NOT,
    
    // Literales e identificadores
    T_ID,
    T_LITNUMERAL,
    T_LITSTRING,
    
    // Operadores de comparación
    T_GE,       // >=
    T_LE,       // <=
    T_NE,       // <>
    
    // Error
    T_ERROR
};

extern int yylineno;
extern char* yytext;
extern FILE* yyin;
extern int tokenLinea;

typedef union {
    int num;
    char* str;
} YYSTYPE;

extern YYSTYPE yylval;

#endif