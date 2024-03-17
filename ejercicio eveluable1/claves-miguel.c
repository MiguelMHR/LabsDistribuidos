#include "claves.h"

// ----------     FUNCIONES COLAS     ---------- //

// Función para abrir las colas de mensajes (cliente y servidor)
int open_queues(mqd_t* serverQueue, mqd_t* clientQueue) {
	// Abrimos la cola del servidor
	*serverQueue = mq_open(
		MQ_SERVER,			 // Queue name
		O_CREAT | O_WRONLY,	 // Open flags (O_WRONLY for sender)
		S_IRUSR | S_IWUSR,	 // User read/write permission
		&requestAttributes); // request queue attributes on "claves.h"

	if (*serverQueue == -1){
		printf("No se pudo abrir la cola del servidor");
		return -1; // return -1 if the queue was not created
	}
    // Nombre de la cola del servidor
    char client_file[MAX_STR_LENGTH];
    sprintf(client_file, "/mq_client_%ld", pthread_self());

	// Abrimos la cola del cliente
	*clientQueue = mq_open(
		client_file,		      // Queue name
		O_CREAT | O_RDONLY,	  // Open flags (O_WRONLY for sender)
		S_IRUSR | S_IWUSR,	  // User read/write permission
		&responseAttributes); // response queue attributes on "claves.h"

	if (*clientQueue == -1){
		printf("No se pudo abrir la cola del cliente");
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
    sprintf(req.q_name, "/mq_client_%ld", pthread_self());
    // Enviamos la petición al servidor
    int send_request = mq_send(server_queue, (char *) &req, sizeof(request), 0);
    if (send_request == -1) {
        printf("Error al enviar la petición al servidor");
        return -1;
    }

    // Recibir la respuesta del servidor
    response res;
    int receive_response = mq_receive(client_queue, (char *) &res, sizeof(response), 0);
    if (receive_response == -1) {
        printf("Error al recibir la respuesta del servidor");
        return -1;
    }

    // Cerramos la cola del cliente
    if (mq_close(client_queue) == -1) {
        printf("Error al cerrar la cola del cliente");
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
    req.N_value2 = N_value2;
    strcpy(req.value1, value1);
    for (int i = 0; i < N_value2; i++) {
        req.V_value_2[i] = V_value_2[i];
    }
    sprintf(req.q_name, "/mq_client_%ld", pthread_self());
    // Enviamos la petición al servidor
    int send_request = mq_send(server_queue, (char *) &req, sizeof(request), 0);
    if (send_request == -1) {
        printf("Error al enviar la petición al servidor");
        return -1;
    }

    // Recibir la respuesta del servidor
    response res;
    int receive_response = mq_receive(client_queue, (char *) &res, sizeof(response), 0);
    if (receive_response == -1) {
        printf("Error al recibir la respuesta del servidor");
        return -1;
    }

    // Cerramos la cola del cliente
    if (mq_close(client_queue) == -1) {
        printf("Error al cerrar la cola del cliente");
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
    req.N_value2 = *N_value2;
    strcpy(req.value1, value1);
    for (int i = 0; i < *N_value2; i++) {
        req.V_value_2[i] = V_value_2[i];
    }
    sprintf(req.q_name, "/mq_client_%ld", pthread_self());
    // Enviamos la petición al servidor
    int send_request = mq_send(server_queue, (char *) &req, sizeof(request), 0);
    if (send_request == -1) {
        printf("Error al enviar la petición al servidor");
        return -1;
    }

    // Recibir la respuesta del servidor
    response res;
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
    if (mq_close(client_queue) == -1) {
        printf("Error al cerrar la cola del cliente");
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
    req.N_value2 = N_value2;
    strcpy(req.value1, value1);
    for (int i = 0; i < N_value2; i++) {
        req.V_value_2[i] = V_value_2[i];
    }
    sprintf(req.q_name, "/mq_client_%ld", pthread_self());
    // Enviamos la petición al servidor
    int send_request = mq_send(server_queue, (char *) &req, sizeof(request), 0);
    if (send_request == -1) {
        printf("Error al enviar la petición al servidor");
        return -1;
    }

    // Recibir la respuesta del servidor
    response res;
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
    if (mq_close(client_queue) == -1) {
        printf("Error al cerrar la cola del cliente");
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
    sprintf(req.q_name, "/mq_client_%ld", pthread_self());
    // Enviamos la petición al servidor
    int send_request = mq_send(server_queue, (char *) &req, sizeof(request), 0);
    if (send_request == -1) {
        printf("Error al enviar la petición al servidor");
        return -1;
    }

    // Recibir la respuesta del servidor
    response res;
    int receive_response = mq_receive(client_queue, (char *) &res, sizeof(response), 0);
    if (receive_response == -1) {
        printf("Error al recibir la respuesta del servidor");
        return -1;
    }

    // Cerramos la cola del cliente
    if (mq_close(client_queue) == -1) {
        printf("Error al cerrar la cola del cliente");
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
    sprintf(req.q_name, "/mq_client_%ld", pthread_self());
    // Enviamos la petición al servidor
    int send_request = mq_send(server_queue, (char *) &req, sizeof(request), 0);
    if (send_request == -1) {
        printf("Error al enviar la petición al servidor");
        return -1;
    }

    // Recibir la respuesta del servidor
    response res;
    int receive_response = mq_receive(client_queue, (char *) &res, sizeof(response), 0);
    if (receive_response == -1) {
        printf("Error al recibir la respuesta del servidor");
        return -1;
    }

    // Cerramos la cola del cliente
    if (mq_close(client_queue) == -1) {
        printf("Error al cerrar la cola del cliente");
        return -1;
    }
    return res.return_value;
}

