#include "parser.h"
#include <iostream>
#include <algorithm>

Parser::Parser(const std::vector<Token>& tokens) 
    : tokens(tokens), pos(0), hayError(false) {}

Token Parser::tokenActual() {
    if (pos < tokens.size()) {
        return tokens[pos];
    }
    // ------------------------------------------------------------
    // Token EOF artificial
    // ------------------------------------------------------------
    Token eof;
    strcpy(eof.tipo, "EOF");
    strcpy(eof.valor, "");
    eof.linea = tokens.empty() ? 1 : tokens.back().linea;
    return eof;
}

Token Parser::avanzar() {
    Token actual = tokenActual();
    if (pos < tokens.size()) pos++;
    return actual;
}

bool Parser::coincidir(const std::string& tipoEsperado) {
    if (std::string(tokenActual().tipo) == tipoEsperado) {
        avanzar();
        return true;
    }
    return false;
}

void Parser::registrarError(const std::string& mensaje) {
    Token actual = tokenActual();
    std::string error = "Error en línea " + std::to_string(actual.linea) + 
                       ": " + mensaje + " (token encontrado: '" + 
                       std::string(actual.valor) + "')";
    errores.push_back(error);
    hayError = true;
}

void Parser::sincronizar(const std::vector<std::string>& siguientes) {
    while (pos < tokens.size()) {
        std::string tipoActual = tokenActual().tipo;
        if (std::find(siguientes.begin(), siguientes.end(), tipoActual) != siguientes.end()) {
            break;
        }
        avanzar();
    }
}

// ------------------------------------------------------------
// programa → int main() { declaraciones sentencias }
// ------------------------------------------------------------
void Parser::programa() {
    if (!coincidir("PALABRA_CLAVE")) {
        registrarError("Se esperaba 'int' al inicio del programa");
        sincronizar({"IDENTIFICADOR"});
    }
    
    if (!coincidir("IDENTIFICADOR")) {
        registrarError("Se esperaba 'main'");
        sincronizar({"PAREN_IZQ"});
    }
    
    if (!coincidir("PAREN_IZQ")) {
        registrarError("Se esperaba '('");
        sincronizar({"PAREN_DER"});
    }
    
    if (!coincidir("PAREN_DER")) {
        registrarError("Se esperaba ')'");
        sincronizar({"LLAVE_IZQ"});
    }
    
    if (!coincidir("LLAVE_IZQ")) {
        registrarError("Se esperaba '{'");
        sincronizar({"PALABRA_CLAVE", "IDENTIFICADOR"});
    }
    
    declaraciones();
    sentencias();
    
    if (!coincidir("LLAVE_DER")) {
        registrarError("Se esperaba '}' al final del programa");
    }
}

// ------------------------------------------------------------
// declaraciones → declaracion declaraciones | ε
// ------------------------------------------------------------
void Parser::declaraciones() {
    std::string tipo = tokenActual().tipo;
    std::string valor = tokenActual().valor;
    
    // First(declaraciones) = {int, float, char, string, ε}
    if (tipo == "PALABRA_CLAVE" && 
        (valor == std::string("int") || valor == std::string("float") || 
         valor == std::string("char") || valor == std::string("string"))) {
        declaracion();
        declaraciones();
    }
    // ε - producción vacía
}

// ------------------------------------------------------------
// declaracion → tipo IDENTIFICADOR = expresion ;
// ------------------------------------------------------------
void Parser::declaracion() {
    tipo();
    
    if (!coincidir("IDENTIFICADOR")) {
        registrarError("Se esperaba un identificador en la declaración");
        sincronizar({"ASIGNACION", "PUNTO_Y_COMA"});
    }
    
    if (!coincidir("ASIGNACION")) {
        registrarError("Se esperaba '=' en la declaración");
        sincronizar({"NUMERO_DECIMAL", "NUMERO_HEX", "NUMERO_REAL", 
                     "CADENA", "IDENTIFICADOR", "PAREN_IZQ"});
    }
    
    expresion();
    
    if (!coincidir("PUNTO_Y_COMA")) {
        registrarError("Se esperaba ';' al final de la declaración");
        sincronizar({"PALABRA_CLAVE", "IDENTIFICADOR", "LLAVE_DER"});
    }
}

