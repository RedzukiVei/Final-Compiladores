#include <iostream>
#include <vector>
#include <string>
#include "tokens.h"
#include "parser.h"

using namespace std;

int lookahead;
extern int yylex();
extern int yylineno;
extern char* yytext;

// ------------------------------------------------------------
// Vector para almacenar todos los errores encontrados
// ------------------------------------------------------------
struct ErrorInfo {
    int linea;
    string mensaje;
};

vector<ErrorInfo> errores;
bool hayErrores = false;
int profundidadRecursion = 0;
const int MAX_RECURSION = 1000;

// ------------------------------------------------------------
// Declaraciones de funciones
// ------------------------------------------------------------

void match(int expected);
void errorSintactico(const char* esperado);
void parseStmt();
void parseExpr();
void parseExprPrime();
void parseTerm();
void parseTermPrime();
void parseFactor();
void parseRelational();
void parseRelationalPrime();
void parseElseOpt();
void sincronizar(const vector<int>& siguientes);
bool esInicioDeStmt();
bool esSiguienteDeStmt();

// ------------------------------------------------------------
// Tokens de sincronización
// ------------------------------------------------------------

const vector<int> STMT_FOLLOW = {';', '}', T_IF, T_INT, T_FLOAT, T_STRING, T_PRINT, 0};
const vector<int> EXPR_FOLLOW = {';', ')', '}', 0};

// ------------------------------------------------------------
// Manejo de errores mejorado
// ------------------------------------------------------------
void errorSintactico(const char* esperado) {
    ErrorInfo error;
    error.linea = yylineno;
    error.mensaje = string("Se esperaba '") + esperado + "' pero se encontró '" + yytext + "'";
    errores.push_back(error);
    hayErrores = true;
}

void sincronizar(const vector<int>& siguientes) {
    int intentos = 0;
    const int MAX_INTENTOS = 100;
    
    // Avanzar hasta encontrar un token de sincronización
    while (lookahead != 0 && intentos < MAX_INTENTOS) {
        for (int token : siguientes) {
            if (lookahead == token) {
                return;
            }
        }
        lookahead = yylex();
        intentos++;
    }
    
    // Si llegamos aquí y no encontramos sincronización, forzar fin
    if (intentos >= MAX_INTENTOS) {
        lookahead = 0;
    }
}

bool esInicioDeStmt() {
    return (lookahead == T_INT || lookahead == T_FLOAT || 
            lookahead == T_STRING || lookahead == T_IF || 
            lookahead == T_PRINT);
}

bool esSiguienteDeStmt() {
    return (lookahead == '}' || lookahead == 0 || esInicioDeStmt());
}

void match(int expected) {
    if (lookahead == expected) {
        lookahead = yylex();
    } else {
        // Crear mensaje descriptivo del token esperado
        string tokenName;
        switch(expected) {
            case T_INT: tokenName = "int"; break;
            case T_FLOAT: tokenName = "float"; break;
            case T_STRING: tokenName = "string"; break;
            case T_IF: tokenName = "if"; break;
            case T_ELSE: tokenName = "else"; break;
            case T_PRINT: tokenName = "print"; break;
            case T_ID: tokenName = "identificador"; break;
            case T_NUMBER: tokenName = "número"; break;
            case T_STRLIT: tokenName = "cadena"; break;
            case ';': tokenName = ";"; break;
            case '(': tokenName = "("; break;
            case ')': tokenName = ")"; break;
            case '{': tokenName = "{"; break;
            case '}': tokenName = "}"; break;
            case '=': tokenName = "="; break;
            default: tokenName = string(1, (char)expected); break;
        }
        errorSintactico(tokenName.c_str());
        
        // NO avanzar automáticamente aquí para evitar bucles
        // La sincronización se hará en niveles superiores
    }
}

// ------------------------------------------------------------
//  PROGRAM ::= STMT PROGRAM'
//  PROGRAM' ::= STMT PROGRAM' | ε
// ------------------------------------------------------------
int program() {
    profundidadRecursion++;
    
    if (profundidadRecursion > MAX_RECURSION) {
        ErrorInfo error;
        error.linea = yylineno;
        error.mensaje = "Recursión excesiva detectada - posible error en el archivo";
        errores.push_back(error);
        hayErrores = true;
        profundidadRecursion--;
        return 0;
    }
    
    while (lookahead != 0 && lookahead != '}') {
        if (esInicioDeStmt()) {
            parseStmt();
        } else if (lookahead == ';') {
            // Punto y coma suelto, consumirlo y reportar
            ErrorInfo error;
            error.linea = yylineno;
            error.mensaje = "Punto y coma inesperado";
            errores.push_back(error);
            hayErrores = true;
            lookahead = yylex();
        } else {
            ErrorInfo error;
            error.linea = yylineno;
            error.mensaje = string("Instrucción inválida: '") + yytext + "'";
            errores.push_back(error);
            hayErrores = true;
            // Avanzar al menos un token para no quedarse atascado
            lookahead = yylex();
        }
    }
    
    profundidadRecursion--;
    return !hayErrores;
}

