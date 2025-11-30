#include <iostream>
#include <vector>
#include <string>
#include "tokens.h"
#include "parser.h"

using namespace std;

// ============================================================
// Variables globales
// ============================================================
int lookahead;
extern int yylex();
extern int yylineno;
extern int tokenLinea;
extern char* yytext;

struct ErrorInfo {
    int linea;
    string mensaje;
};

vector<ErrorInfo> errores;
bool hayErrores = false;

// ============================================================
// Declaraciones forward
// ============================================================
void match(int expected);
void errorSintactico(const char* esperado);
void sincronizar(const vector<int>& siguientes);

void programa();
void decl();
void nl();
void globalDecl();
void funcion();
void bloque();
void params();
void parametro();
void tipo();
void tipobase();
void declvar();
void comando();
void cmdif();
void cmdwhile();
void cmdreturn();
void listaexp();
void exp();
void expOr();
void expOrPrime();
void expAnd();
void expAndPrime();
void expRel();
void expRelPrime();
void expAdd();
void expAddPrime();
void expMul();
void expMulPrime();
void expUnary();
void expFactor();

// ============================================================
// Conjuntos de sincronización
// ============================================================
const vector<int> SYNC_DECL = {T_FUN, T_ID, T_NL, T_EOF};
const vector<int> SYNC_COMANDO = {T_NL, T_END, T_LOOP, T_ELSE, T_EOF};
const vector<int> SYNC_EXP = {T_NL, ')', ',', ']', T_EOF};

// ============================================================
// Funciones auxiliares
// ============================================================
string nombreToken(int token) {
    switch(token) {
        case T_EOF: return "fin de archivo";
        case T_NL: return "salto de linea";
        case T_IF: return "if";
        case T_ELSE: return "else";
        case T_END: return "end";
        case T_WHILE: return "while";
        case T_LOOP: return "loop";
        case T_FUN: return "fun";
        case T_RETURN: return "return";
        case T_NEW: return "new";
        case T_STRING: return "string";
        case T_INT: return "int";
        case T_CHAR: return "char";
        case T_BOOL: return "bool";
        case T_TRUE: return "true";
        case T_FALSE: return "false";
        case T_AND: return "and";
        case T_OR: return "or";
        case T_NOT: return "not";
        case T_ID: return "identificador";
        case T_LITNUMERAL: return "numero";
        case T_LITSTRING: return "string literal";
        case T_GE: return ">=";
        case T_LE: return "<=";
        case T_NE: return "<>";
        case T_ERROR: return "caracter no reconocido";
        default:
            if (token > 0 && token < 256) {
                return string("'") + (char)token + "'";
            }
            return "token desconocido";
    }
}

void errorSintactico(const char* esperado) {
    ErrorInfo error;
    error.linea = tokenLinea;
    
    string encontrado;
    if (lookahead == T_EOF) {
        encontrado = "fin de archivo";
    } else if (lookahead == T_NL) {
        encontrado = "salto de linea";
    } else if (lookahead == T_ERROR) {
        encontrado = "caracter no reconocido";
    } else if (yytext != nullptr && yytext[0] != '\0') {
        encontrado = yytext;
    } else {
        encontrado = nombreToken(lookahead);
    }
    
    error.mensaje = string("Se esperaba ") + esperado + 
                    " pero se encontro '" + encontrado + "'";
    errores.push_back(error);
    hayErrores = true;
}

void sincronizar(const vector<int>& siguientes) {
    int intentos = 0;
    const int MAX_INTENTOS = 100;
    
    while (lookahead != T_EOF && intentos < MAX_INTENTOS) {
        for (int token : siguientes) {
            if (lookahead == token) {
                return;
            }
        }
        lookahead = yylex();
        intentos++;
    }
}

void match(int expected) {
    if (lookahead == expected) {
        lookahead = yylex();
    } else {
        errorSintactico(nombreToken(expected).c_str());
    }
}

void skipNL() {
    while (lookahead == T_NL) {
        lookahead = yylex();
    }
}

// ============================================================
// GRAMÁTICA Mini-0
// ============================================================

// programa -> { NL } decl { decl }
void programa() {
    skipNL();
    
    if (lookahead == T_EOF) {
        errorSintactico("al menos una declaracion");
        return;
    }
    
    decl();
    
    while (lookahead != T_EOF) {
        decl();
    }
}

// decl -> funcion | global
void decl() {
    if (lookahead == T_FUN) {
        funcion();
    } else if (lookahead == T_ID) {
        globalDecl();
    } else if (lookahead == T_ERROR) {
        errorSintactico("declaracion valida");
        lookahead = yylex();
        skipNL();
    } else {
        errorSintactico("declaracion (fun o identificador)");
        sincronizar(SYNC_DECL);
        skipNL();
    }
}