// ------------------------------------------------------------
// tipo → int | float | char | string
// ------------------------------------------------------------
void Parser::tipo() {
    if (!coincidir("PALABRA_CLAVE")) {
        registrarError("Se esperaba un tipo de dato (int, float, char, string)");
        sincronizar({"IDENTIFICADOR"});
    }
}

// ------------------------------------------------------------
// sentencias → sentencia sentencias | ε
// ------------------------------------------------------------
void Parser::sentencias() {
    std::string tipo = tokenActual().tipo;
    std::string valor = tokenActual().valor;
    
    // First(sentencias) = {if, while, for, print, IDENTIFICADOR, ε}
    if (tipo == "PALABRA_CLAVE" || tipo == "IDENTIFICADOR") {
        sentencia();
        sentencias();
    }
    // ε - producción vacía
}

// ------------------------------------------------------------
// sentencia → sentenciaIf | sentenciaWhile | sentenciaFor | sentenciaAsignacion | sentenciaPrint
// ------------------------------------------------------------
void Parser::sentencia() {
    std::string tipo = tokenActual().tipo;
    std::string valor = tokenActual().valor;
    
    if (tipo == "PALABRA_CLAVE") {
        if (valor == std::string("if")) {
            sentenciaIf();
        } else if (valor == std::string("while")) {
            sentenciaWhile();
        } else if (valor == std::string("for")) {
            sentenciaFor();
        } else if (valor == std::string("print")) {
            sentenciaPrint();
        } else {
            registrarError("Palabra clave inesperada en sentencia");
            sincronizar({"PUNTO_Y_COMA", "LLAVE_DER"});
        }
    } else if (tipo == "IDENTIFICADOR") {
        sentenciaAsignacion();
    } else if (tipo == "ERROR") {
        registrarError("Token no reconocido: '" + std::string(tokenActual().valor) + "'");
        avanzar();
        sincronizar({"PUNTO_Y_COMA", "LLAVE_DER"});
    } else {
        registrarError("Se esperaba una sentencia");
        sincronizar({"PUNTO_Y_COMA", "LLAVE_DER"});
    }
}

// ------------------------------------------------------------
// sentenciaIf → if ( expresion ) { sentencias } | if ( expresion ) { sentencias } else { sentencias }
// ------------------------------------------------------------
void Parser::sentenciaIf() {
    coincidir("PALABRA_CLAVE"); // if
    
    if (!coincidir("PAREN_IZQ")) {
        registrarError("Se esperaba '(' después de 'if'");
        sincronizar({"IDENTIFICADOR", "NUMERO_DECIMAL", "PAREN_IZQ"});
    }
    
    expresion();
    
    if (!coincidir("PAREN_DER")) {
        registrarError("Se esperaba ')' después de la condición");
        sincronizar({"LLAVE_IZQ"});
    }
    
    if (!coincidir("LLAVE_IZQ")) {
        registrarError("Se esperaba '{' después de la condición del if");
        sincronizar({"PALABRA_CLAVE", "IDENTIFICADOR"});
    }
    
    sentencias();
    
    if (!coincidir("LLAVE_DER")) {
        registrarError("Se esperaba '}' al final del bloque if");
        sincronizar({"PALABRA_CLAVE"});
    }
    
    // else opcional
    if (std::string(tokenActual().valor) == "else") {
        coincidir("PALABRA_CLAVE"); // else
        
        if (!coincidir("LLAVE_IZQ")) {
            registrarError("Se esperaba '{' después de 'else'");
            sincronizar({"PALABRA_CLAVE", "IDENTIFICADOR"});
        }
        
        sentencias();
        
        if (!coincidir("LLAVE_DER")) {
            registrarError("Se esperaba '}' al final del bloque else");
        }
    }
}

