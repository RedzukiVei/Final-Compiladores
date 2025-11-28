#ifndef PARSER_H
#define PARSER_H

int program();
void errorSintactico(const char* esperado);

void mostrarErrores();
bool tieneErrores();

#endif
