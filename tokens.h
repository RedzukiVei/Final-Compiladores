#ifndef TOKENS_H
#define TOKENS_H

#include <cstring>

typedef struct {
    char tipo[30];
    char valor[100];
    int linea;
} Token;

#endif