// ------------------------------------------------------------
// sentenciaWhile → while ( expresion ) { sentencias }
// ------------------------------------------------------------
void Parser::sentenciaWhile() {
    coincidir("PALABRA_CLAVE"); // while
    
    if (!coincidir("PAREN_IZQ")) {
        registrarError("Se esperaba '(' después de 'while'");
        sincronizar({"IDENTIFICADOR", "NUMERO_DECIMAL"});
    }
    
    expresion();
    
    if (!coincidir("PAREN_DER")) {
        registrarError("Se esperaba ')' después de la condición");
        sincronizar({"LLAVE_IZQ"});
    }
    
    if (!coincidir("LLAVE_IZQ")) {
        registrarError("Se esperaba '{' después de la condición del while");
        sincronizar({"PALABRA_CLAVE", "IDENTIFICADOR"});
    }
    
    sentencias();
    
    if (!coincidir("LLAVE_DER")) {
        registrarError("Se esperaba '}' al final del bloque while");
    }
}

// ------------------------------------------------------------
// sentenciaFor → for ( sentenciaAsignacion expresion ; expresion ) { sentencias }
// ------------------------------------------------------------
void Parser::sentenciaFor() {
    coincidir("PALABRA_CLAVE"); // for
    
    if (!coincidir("PAREN_IZQ")) {
        registrarError("Se esperaba '(' después de 'for'");
    }
    
    sentenciaAsignacion(); // inicialización
    expresion(); // condición
    
    if (!coincidir("PUNTO_Y_COMA")) {
        registrarError("Se esperaba ';' después de la condición del for");
    }
    
    expresion(); // incremento
    
    if (!coincidir("PAREN_DER")) {
        registrarError("Se esperaba ')' después del incremento");
        sincronizar({"LLAVE_IZQ"});
    }
    
    if (!coincidir("LLAVE_IZQ")) {
        registrarError("Se esperaba '{' después del encabezado del for");
        sincronizar({"PALABRA_CLAVE", "IDENTIFICADOR"});
    }
    
    sentencias();
    
    if (!coincidir("LLAVE_DER")) {
        registrarError("Se esperaba '}' al final del bloque for");
    }
}

// ------------------------------------------------------------
// sentenciaAsignacion → IDENTIFICADOR = expresion ;
// ------------------------------------------------------------
void Parser::sentenciaAsignacion() {
    if (!coincidir("IDENTIFICADOR")) {
        registrarError("Se esperaba un identificador");
        sincronizar({"ASIGNACION"});
    }
    
    if (!coincidir("ASIGNACION")) {
        registrarError("Se esperaba '=' en la asignación");
        sincronizar({"NUMERO_DECIMAL", "IDENTIFICADOR"});
    }
    
    expresion();
    
    if (!coincidir("PUNTO_Y_COMA")) {
        registrarError("Se esperaba ';' al final de la asignación");
        sincronizar({"PALABRA_CLAVE", "IDENTIFICADOR", "LLAVE_DER"});
    }
}

// ------------------------------------------------------------
// sentenciaPrint → print ( CADENA ) ;
// ------------------------------------------------------------
void Parser::sentenciaPrint() {
    coincidir("PALABRA_CLAVE"); // print
    
    if (!coincidir("PAREN_IZQ")) {
        registrarError("Se esperaba '(' después de 'print'");
        sincronizar({"CADENA"});
    }
    
    if (!coincidir("CADENA")) {
        registrarError("Se esperaba una cadena en print");
        sincronizar({"PAREN_DER"});
    }
    
    if (!coincidir("PAREN_DER")) {
        registrarError("Se esperaba ')' después de la cadena");
        sincronizar({"PUNTO_Y_COMA"});
    }
    
    if (!coincidir("PUNTO_Y_COMA")) {
        registrarError("Se esperaba ';' al final de print");
        sincronizar({"PALABRA_CLAVE", "IDENTIFICADOR", "LLAVE_DER"});
    }
}

// ------------------------------------------------------------
// expresion → expresionLogica
// ------------------------------------------------------------
void Parser::expresion() {
    expresionLogica();
}

// ------------------------------------------------------------
// expresionLogica → expresionRelacional expresionLogicaPrima
// ------------------------------------------------------------
void Parser::expresionLogica() {
    expresionRelacional();
    expresionLogicaPrima();
}

// ------------------------------------------------------------
// expresionLogicaPrima → && expresionRelacional expresionLogicaPrima | || expresionRelacional expresionLogicaPrima | ε
// ------------------------------------------------------------
void Parser::expresionLogicaPrima() {
    std::string valor = tokenActual().valor;
    
    if (valor == std::string("&&") || valor == std::string("||")) {
        avanzar();
        expresionRelacional();
        expresionLogicaPrima();
    }
    // ε - producción vacía
}