// nl -> NL { NL }
void nl() {
    if (lookahead == T_NL) {
        match(T_NL);
        while (lookahead == T_NL) {
            match(T_NL);
        }
    } else if (lookahead != T_EOF) {
        errorSintactico("salto de linea");
        sincronizar(SYNC_COMANDO);
    }
}

// global -> declvar nl
void globalDecl() {
    declvar();
    nl();
}

// funcion -> 'fun' ID '(' params ')' [ ':' tipo ] nl bloque 'end' nl
void funcion() {
    match(T_FUN);
    
    if (lookahead == T_ID) {
        match(T_ID);
    } else {
        errorSintactico("nombre de funcion");
        sincronizar(SYNC_DECL);
        return;
    }
    
    if (lookahead == '(') {
        match('(');
    } else {
        errorSintactico("'('");
    }
    
    params();
    
    if (lookahead == ')') {
        match(')');
    } else {
        errorSintactico("')'");
    }
    
    if (lookahead == ':') {
        match(':');
        tipo();
    }
    
    nl();
    bloque();
    
    if (lookahead == T_END) {
        match(T_END);
    } else {
        errorSintactico("'end'");
        sincronizar(SYNC_DECL);
    }
    
    nl();
}

// bloque -> { declvar nl } { comando nl }
void bloque() {
    while (lookahead == T_ID) {
        int savedLinea = tokenLinea;
        match(T_ID);
        
        if (lookahead == ':') {
            match(':');
            tipo();
            nl();
        } else {
            if (lookahead == '=') {
                match('=');
                exp();
                nl();
            } else if (lookahead == '(') {
                match('(');
                listaexp();
                if (lookahead == ')') {
                    match(')');
                } else {
                    errorSintactico("')'");
                }
                nl();
            } else if (lookahead == '[') {
                while (lookahead == '[') {
                    match('[');
                    exp();
                    if (lookahead == ']') {
                        match(']');
                    } else {
                        errorSintactico("']'");
                    }
                }
                if (lookahead == '=') {
                    match('=');
                    exp();
                }
                nl();
            } else {
                errorSintactico("':' o '=' o '('");
                sincronizar(SYNC_COMANDO);
                if (lookahead == T_NL) nl();
            }
            break;
        }
    }
    
    while (lookahead == T_IF || lookahead == T_WHILE || 
           lookahead == T_RETURN || lookahead == T_ID) {
        comando();
        nl();
    }
}

// params -> /* vacio */ | parametro { ',' parametro }
void params() {
    if (lookahead == T_ID) {
        parametro();
        while (lookahead == ',') {
            match(',');
            parametro();
        }
    }
}

// parametro -> ID ':' tipo
void parametro() {
    if (lookahead == T_ID) {
        match(T_ID);
    } else {
        errorSintactico("nombre de parametro");
        return;
    }
    
    if (lookahead == ':') {
        match(':');
    } else {
        errorSintactico("':'");
        return;
    }
    
    tipo();
}

// tipo -> tipobase | '[' ']' tipo
void tipo() {
    if (lookahead == '[') {
        match('[');
        if (lookahead == ']') {
            match(']');
        } else {
            errorSintactico("']'");
        }
        tipo();
    } else {
        tipobase();
    }
}

// tipobase -> 'int' | 'bool' | 'char' | 'string'
void tipobase() {
    if (lookahead == T_INT || lookahead == T_BOOL || 
        lookahead == T_CHAR || lookahead == T_STRING) {
        match(lookahead);
    } else {
        errorSintactico("tipo (int, bool, char, string)");
    }
}

// declvar -> ID ':' tipo
void declvar() {
    if (lookahead == T_ID) {
        match(T_ID);
    } else {
        errorSintactico("identificador");
        return;
    }
    
    if (lookahead == ':') {
        match(':');
    } else {
        errorSintactico("':'");
        return;
    }
    
    tipo();
}

// comando -> cmdif | cmdwhile | cmdatrib | cmdreturn | llamada
void comando() {
    if (lookahead == T_IF) {
        cmdif();
    } else if (lookahead == T_WHILE) {
        cmdwhile();
    } else if (lookahead == T_RETURN) {
        cmdreturn();
    } else if (lookahead == T_ID) {
        match(T_ID);
        
        while (lookahead == '[') {
            match('[');
            exp();
            if (lookahead == ']') {
                match(']');
            } else {
                errorSintactico("']'");
            }
        }
        
        if (lookahead == '=') {
            match('=');
            exp();
        } else if (lookahead == '(') {
            match('(');
            listaexp();
            if (lookahead == ')') {
                match(')');
            } else {
                errorSintactico("')'");
            }
        } else {
            errorSintactico("'=' o '('");
        }
    } else {
        errorSintactico("comando (if, while, return, identificador)");
        sincronizar(SYNC_COMANDO);
    }
}

