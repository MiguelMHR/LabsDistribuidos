// ----------     LIBRERÍAS Y DEFINICIONES     ---------- //

#include "claves.h"
#include "messages.h"

// Librerías básicas
#include <fcntl.h>      /* For O_* constants */
#include <sys/stat.h>   /* For mode constants */
#include <sys/types.h>  /* For mode constants */
#include <mqueue.h>     /* For message queue */
#include <string.h>     /* For strlen, strcpy, sprintf... */
#include <unistd.h>     /* For sleep */
#include <pthread.h>    /* For threads */
#include <stdio.h>      /* For printf */
#include <stdlib.h>     /* For exit */

// Definiciones básicas
#define NUM_THREADS 1

// Cola del cliente (servidor-cliente)
char cq_name[MAX_STR_LENGTH];

// ----------     FUNCIONES COLAS     ---------- //

// Función para abrir las colas de mensajes (cliente y servidor)
int open_queues(mqd_t* serverQueue, mqd_t* clientQueue) {
	// Abrimos la cola cliente-servidor (o solo servidor)
    printf("[comms][client] Abriendo colas\n");
	*serverQueue = mq_open(
		MQ_SERVER,			 // Queue name
		O_CREAT | O_WRONLY,	 // Open flags (O_WRONLY for sender)
		S_IRUSR | S_IWUSR,	 // User read/write permission
		&requestAttributes); // request queue attributes on "claves.h"

	if (*serverQueue == -1){
		perror("No se pudo abrir la cola servidor-cliente: ");
		return -1; // return -1 if the queue was not created
    }
    // Nombre de la cola servidor-cliente (o solo cliente)
    sprintf(cq_name, "/mq_client_%d", getpid());
    printf("[comms][client] Nombre de la cola servidor-cliente: %s\n", cq_name);
    // Abrimos la cola del cliente
	*clientQueue = mq_open(
		cq_name,		      // Queue name
		O_CREAT | O_RDONLY,	  // Open flags (O_WRONLY for sender)
		S_IRUSR | S_IWUSR,	  // User read/write permission
		&responseAttributes); // response queue attributes on "claves.h"

	if (*clientQueue == -1){
		perror("No se pudo abrir la cola del cliente: ");
		return -1; // return -1 if the queue was not created 
    }
	return 0;
}

// ----------     FUNCIONES PRINCIPALES    ---------- //
int init() {
    // Inicializar las colas de mensajes
    mqd_t server_queue;
    mqd_t client_queue;
    open_queues(&server_queue, &client_queue);

    // Crear la petición para el servidor
    request req;
    req.operation = 1;
    strcpy(req.q_name, cq_name);
    // Enviamos la petición al servidor
    printf("[comms][client] Enviando petición init al servidor\n");
    int send_request = mq_send(server_queue, (char *) &req, sizeof(request), 0);
    if (send_request == -1) {
        perror("Error al enviar la petición al servidor: ");
        return -1;
    }

    // Recibir la respuesta del servidor
    printf("[comms][client] Recibiendo respuesta init del servidor\n");
    response res;
    int receive_response = mq_receive(client_queue, (char *)&res, sizeof(response), 0);
    if (receive_response == -1) {
        perror("Error al recibir la respuesta del servidor: ");
        return -1;
    }

    // Cerramos la cola del cliente
    printf("[comms][client] Cerramos y borramos colas\n");
    if (mq_close(client_queue) == -1) {
        perror("Error al cerrar la cola servidor-cliente: ");
        return -1;
    }
    // Borramos la cola del cliente
    if (mq_unlink(cq_name) == -1) {
        perror("Error al borrar la cola servidor-cliente: ");
        return -1;
    }
    if (mq_close(server_queue) == -1) {
        perror("Error al cerrar la cola cliente-servidor: ");
        return -1;
    }
    return res.return_value;
}