// ------------------------------------------------------------
//  STMT
// ------------------------------------------------------------
void parseStmt() {
    // DECLARACIÓN: TYPE ID = EXPR ;
    if (lookahead == T_INT || lookahead == T_FLOAT || lookahead == T_STRING) {
        int tipo = lookahead;
        match(lookahead);  // tipo
        
        if (lookahead == T_ID) {
            match(T_ID);
        } else {
            errorSintactico("identificador");
            sincronizar(STMT_FOLLOW);
            return;
        }
        
        if (lookahead == '=') {
            match('=');
        } else {
            errorSintactico("=");
            sincronizar(STMT_FOLLOW);
            return;
        }
        
        parseExpr();
        
        if (lookahead == ';') {
            match(';');
        } else {
            errorSintactico(";");
            // Buscar el próximo punto y coma o inicio de statement
            sincronizar(STMT_FOLLOW);
            if (lookahead == ';') {
                match(';');
            }
        }
    }
    // IF (EXPR) { PROGRAM } ELSE_OPT
    else if (lookahead == T_IF) {
        match(T_IF);
        
        if (lookahead == '(') {
            match('(');
        } else {
            errorSintactico("(");
            sincronizar(vector<int>{')', '{', '}', 0});
        }
        
        if (lookahead != ')' && lookahead != 0) {
            parseExpr();
        }
        
        if (lookahead == ')') {
            match(')');
        } else {
            errorSintactico(")");
            sincronizar(vector<int>{'{', '}', 0});
        }
        
        if (lookahead == '{') {
            match('{');
            program();
            
            if (lookahead == '}') {
                match('}');
            } else {
                errorSintactico("}");
                sincronizar(STMT_FOLLOW);
            }
        } else {
            errorSintactico("{");
            sincronizar(STMT_FOLLOW);
        }
        
        parseElseOpt();
    }
    // PRINT(EXPR);
    else if (lookahead == T_PRINT) {
        match(T_PRINT);
        
        if (lookahead == '(') {
            match('(');
        } else {
            errorSintactico("(");
            sincronizar(STMT_FOLLOW);
            return;
        }
        
        if (lookahead != ')' && lookahead != 0) {
            parseExpr();
        }
        
        if (lookahead == ')') {
            match(')');
        } else {
            errorSintactico(")");
            sincronizar(STMT_FOLLOW);
        }
        
        if (lookahead == ';') {
            match(';');
        } else {
            errorSintactico(";");
            sincronizar(STMT_FOLLOW);
            if (lookahead == ';') {
                match(';');
            }
        }
    }
    else {
        errorSintactico("instrucción válida (int, float, string, if, print)");
        sincronizar(STMT_FOLLOW);
    }
}

// ------------------------------------------------------------
// ELSE_OPT ::= else { PROGRAM } | ε
// ------------------------------------------------------------
void parseElseOpt() {
    if (lookahead == T_ELSE) {
        match(T_ELSE);
        
        if (lookahead == '{') {
            match('{');
            program();
            
            if (lookahead == '}') {
                match('}');
            } else {
                errorSintactico("}");
                sincronizar(STMT_FOLLOW);
            }
        } else {
            errorSintactico("{");
            sincronizar(STMT_FOLLOW);
        }
    }
    // ε (epsilon) - no hacer nada si no hay else
}

// ------------------------------------------------------------
//  EXPR ::= RELATIONAL EXPR'
//  EXPR' ::= (+|-|&&|||) RELATIONAL EXPR' | ε
// ------------------------------------------------------------
void parseExpr() {
    parseRelational();
    parseExprPrime();
}

void parseExprPrime() {
    if (lookahead == '+' || lookahead == '-' ||
        lookahead == T_AND || lookahead == T_OR) {
        match(lookahead);
        parseRelational();
        parseExprPrime();
    }
    // ε (epsilon) - no hacer nada
}

// ------------------------------------------------------------
// RELATIONAL ::= TERM RELATIONAL'
// RELATIONAL' ::= (==|!=|<|<=|>|>=) TERM RELATIONAL' | ε
// ------------------------------------------------------------
void parseRelational() {
    parseTerm();
    parseRelationalPrime();
}

void parseRelationalPrime() {
    if (lookahead == T_EQ || lookahead == T_NE ||
        lookahead == '<' || lookahead == T_LE ||
        lookahead == '>' || lookahead == T_GE) {
        match(lookahead);
        parseTerm();
        parseRelationalPrime();
    }
    // ε (epsilon) - no hacer nada
}

// ------------------------------------------------------------
// TERM ::= FACTOR TERM'
// TERM' ::= (*|/) FACTOR TERM' | ε
// ------------------------------------------------------------
void parseTerm() {
    parseFactor();
    parseTermPrime();
}

void parseTermPrime() {
    if (lookahead == '*' || lookahead == '/') {
        match(lookahead);
        parseFactor();
        parseTermPrime();
    }
    // ε (epsilon) - no hacer nada
}

// ------------------------------------------------------------
// FACTOR ::= NUMBER | STRLIT | ID | (EXPR)
// ------------------------------------------------------------
void parseFactor() {
    if (lookahead == T_NUMBER || lookahead == T_STRLIT || lookahead == T_ID) {
        match(lookahead);
    }
    else if (lookahead == '(') {
        match('(');
        parseExpr();
        
        if (lookahead == ')') {
            match(')');
        } else {
            errorSintactico(")");
        }
    }
    else {
        errorSintactico("expresión (número, cadena, identificador o '(')");
        // Avanzar un token para evitar bucle infinito
        if (lookahead != 0 && lookahead != ';' && lookahead != ')' && lookahead != '}') {
            lookahead = yylex();
        }
    }
}

// ------------------------------------------------------------
// Función para mostrar todos los errores
// ------------------------------------------------------------
void mostrarErrores() {
    if (!errores.empty()) {
        cerr << "\n=== ERRORES SINTÁCTICOS ENCONTRADOS ===" << endl;
        cerr << "Total de errores: " << errores.size() << "\n" << endl;
        
        for (size_t i = 0; i < errores.size(); i++) {
            cerr << "Error " << (i + 1) << " [Línea " << errores[i].linea << "]: " 
                 << errores[i].mensaje << endl;
        }
        cerr << "\n========================================" << endl;
    }
}

bool tieneErrores() {
    return hayErrores;
}