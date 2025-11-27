#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <string>
#include "tokens.h"

class Parser {
private:
    std::vector<Token> tokens;
    size_t pos;
    std::vector<std::string> errores;
    bool hayError;
    
    Token tokenActual();
    Token avanzar();
    bool coincidir(const std::string& tipoEsperado);
    void registrarError(const std::string& mensaje);
    void sincronizar(const std::vector<std::string>& siguientes);
    
    // ------------------------------------------------------------
    // Funciones para cada no-terminal de la gramática
    // ------------------------------------------------------------
    void programa();
    void declaraciones();
    void declaracion();
    void tipo();
    void sentencias();
    void sentencia();
    void sentenciaIf();
    void sentenciaWhile();
    void sentenciaFor();
    void sentenciaAsignacion();
    void sentenciaPrint();
    void expresion();
    void expresionLogica();
    void expresionLogicaPrima();
    void expresionRelacional();
    void expresionRelacionalPrima();
    void expresionAditiva();
    void expresionAditivaPrima();
    void expresionMultiplicativa();
    void expresionMultiplicativaPrima();
    void factor();
    void operadorRelacional();
    
public:
    Parser(const std::vector<Token>& tokens);
    bool analizar();
    void mostrarErrores();
};

#endif