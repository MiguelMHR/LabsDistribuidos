// ----------     LIBRERÍAS Y DEFINICIONES     ---------- //
#include <mqueue.h>
#ifndef MESSAGES_H
#define MESSAGES_H

// Definiciones básicas
#define MAX_STR_LENGTH 1024     // Longitud máxima de una cadena de caracteres
#define MAX_VALUE1_LENGTH 256   // Longitud máxima dada por el enunciado
#define MAX_N_VALUE2 32         // Longitud máxima dada por el enunciado
#define MQ_SERVER "/mq_server"  // Nombre de la message queue del servidor

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

// Estructura de la solicitud
typedef struct {
    // Tupla:
    int key;
    char value1[MAX_VALUE1_LENGTH];
    int N_value2;
    double V_value_2[MAX_N_VALUE2];
    // Operación
    int operation;
    char q_name[MAX_STR_LENGTH];
} request;

// Estructura de la respuesta
typedef struct {
    int return_value; // Error (-1) o éxito (0)
    // Tupla:
    char value1[MAX_VALUE1_LENGTH];
    int N_value2;
    double V_value_2[MAX_N_VALUE2];
} response;

// Estrcutura de la message queue de la solicitud
struct mq_attr requestAttributes = {
	.mq_flags = 0,				   // Flags (ignored for mq_open())
	.mq_maxmsg = 10,			   // Max. # of messages on queue
	.mq_msgsize = sizeof(request), // Max. message size (bytes)
	.mq_curmsgs = 0,			   // # of messages currently in queue
};

// Estructura de la message queue de la respuesta
struct mq_attr responseAttributes = {
	.mq_flags = 0,					// Flags (ignored for mq_open())
	.mq_maxmsg = 1,					// Max. # of messages on queue (only 1 response)
	.mq_msgsize = sizeof(response), // Max. message size (bytes)
	.mq_curmsgs = 0,				// # of messages currently in queue
};

#endif
