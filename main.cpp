#define _TIMESPEC_DEFINED 1
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include "tokens.h"
#include "parser.h"

extern FILE* yyin;
int yylex();

std::vector<Token> listaTokens;
bool hayErrorLexico = false;

extern "C" int agregarToken(const char* tipo, const char* valor, int linea) {
    Token t;
    strncpy(t.tipo, tipo, 29);
    t.tipo[29] = '\0';
    strncpy(t.valor, valor, 99);
    t.valor[99] = '\0';
    t.linea = linea;
    
    listaTokens.push_back(t);
    
    if (std::string(tipo) == "ERROR") {
        hayErrorLexico = true;
    }
    
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <archivo.m0>\n", argv[0]);
        return 1;
    }

    yyin = fopen(argv[1], "r");
    if (!yyin) {
        perror("No se pudo abrir el archivo");
        return 1;
    }
    // ------------------------------------------------------------
    // Fase 1: Análisis Léxico
    // ------------------------------------------------------------
    yylex();
    fclose(yyin);
    
    // Verificar errores léxicos
    if (hayErrorLexico) {
        std::cerr << "\n=== ERRORES LÉXICOS DETECTADOS ===\n\n";
        for (const auto& token : listaTokens) {
            if (std::string(token.tipo) == "ERROR") {
                std::cerr << "Error en línea " << token.linea 
                         << ": Token no reconocido '" << token.valor << "'\n";
            }
        }
        std::cerr << "\n=== ANÁLISIS ABORTADO ===\n";
        return 1;
    }
    
    // ------------------------------------------------------------
    // Fase 2: Análisis Sintáctico
    // ------------------------------------------------------------
    Parser parser(listaTokens);
    bool exitoso = parser.analizar();
    
    if (!exitoso) {
        parser.mostrarErrores();
        std::cerr << "\n=== ANÁLISIS SINTÁCTICO FALLIDO ===\n";
        return 1;
    }
    
    
    std::cout << "\n=== ANÁLISIS SINTÁCTICO EXITOSO ===\n";
    std::cout << "El programa es sintácticamente correcto.\n";
    
    return 0;
}