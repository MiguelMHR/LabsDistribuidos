// ----------     LIBRERÍAS Y DEFINICIONES     ---------- //
#ifndef MESSAGES_H
#define MESSAGES_H

// Definiciones básicas
#define MAX_ARRAY_LENGTH 1024     // Longitud máxima de una cadena de caracteres
#define MAX_VALUE1_LENGTH 256   // Longitud máxima dada por el enunciado
#define MAX_N_VALUE2 32         // Longitud máxima dada por el enunciado


// ----------     ESTRUCTURAS     ---------- //

// Para poder comunicarse, se debe establecer un código de operación
// para saber qué debe ejecutar el servidor. Los códigos de operación son:
//     1: init()
//     2: set_value()
//     3: get_value()
//     4: modify_value()
//     5: delete_key()
//     6: exists()
// La variable operation guardará la opción elegida por el cliente

// Estructura de la solicitud (pasada al hilo en servidor.c)
typedef struct {
    int socket;         // Descriptor del socket
    int operation;      // Codigo de operacion
} Peticion;

// Estructura de los parámetros de las funciones para files.c
typedef struct {
    int key;                            // 4 bytes
    char value1[MAX_VALUE1_LENGTH];     // 255 chars + '\0'
    int n_value2;                       // 4 bytes
    double V_value_2[MAX_N_VALUE2];     // 8 bytes
} Parametros;

#endif
