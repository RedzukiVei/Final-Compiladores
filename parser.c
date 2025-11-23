#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

// Variables globales
ParserToken currentToken;
int parseErrors = 0;
FILE* sourceFile;

// Mapeo de strings de tokens a tipos
TokenType mapTokenType(const char* tipo) {
    if (strcmp(tipo, "PALABRA_CLAVE") == 0) {
        return TOKEN_UNKNOWN;
    }
    if (strcmp(tipo, "IDENTIFICADOR") == 0) return TOKEN_ID;
    if (strcmp(tipo, "NUMERO_DECIMAL") == 0) return TOKEN_NUM_DEC;
    if (strcmp(tipo, "NUMERO_HEX") == 0) return TOKEN_NUM_HEX;
    if (strcmp(tipo, "NUMERO_REAL") == 0) return TOKEN_NUM_REAL;
    if (strcmp(tipo, "CADENA") == 0) return TOKEN_STRING;
    if (strcmp(tipo, "IGUALDAD") == 0) return TOKEN_EQ;
    if (strcmp(tipo, "DISTINTO") == 0) return TOKEN_NEQ;
    if (strcmp(tipo, "MENOR_IGUAL") == 0) return TOKEN_LE;
    if (strcmp(tipo, "MAYOR_IGUAL") == 0) return TOKEN_GE;
    if (strcmp(tipo, "ASIGNACION") == 0) return TOKEN_ASSIGN;
    if (strcmp(tipo, "MENOR") == 0) return TOKEN_LT;
    if (strcmp(tipo, "MAYOR") == 0) return TOKEN_GT;
    if (strcmp(tipo, "MAS") == 0) return TOKEN_PLUS;
    if (strcmp(tipo, "MENOS") == 0) return TOKEN_MINUS;
    if (strcmp(tipo, "MULT") == 0) return TOKEN_MULT;
    if (strcmp(tipo, "DIV") == 0) return TOKEN_DIV;
    if (strcmp(tipo, "PUNTO_Y_COMA") == 0) return TOKEN_SEMICOLON;
    if (strcmp(tipo, "COMA") == 0) return TOKEN_COMMA;
    if (strcmp(tipo, "PAREN_IZQ") == 0) return TOKEN_LPAREN;
    if (strcmp(tipo, "PAREN_DER") == 0) return TOKEN_RPAREN;
    if (strcmp(tipo, "LLAVE_IZQ") == 0) return TOKEN_LBRACE;
    if (strcmp(tipo, "LLAVE_DER") == 0) return TOKEN_RBRACE;
    if (strcmp(tipo, "ERROR") == 0) return TOKEN_ERROR;
    return TOKEN_UNKNOWN;
}

TokenType mapKeyword(const char* lexeme) {
    if (strcmp(lexeme, "if") == 0) return TOKEN_IF;
    if (strcmp(lexeme, "else") == 0) return TOKEN_ELSE;
    if (strcmp(lexeme, "while") == 0) return TOKEN_WHILE;
    if (strcmp(lexeme, "for") == 0) return TOKEN_FOR;
    if (strcmp(lexeme, "int") == 0) return TOKEN_INT;
    if (strcmp(lexeme, "float") == 0) return TOKEN_FLOAT;
    if (strcmp(lexeme, "char") == 0) return TOKEN_CHAR;
    if (strcmp(lexeme, "return") == 0) return TOKEN_RETURN;
    return TOKEN_UNKNOWN;
}

void initParser(FILE* input) {
    sourceFile = input;
    parseErrors = 0;
    nextToken(); // Carga el primer token
}

void nextToken() {
    extern int yylex();
    extern char* yytext;
    extern int linea;
    extern char lastTokenValue[100];
    
    int tokenCode = yylex();
    
    if (tokenCode == 0) {
        currentToken.type = TOKEN_EOF;
        strcpy(currentToken.lexeme, "EOF");
        currentToken.line = linea;
        return;
    }
    
    
    currentToken.type = (TokenType)tokenCode;
    strcpy(currentToken.lexeme, lastTokenValue);
    currentToken.line = linea;
}

void syntaxError(const char* message) {
    fprintf(stderr, "Error sintáctico en línea %d: %s (token: '%s')\n", 
            currentToken.line, message, currentToken.lexeme);
    parseErrors++;
}

void match(TokenType expected) {
    if (currentToken.type == expected) {
        nextToken();
    } else {
        char errorMsg[200];
        sprintf(errorMsg, "Se esperaba token de tipo %d, se encontró '%s'", 
                expected, currentToken.lexeme);
        syntaxError(errorMsg);
        // Modo pánico: saltar hasta encontrar un punto de sincronización
        while (currentToken.type != TOKEN_SEMICOLON && 
               currentToken.type != TOKEN_RBRACE &&
               currentToken.type != TOKEN_EOF) {
            nextToken();
        }
        if (currentToken.type == TOKEN_SEMICOLON) {
            nextToken();
        }
    }
}



