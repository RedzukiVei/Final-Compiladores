Este proyecto implementa un analizador sintáctico sencillo para un lenguaje tipo Mini0, utilizando Flex para el análisis léxico y un parser descendente recursivo escrito manualmente en C++.

1. Descripción general

El sistema está compuesto por:

mini.l – Archivo Flex que define el scanner.
Se encarga de reconocer:

Palabras reservadas: int, float, string, if, else, print.

Identificadores.

Números.

Cadenas entre comillas.

Operadores (==, !=, <=, >=, &&, ||, +, -, *, /, <, >, =).

Símbolos especiales como (, ), {, }, ;.

parser.cpp – Implementación del analizador sintáctico.
Realiza el reconocimiento de:

Declaraciones de variables.

Sentencias if y else.

Sentencias print.

Expresiones aritméticas y lógicas.

Bloques { ... }.

tokens.h / tokens.cpp – Definiciones y variables globales del sistema de tokens.

main.cpp – Carga el archivo de entrada y ejecuta el análisis sintáctico.

2. Funcionamiento del programa

El usuario debe proporcionar un archivo con extensión .m0.

El scanner (Flex) lee el archivo y devuelve tokens.

El parser interpreta esos tokens siguiendo la gramática definida.

Si el código es válido, el programa imprime:

Análisis sintáctico exitoso


Si el código contiene errores de sintaxis, se muestra un mensaje indicando la línea y el elemento esperado.

3. Compilación

El proyecto se compila con:

flex mini.l
g++ lex.yy.c parser.cpp main.cpp tokens.cpp -o mini0

4. Ejecución

Para analizar un archivo:

./mini0 ruta/del/archivo.m0


Ejemplo:

./mini0 pruebas/prueba_ok1.m0

5. Pruebas

En la carpeta pruebas se incluyen:

prueba_ok1.m0 — Código válido.

prueba_bad1.m0 — Código con error sintáctico.