int set_value(int key, char* value1, int N_value2, double* V_value_2) {
    // Inicializar las colas de mensajes
    mqd_t server_queue;
    mqd_t client_queue;
    open_queues(&server_queue, &client_queue);

    if (N_value2 <= 1 || N_value2 >= 32) {
        printf("El tamaño del vector no es válido\n");
        return -1;
    }

    // Crear la petición para el servidor
    request req;
    req.key = key;
    req.operation = 2;
    strcpy(req.q_name, cq_name);
    req.N_value2 = N_value2;
    strcpy(req.value1, value1);
    for (int i = 0; i < N_value2; i++) {
        req.V_value_2[i] = V_value_2[i];
    }
    // Enviamos la petición al servidor
    printf("[comms][client] Enviando petición set_value al servidor\n");
    int send_request = mq_send(server_queue, (char *) &req, sizeof(request), 0);
    if (send_request == -1) {
        perror("Error al enviar la petición al servidor: ");
        return -1;
    }

    // Recibir la respuesta del servidor
    response res;
    int receive_response = mq_receive(client_queue, (char *) &res, sizeof(response), NULL);
    printf("[comms][client] Recibiendo respuesta set_value del servidor\n");
    if (receive_response == -1) {
        perror("Error al recibir la respuesta del servidor: ");
        return -1;
    }

    // Cerramos la cola del cliente
    printf("[comms][client] Cerrando y borrando colas\n");
    if (mq_close(client_queue) == -1) {
        perror("Error al cerrar la cola servidor-cliente: ");
        return -1;
    }
    // Borramos la cola del cliente
    if (mq_unlink(cq_name) == -1) {
        perror("Error al borrar la cola servidor-cliente: ");
        return -1;
    }
    if (mq_close(server_queue) == -1) {
        perror("Error al cerrar la cola cliente-servidor: ");
        return -1;
    }
    return res.return_value;
}

int get_value(int key, char* value1, int* N_value2, double* V_value_2) {
    // Inicializar las colas de mensajes
    mqd_t server_queue;
    mqd_t client_queue;
    open_queues(&server_queue, &client_queue);

    if (*N_value2 <= 1 || *N_value2 >= 32) {
        printf("El tamaño del vector no es válido\n");
        return -1;
    }

    // Crear la petición para el servidor
    request req;
    req.key = key;
    req.operation = 3;
    strcpy(req.q_name, cq_name);
    req.N_value2 = *N_value2;
    strcpy(req.value1, value1);
    for (int i = 0; i < *N_value2; i++) {
        req.V_value_2[i] = V_value_2[i];
    }
    // Enviamos la petición al servidor
    printf("[comms][client] Enviando petición get_value al servidor\n");
    int send_request = mq_send(server_queue, (char *) &req, sizeof(request), 0);
    if (send_request == -1) {
        printf("Error al enviar la petición al servidor");
        return -1;
    }

    // Recibir la respuesta del servidor
    response res;
    printf("[comms][client] Recibiendo respuesta get_value del servidor\n");
    int receive_response = mq_receive(client_queue, (char *) &res, sizeof(response), 0);
    if (receive_response == -1) {
        printf("Error al recibir la respuesta del servidor");
        return -1;
    }

    // Modificamos los valores con los recibidos:
    strcpy(value1, res.value1);
    for (int i = 0; i < *N_value2; i++) {
        V_value_2[i] = res.V_value_2[i];
    }
    *N_value2 = res.N_value2;

    // Cerramos la cola del cliente
    printf("[comms][client] Cerrando y borrando colas\n");
    if (mq_close(client_queue) == -1) {
        perror("Error al cerrar la cola servidor-cliente: ");
        return -1;
    }
    // Borramos la cola del cliente
    if (mq_unlink(cq_name) == -1) {
        perror("Error al borrar la cola servidor-cliente: ");
        return -1;
    }
    if (mq_close(server_queue) == -1) {
        perror("Error al cerrar la cola cliente-servidor: ");
        return -1;
    }
    return res.return_value;
}