// cmdif -> 'if' exp nl bloque { 'else' 'if' exp nl bloque } [ 'else' nl bloque ] 'end'
void cmdif() {
    match(T_IF);
    exp();
    nl();
    bloque();
    
    while (lookahead == T_ELSE) {
        match(T_ELSE);
        
        if (lookahead == T_IF) {
            match(T_IF);
            exp();
            nl();
            bloque();
        } else {
            nl();
            bloque();
            break;
        }
    }
    
    if (lookahead == T_END) {
        match(T_END);
    } else {
        errorSintactico("'end'");
    }
}

// cmdwhile -> 'while' exp nl bloque 'loop'
void cmdwhile() {
    match(T_WHILE);
    exp();
    nl();
    bloque();
    
    if (lookahead == T_LOOP) {
        match(T_LOOP);
    } else {
        errorSintactico("'loop'");
    }
}

// cmdreturn -> 'return' exp | 'return'
void cmdreturn() {
    match(T_RETURN);
    
    if (lookahead != T_NL && lookahead != T_EOF) {
        exp();
    }
}

// listaexp -> /* vacio */ | exp { ',' exp }
void listaexp() {
    if (lookahead != ')') {
        exp();
        while (lookahead == ',') {
            match(',');
            exp();
        }
    }
}

// ============================================================
// EXPRESIONES (con precedencia correcta estilo C)
// ============================================================

void exp() {
    expOr();
}

void expOr() {
    expAnd();
    expOrPrime();
}

void expOrPrime() {
    if (lookahead == T_OR) {
        match(T_OR);
        expAnd();
        expOrPrime();
    }
}

void expAnd() {
    expRel();
    expAndPrime();
}

void expAndPrime() {
    if (lookahead == T_AND) {
        match(T_AND);
        expRel();
        expAndPrime();
    }
}

void expRel() {
    expAdd();
    expRelPrime();
}

void expRelPrime() {
    if (lookahead == '<' || lookahead == '>' || 
        lookahead == T_LE || lookahead == T_GE ||
        lookahead == '=' || lookahead == T_NE) {
        match(lookahead);
        expAdd();
        expRelPrime();
    }
}

void expAdd() {
    expMul();
    expAddPrime();
}

void expAddPrime() {
    if (lookahead == '+' || lookahead == '-') {
        match(lookahead);
        expMul();
        expAddPrime();
    }
}

void expMul() {
    expUnary();
    expMulPrime();
}

void expMulPrime() {
    if (lookahead == '*' || lookahead == '/') {
        match(lookahead);
        expUnary();
        expMulPrime();
    }
}

void expUnary() {
    if (lookahead == T_NOT) {
        match(T_NOT);
        expUnary();
    } else if (lookahead == '-') {
        match('-');
        expUnary();
    } else {
        expFactor();
    }
}

void expFactor() {
    if (lookahead == T_LITNUMERAL) {
        match(T_LITNUMERAL);
    } else if (lookahead == T_LITSTRING) {
        match(T_LITSTRING);
    } else if (lookahead == T_TRUE) {
        match(T_TRUE);
    } else if (lookahead == T_FALSE) {
        match(T_FALSE);
    } else if (lookahead == T_NEW) {
        match(T_NEW);
        if (lookahead == '[') {
            match('[');
        } else {
            errorSintactico("'['");
        }
        exp();
        if (lookahead == ']') {
            match(']');
        } else {
            errorSintactico("']'");
        }
        tipo();
    } else if (lookahead == '(') {
        match('(');
        exp();
        if (lookahead == ')') {
            match(')');
        } else {
            errorSintactico("')'");
        }
    } else if (lookahead == T_ID) {
        match(T_ID);
        
        if (lookahead == '(') {
            match('(');
            listaexp();
            if (lookahead == ')') {
                match(')');
            } else {
                errorSintactico("')'");
            }
        } else {
            while (lookahead == '[') {
                match('[');
                exp();
                if (lookahead == ']') {
                    match(']');
                } else {
                    errorSintactico("']'");
                }
            }
        }
    } else {
        errorSintactico("expresion");
        sincronizar(SYNC_EXP);
    }
}

// ============================================================
// Funciones de reporte de errores
// ============================================================

void mostrarErrores() {
    if (!errores.empty()) {
        cerr << "\n=== ERRORES SINTACTICOS ENCONTRADOS ===" << endl;
        cerr << "Total de errores: " << errores.size() << "\n" << endl;
        
        for (size_t i = 0; i < errores.size(); i++) {
            cerr << "Error " << (i + 1) << " [Linea " << errores[i].linea << "]: " 
                 << errores[i].mensaje << endl;
        }
        cerr << "\n========================================" << endl;
    }
}

bool tieneErrores() {
    return hayErrores;
}