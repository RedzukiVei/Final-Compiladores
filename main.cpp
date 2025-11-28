#include <iostream>
#include <cstdio>
#include <cstring>
#include "parser.h"

extern int lookahead;
extern FILE* yyin;
int yylex();

bool validarExtension(const char* nombreArchivo) {
    const char* extension = strrchr(nombreArchivo, '.');
    if (extension == NULL) {
        return false;
    }
    return strcmp(extension, ".m0") == 0;
}

int main(int argc, char* argv[]) {

    if (argc < 2) {
        std::cerr << "Uso: ./parser <archivo.m0>" << std::endl;
        return 1;
    }
    // ------------------------------------------------------------
    // Validar que el archivo tenga extensión .m0
    // ------------------------------------------------------------
    if (!validarExtension(argv[1])) {
        std::cerr << "Error: El archivo debe tener extensión .m0" << std::endl;
        std::cerr << "Archivo proporcionado: " << argv[1] << std::endl;
        return 1;
    }

    yyin = fopen(argv[1], "r");
    if (!yyin) {
        std::cerr << "Error: No se pudo abrir el archivo '" << argv[1] << "'" << std::endl;
        return 1;
    }

    lookahead = yylex();   // Primer token

    // ------------------------------------------------------------
    // Ejecutar el parser
    // ------------------------------------------------------------
    program();

    // Verificar si hubo errores
    if (tieneErrores()) {
        mostrarErrores();  // Mostrar lista completa de errores
        fclose(yyin);
        return 1;
    } else {
        std::cout << "Análisis sintáctico exitoso" << std::endl;
        fclose(yyin);
        return 0;
    }
}