int modify_value(int key, char* value1, int N_value2, double* V_value_2) {
    // Inicializar las colas de mensajes
    mqd_t server_queue;
    mqd_t client_queue;
    open_queues(&server_queue, &client_queue);

    if (N_value2 <= 1 || N_value2 >= 32) {
        printf("El tamaño del vector no es válido\n");
        return -1;
    }

    // Crear la petición para el servidor
    request req;
    req.key = key;
    req.operation = 4;
    strcpy(req.q_name, cq_name);
    req.N_value2 = N_value2;
    strcpy(req.value1, value1);
    for (int i = 0; i < N_value2; i++) {
        req.V_value_2[i] = V_value_2[i];
    }
    // Enviamos la petición al servidor
    printf("[comms][client] Enviando petición modify_value al servidor\n");
    int send_request = mq_send(server_queue, (char *) &req, sizeof(request), 0);
    if (send_request == -1) {
        printf("Error al enviar la petición al servidor");
        return -1;
    }

    // Recibir la respuesta del servidor
    response res;
    printf("[comms][client] Recibiendo respuesta modify_value del servidor\n");
    int receive_response = mq_receive(client_queue, (char *) &res, sizeof(response), 0);
    if (receive_response == -1) {
        printf("Error al recibir la respuesta del servidor");
        return -1;
    }

    // Modificamos los valores con los recibidos:
    strcpy(value1, res.value1);
    for (int i = 0; i < N_value2; i++) {
        V_value_2[i] = res.V_value_2[i];
    }
    N_value2 = res.N_value2;

    // Cerramos la cola del cliente
    printf("[comms][client] Cerrando y borrando colas\n");
    if (mq_close(client_queue) == -1) {
        perror("Error al cerrar la cola servidor-cliente: ");
        return -1;
    }
    // Borramos la cola del cliente
    if (mq_unlink(cq_name) == -1) {
        perror("Error al borrar la cola servidor-cliente: ");
        return -1;
    }
    if (mq_close(server_queue) == -1) {
        perror("Error al cerrar la cola cliente-servidor: ");
        return -1;
    }
    return res.return_value;
}

int delete_key(int key) {
    // Inicializar las colas de mensajes
    mqd_t server_queue;
    mqd_t client_queue;
    open_queues(&server_queue, &client_queue);

    // Crear la petición para el servidor
    request req;
    req.key = key;
    req.operation = 5;
    strcpy(req.q_name, cq_name);
    // Enviamos la petición al servidor
    printf("[comms][client] Enviando petición delete_key al servidor\n");
    int send_request = mq_send(server_queue, (char *) &req, sizeof(request), 0);
    if (send_request == -1) {
        printf("Error al enviar la petición al servidor");
        return -1;
    }

    // Recibir la respuesta del servidor
    response res;
    printf("[comms][client] Recibiendo respuesta delete_key del servidor\n");
    int receive_response = mq_receive(client_queue, (char *) &res, sizeof(response), 0);
    if (receive_response == -1) {
        printf("Error al recibir la respuesta del servidor");
        return -1;
    }

    // Cerramos la cola del cliente
    printf("[comms][client] Cerrando y borrando colas\n");
    if (mq_close(client_queue) == -1) {
        perror("Error al cerrar la cola servidor-cliente: ");
        return -1;
    }
    // Borramos la cola del cliente
    if (mq_unlink(cq_name) == -1) {
        perror("Error al borrar la cola servidor-cliente: ");
        return -1;
    }
    if (mq_close(server_queue) == -1) {
        perror("Error al cerrar la cola cliente-servidor: ");
        return -1;
    }
    return res.return_value;
}

int exist(int key) {
    // Inicializar las colas de mensajes
    mqd_t server_queue;
    mqd_t client_queue;
    open_queues(&server_queue, &client_queue);

    // Crear la petición para el servidor
    request req;
    req.key = key;
    req.operation = 6;
    strcpy(req.q_name, cq_name);
    // Enviamos la petición al servidor
    printf("[comms][client] Enviando petición exist al servidor\n");
    int send_request = mq_send(server_queue, (char *) &req, sizeof(request), 0);
    if (send_request == -1) {
        printf("Error al enviar la petición al servidor");
        return -1;
    }

    // Recibir la respuesta del servidor
    response res;
    printf("[comms][client] Recibiendo respuesta exist del servidor\n");
    int receive_response = mq_receive(client_queue, (char *) &res, sizeof(response), 0);
    if (receive_response == -1) {
        printf("Error al recibir la respuesta del servidor");
        return -1;
    }

    // Cerramos la cola del cliente
    printf("[comms][client] Cerrando y borrando colas\n");
    if (mq_close(client_queue) == -1) {
        perror("Error al cerrar la cola servidor-cliente: ");
        return -1;
    }
    // Borramos la cola del cliente
    if (mq_unlink(cq_name) == -1) {
        perror("Error al borrar la cola servidor-cliente: ");
        return -1;
    }
    if (mq_close(server_queue) == -1) {
        perror("Error al cerrar la cola cliente-servidor: ");
        return -1;
    }
    return res.return_value;
}

