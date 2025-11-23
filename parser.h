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


void nextToken();
void match(TokenType expected);
void syntaxError(const char* message);
TokenType mapTokenType(const char* tipo);
TokenType mapKeyword(const char* lexeme);

void program();
void declaration_list();
void declaration_list_prime();
void declaration();
void declaration_tail();
void params_opt();
void type();
void params();
void param_list_prime();
void param();
void compound_stmt();
void local_declarations();
void var_declaration();
void var_decl_tail();
void statement_list();
void statement();
void expression_stmt();
void selection_stmt();
void selection_stmt_tail();
void iteration_stmt();
void return_stmt();
void return_tail();
void expression();
void expression_prime();
void simple_expression();
void simple_expression_prime();
void relop();
void additive_expression();
void additive_expression_prime();
void addop();
void term();
void term_prime();
void mulop();
void factor();
void factor_tail();
void args_opt();
void args();
void args_prime();

#endif 