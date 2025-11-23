#define _TIMESPEC_DEFINED 1
#include <stdio.h>
#include <stdlib.h>
#include "parser.h"

extern FILE* yyin;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Uso: %s <archivo.m0>\n", argv[0]);
        return 1;
    }

    yyin = fopen(argv[1], "r");
    if (!yyin) {
        perror("No se pudo abrir el archivo");
        return 1;
    }

    printf("=== COMPILADOR MINI-0 ===\n\n");
    printf("--- Fase 1: Análisis Léxico ---\n");
    
    // Inicializar el parser (que usará el lexer internamente)
    initParser(yyin);
    
    printf("\n--- Fase 2: Análisis Sintáctico ---\n");
    
    // Ejecutar el análisis sintáctico
    int result = parse();
    
    fclose(yyin);
    
    if (result == 0) {
        printf("\n=== COMPILACIÓN EXITOSA ===\n");
    } else {
        fprintf(stderr, "\n=== COMPILACIÓN FALLIDA ===\n");
    }
    
    return result;
}