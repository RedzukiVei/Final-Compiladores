#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include "tokens.h"


extern ParserToken currentToken;
extern int parseErrors;
extern FILE* sourceFile;


extern char* yytext;
extern int linea;
extern FILE* yyin;


void initParser(FILE* input);
int parse();