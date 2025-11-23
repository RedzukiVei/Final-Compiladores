#ifndef TOKENS_H
#define TOKENS_H

// Estructura original del token (para el lexer)
typedef struct {
    char tipo[30];
    char valor[100];
    int linea;
} Token;

// Tipos de tokens para el parser (enumeración)
typedef enum {
    // Palabras clave
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_WHILE,
    TOKEN_FOR,
    TOKEN_INT,
    TOKEN_FLOAT,
    TOKEN_CHAR,
    TOKEN_RETURN,
    
    // Identificadores y literales
    TOKEN_ID,
    TOKEN_NUM_DEC,
    TOKEN_NUM_HEX,
    TOKEN_NUM_REAL,
    TOKEN_STRING,
    
    // Operadores relacionales
    TOKEN_EQ,        // ==
    TOKEN_NEQ,       // !=
    TOKEN_LE,        // <=
    TOKEN_GE,        // >=
    TOKEN_LT,        // <
    TOKEN_GT,        // >
    TOKEN_ASSIGN,    // =
    
    // Operadores aritméticos
    TOKEN_PLUS,      // +
    TOKEN_MINUS,     // -
    TOKEN_MULT,      // *
    TOKEN_DIV,       // /
    
    // Delimitadores
    TOKEN_SEMICOLON, // ;
    TOKEN_COMMA,     // ,
    TOKEN_LPAREN,    // (
    TOKEN_RPAREN,    // )
    TOKEN_LBRACE,    // {
    TOKEN_RBRACE,    // }
    
    // Especiales
    TOKEN_EOF,       // Fin de archivo
    TOKEN_ERROR,     // Token con error
    TOKEN_UNKNOWN    // Token no reconocido
} TokenType;

// Estructura del token para el parser
typedef struct {
    TokenType type;      // Tipo enumerado del token
    char lexeme[100];    // Texto del token
    int line;            // Número de línea
} ParserToken;

// Nombres de los tokens para debugging/mensajes de error
static const char* TokenTypeNames[] = {
    "IF", "ELSE", "WHILE", "FOR",
    "INT", "FLOAT", "CHAR", "RETURN",
    "IDENTIFICADOR", "NUMERO_DECIMAL", "NUMERO_HEX", "NUMERO_REAL", "CADENA",
    "==", "!=", "<=", ">=", "<", ">", "=",
    "+", "-", "*", "/",
    ";", ",", "(", ")", "{", "}",
    "EOF", "ERROR", "UNKNOWN"
};

// Función auxiliar para obtener el nombre de un token (opcional, útil para debugging)
static inline const char* getTokenName(TokenType type) {
    if (type >= 0 && type <= TOKEN_UNKNOWN) {
        return TokenTypeNames[type];
    }
    return "INVALID";
}

#endif // TOKENS_H