void program() {
    printf("Iniciando análisis sintáctico...\n");
    declaration_list();
    match(TOKEN_EOF);
}

void declaration_list() {
    
    if (currentToken.type == TOKEN_INT || 
        currentToken.type == TOKEN_FLOAT || 
        currentToken.type == TOKEN_CHAR) {
        declaration();
        declaration_list_prime();
    } else {
        syntaxError("Se esperaba una declaración (int, float, char)");
    }
}

void declaration_list_prime() {

    if (currentToken.type == TOKEN_INT || 
        currentToken.type == TOKEN_FLOAT || 
        currentToken.type == TOKEN_CHAR) {
        declaration();
        declaration_list_prime();
    }
    
}

void declaration() {
    
    type();
    match(TOKEN_ID);
    declaration_tail();
}

void declaration_tail() {
    
    if (currentToken.type == TOKEN_SEMICOLON) {
        match(TOKEN_SEMICOLON);
    } else if (currentToken.type == TOKEN_ASSIGN) {
        match(TOKEN_ASSIGN);
        expression();
        match(TOKEN_SEMICOLON);
    } else if (currentToken.type == TOKEN_LPAREN) {
        match(TOKEN_LPAREN);
        params_opt();
        match(TOKEN_RPAREN);
        compound_stmt();
    } else {
        syntaxError("Se esperaba ';', '=' o '(' después del identificador");
    }
}

void params_opt() {
    
    if (currentToken.type == TOKEN_INT || 
        currentToken.type == TOKEN_FLOAT || 
        currentToken.type == TOKEN_CHAR) {
        params();
    }
    
}

void type() {
   
    if (currentToken.type == TOKEN_INT) {
        match(TOKEN_INT);
    } else if (currentToken.type == TOKEN_FLOAT) {
        match(TOKEN_FLOAT);
    } else if (currentToken.type == TOKEN_CHAR) {
        match(TOKEN_CHAR);
    } else {
        syntaxError("Se esperaba un tipo (int, float, char)");
    }
}

void params() {
    
    param();
    param_list_prime();
}

void param_list_prime() {
   
    if (currentToken.type == TOKEN_COMMA) {
        match(TOKEN_COMMA);
        param();
        param_list_prime();
    }
    
}

void param() {
    
    type();
    match(TOKEN_ID);
}

void compound_stmt() {
    match(TOKEN_LBRACE);
    local_declarations();
    statement_list();
    match(TOKEN_RBRACE);
}

void local_declarations() {
    
    while (currentToken.type == TOKEN_INT || 
           currentToken.type == TOKEN_FLOAT || 
           currentToken.type == TOKEN_CHAR) {
        var_declaration();
    }
}

void var_declaration() {
    
    type();
    match(TOKEN_ID);
    var_decl_tail();
}

void var_decl_tail() {
    
    if (currentToken.type == TOKEN_SEMICOLON) {
        match(TOKEN_SEMICOLON);
    } else if (currentToken.type == TOKEN_ASSIGN) {
        match(TOKEN_ASSIGN);
        expression();
        match(TOKEN_SEMICOLON);
    } else {
        syntaxError("Se esperaba ';' o '=' en declaración de variable");
    }
}

void statement_list() {
    
    while (currentToken.type != TOKEN_RBRACE && currentToken.type != TOKEN_EOF) {
        statement();
    }
}

void statement() {
    
    if (currentToken.type == TOKEN_LBRACE) {
        compound_stmt();
    } else if (currentToken.type == TOKEN_IF) {
        selection_stmt();
    } else if (currentToken.type == TOKEN_WHILE || currentToken.type == TOKEN_FOR) {
        iteration_stmt();
    } else if (currentToken.type == TOKEN_RETURN) {
        return_stmt();
    } else {
        expression_stmt();
    }
}

void expression_stmt() {
    
    if (currentToken.type == TOKEN_SEMICOLON) {
        match(TOKEN_SEMICOLON);
    } else {
        expression();
        match(TOKEN_SEMICOLON);
    }
}

void selection_stmt() {
    
    match(TOKEN_IF);
    match(TOKEN_LPAREN);
    expression();
    match(TOKEN_RPAREN);
    statement();
    selection_stmt_tail();
}

void selection_stmt_tail() {
    
    if (currentToken.type == TOKEN_ELSE) {
        match(TOKEN_ELSE);
        statement();
    }
}