// ------------------------------------------------------------
// expresionRelacional → expresionAditiva expresionRelacionalPrima
// ------------------------------------------------------------
void Parser::expresionRelacional() {
    expresionAditiva();
    expresionRelacionalPrima();
}

// ------------------------------------------------------------
// expresionRelacionalPrima → operadorRelacional expresionAditiva | ε
// ------------------------------------------------------------
void Parser::expresionRelacionalPrima() {
    std::string tipo = tokenActual().tipo;
    
    if (tipo == "IGUALDAD" || tipo == "DISTINTO" || tipo == "MENOR" || 
        tipo == "MAYOR" || tipo == "MENOR_IGUAL" || tipo == "MAYOR_IGUAL") {
        operadorRelacional();
        expresionAditiva();
    }
    // ε - producción vacía
}

// ------------------------------------------------------------
// expresionAditiva → expresionMultiplicativa expresionAditivaPrima
// ------------------------------------------------------------
void Parser::expresionAditiva() {
    expresionMultiplicativa();
    expresionAditivaPrima();
}

// ------------------------------------------------------------
// expresionAditivaPrima → + expresionMultiplicativa expresionAditivaPrima | - expresionMultiplicativa expresionAditivaPrima | ε
// ------------------------------------------------------------
void Parser::expresionAditivaPrima() {
    std::string tipo = tokenActual().tipo;
    
    if (tipo == "MAS" || tipo == "MENOS") {
        avanzar();
        expresionMultiplicativa();
        expresionAditivaPrima();
    }
    // ε - producción vacía
}

// ------------------------------------------------------------
// expresionMultiplicativa → factor expresionMultiplicativaPrima
// ------------------------------------------------------------
void Parser::expresionMultiplicativa() {
    factor();
    expresionMultiplicativaPrima();
}

// ------------------------------------------------------------
// expresionMultiplicativaPrima → * factor expresionMultiplicativaPrima | / factor expresionMultiplicativaPrima | ε
// ------------------------------------------------------------
void Parser::expresionMultiplicativaPrima() {
    std::string tipo = tokenActual().tipo;
    
    if (tipo == "MULT" || tipo == "DIV") {
        avanzar();
        factor();
        expresionMultiplicativaPrima();
    }
    // ε - producción vacía
}

// ------------------------------------------------------------
// factor → NUMERO_DECIMAL | NUMERO_HEX | NUMERO_REAL | IDENTIFICADOR | ( expresion )
// ------------------------------------------------------------
void Parser::factor() {
    std::string tipo = tokenActual().tipo;
    
    if (tipo == "NUMERO_DECIMAL" || tipo == "NUMERO_HEX" || 
        tipo == "NUMERO_REAL" || tipo == "IDENTIFICADOR" || tipo == "CADENA") {
        avanzar();
    } else if (tipo == "PAREN_IZQ") {
        avanzar();
        expresion();
        if (!coincidir("PAREN_DER")) {
            registrarError("Se esperaba ')' después de la expresión");
        }
    } else {
        registrarError("Se esperaba un número, identificador o '('");
        sincronizar({"MAS", "MENOS", "MULT", "DIV", "PUNTO_Y_COMA", "PAREN_DER"});
    }
}

// ------------------------------------------------------------
// operadorRelacional → == | != | < | > | <= | >=
// ------------------------------------------------------------
void Parser::operadorRelacional() {
    avanzar();
}

bool Parser::analizar() {
    programa();
    
    // Verificar si llegamos al final
    if (pos < tokens.size() && std::string(tokenActual().tipo) != "EOF") {
        registrarError("Contenido inesperado después del programa principal");
    }
    
    return !hayError;
}

void Parser::mostrarErrores() {
    if (!errores.empty()) {
        std::cerr << "\n=== ERRORES SINTÁCTICOS DETECTADOS ===\n\n";
        for (const auto& error : errores) {
            std::cerr << error << std::endl;
        }
        std::cerr << "\nTotal de errores: " << errores.size() << "\n";
    }
}