void iteration_stmt() {
    
    if (currentToken.type == TOKEN_WHILE) {
        match(TOKEN_WHILE);
        match(TOKEN_LPAREN);
        expression();
        match(TOKEN_RPAREN);
        statement();
    } else if (currentToken.type == TOKEN_FOR) {
        match(TOKEN_FOR);
        match(TOKEN_LPAREN);
        expression();
        match(TOKEN_SEMICOLON);
        expression();
        match(TOKEN_SEMICOLON);
        expression();
        match(TOKEN_RPAREN);
        statement();
    } else {
        syntaxError("Se esperaba 'while' o 'for'");
    }
}

void return_stmt() {
    
    match(TOKEN_RETURN);
    return_tail();
}

void return_tail() {
   
    if (currentToken.type == TOKEN_SEMICOLON) {
        match(TOKEN_SEMICOLON);
    } else {
        expression();
        match(TOKEN_SEMICOLON);
    }
}

void expression() {
    
    simple_expression();
    expression_prime();
}

void expression_prime() {
    
    if (currentToken.type == TOKEN_ASSIGN) {
        match(TOKEN_ASSIGN);
        expression();
    }

}

void simple_expression() {

    additive_expression();
    simple_expression_prime();
}

void simple_expression_prime() {

    if (currentToken.type == TOKEN_LE || currentToken.type == TOKEN_LT ||
        currentToken.type == TOKEN_GT || currentToken.type == TOKEN_GE ||
        currentToken.type == TOKEN_EQ || currentToken.type == TOKEN_NEQ) {
        relop();
        additive_expression();
    }

}

void relop() {
   
    if (currentToken.type == TOKEN_LE) match(TOKEN_LE);
    else if (currentToken.type == TOKEN_LT) match(TOKEN_LT);
    else if (currentToken.type == TOKEN_GT) match(TOKEN_GT);
    else if (currentToken.type == TOKEN_GE) match(TOKEN_GE);
    else if (currentToken.type == TOKEN_EQ) match(TOKEN_EQ);
    else if (currentToken.type == TOKEN_NEQ) match(TOKEN_NEQ);
    else syntaxError("Se esperaba un operador relacional");
}

void additive_expression() {
    
    term();
    additive_expression_prime();
}

void additive_expression_prime() {

    while (currentToken.type == TOKEN_PLUS || currentToken.type == TOKEN_MINUS) {
        addop();
        term();
    }
}

void addop() {

    if (currentToken.type == TOKEN_PLUS) {
        match(TOKEN_PLUS);
    } else if (currentToken.type == TOKEN_MINUS) {
        match(TOKEN_MINUS);
    } else {
        syntaxError("Se esperaba '+' o '-'");
    }
}

void term() {

    factor();
    term_prime();
}

void term_prime() {
    
    while (currentToken.type == TOKEN_MULT || currentToken.type == TOKEN_DIV) {
        mulop();
        factor();
    }
}

void mulop() {
    
    if (currentToken.type == TOKEN_MULT) {
        match(TOKEN_MULT);
    } else if (currentToken.type == TOKEN_DIV) {
        match(TOKEN_DIV);
    } else {
        syntaxError("Se esperaba '*' o '/'");
    }
}

void factor() {

    if (currentToken.type == TOKEN_LPAREN) {
        match(TOKEN_LPAREN);
        expression();
        match(TOKEN_RPAREN);
    } else if (currentToken.type == TOKEN_ID) {
        match(TOKEN_ID);
        factor_tail();
    } else if (currentToken.type == TOKEN_NUM_DEC || 
               currentToken.type == TOKEN_NUM_HEX || 
               currentToken.type == TOKEN_NUM_REAL) {
        nextToken(); // Consume cualquier número
    } else if (currentToken.type == TOKEN_STRING) {
        match(TOKEN_STRING);
    } else {
        syntaxError("Se esperaba '(', identificador, número o cadena");
    }
}

void factor_tail() {

    if (currentToken.type == TOKEN_LPAREN) {
        match(TOKEN_LPAREN);
        args_opt();
        match(TOKEN_RPAREN);
    }

}

void args_opt() {

    if (currentToken.type != TOKEN_RPAREN) {
        args();
    }
}

void args() {

    expression();
    args_prime();
}

void args_prime() {

    while (currentToken.type == TOKEN_COMMA) {
        match(TOKEN_COMMA);
        expression();
    }
}

int parse() {
    program();
    
    if (parseErrors == 0) {
        printf("\n=== ANÁLISIS SINTÁCTICO EXITOSO ===\n");
        return 0;
    } else {
        fprintf(stderr, "\n=== ANÁLISIS FALLÓ CON %d ERROR(ES) ===\n", parseErrors);
        return 1;
    }
}