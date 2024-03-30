#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "claves.h" // Incluir las funciones definidas en claves.c

#define SERVER_PORT 7080
#define MAX_CLIENTS 10
#define MAX_MSG_SIZE 256

// Función para manejar la solicitud de un cliente
void handle_client_request(int client_socket, const char *request) {
    char response[MAX_MSG_SIZE];

    // Separar el comando y los argumentos del mensaje recibido
    char *token = strtok(request, "|");
    if (token == NULL) {
        perror("Comando no válido");
        return;
    }
    // Procesar la solicitud del cliente
    if (strcmp(token, "init") == 0) {
        // Llamar a la función init de claves.c
        int result = init();
        // Enviar la respuesta al cliente
        snprintf(response, MAX_MSG_SIZE, "%d", result);
        send(client_socket, response, strlen(response), 0);
    } else if (strcmp(token, "set") == 0) {
        // Llamar a la función set_value de claves.c con los argumentos adecuados
        // Obtener los argumentos
        char *key = strtok(NULL, "|");
        char *value1 = strtok(NULL, "|");
        int N_value2 = atoi(strtok(NULL, "|"));
        double *V_value2 = malloc(N_value2 * sizeof(double));
        for (int i = 0; i < N_value2; i++) {
            V_value2[i] = atof(strtok(NULL, "|"));
        }
        int result = set_value(atoi(key), value1, N_value2, V_value2);
        free(V_value2);
        // Enviar la respuesta al cliente
        snprintf(response, MAX_MSG_SIZE, "%d", result);
        send(client_socket, response, strlen(response), 0);
    } else if (strcmp(token, "get") == 0) {
        // Llamar a la función get_value de claves.c con el argumento adecuado
        // Obtener el argumento
        char *key = strtok(NULL, "|");
        char value1[MAX_VALUE1];
        int N_value2;
        double *V_value2 = malloc(MAX_VALUE2 * sizeof(double));
        int result = get_value(atoi(key), value1, &N_value2, V_value2);
        // Enviar la respuesta al cliente
        if (result == 0) {
            snprintf(response, MAX_MSG_SIZE, "%s|%d", value1, N_value2);
            send(client_socket, response, strlen(response), 0);
            for (int i = 0; i < N_value2; i++) {
                snprintf(response, MAX_MSG_SIZE, "%f", V_value2[i]);
                send(client_socket, response, strlen(response), 0);
            }
        } else {
            snprintf(response, MAX_MSG_SIZE, "%d", result);
            send(client_socket, response, strlen(response), 0);
        }
        free(V_value2);
    } else if (strcmp(token, "delete") == 0) {
        // Llamar a la función delete_key de claves.c con el argumento adecuado
        // Obtener el argumento
        char *key = strtok(NULL, "|");
        int result = delete_key(atoi(key));
        // Enviar la respuesta al cliente
        snprintf(response, MAX_MSG_SIZE, "%d", result);
        send(client_socket, response, strlen(response), 0);
    } else if (strcmp(token, "modify") == 0) {
        // Llamar a la función modify_value de claves.c con los argumentos adecuados
        // Obtener los argumentos
        char *key = strtok(NULL, "|");
        char *value1 = strtok(NULL, "|");
        int N_value2 = atoi(strtok(NULL, "|"));
        double *V_value2 = malloc(N_value2 * sizeof(double));
        for (int i = 0; i < N_value2; i++) {
            V_value2[i] = atof(strtok(NULL, "|"));
        }
        int result = modify_value(atoi(key), value1, N_value2, V_value2);
        free(V_value2);
        // Enviar la respuesta al cliente
        snprintf(response, MAX_MSG_SIZE, "%d", result);
        send(client_socket, response, strlen(response), 0);
    } else if (strcmp(token, "exist") == 0) {
        // Llamar a la función exist de claves.c con el argumento adecuado
        // Obtener el argumento
        char *key = strtok(NULL, "|");
        int result = exist(atoi(key));
        // Enviar la respuesta al cliente
        snprintf(response, MAX_MSG_SIZE, "%d", result);
        send(client_socket, response, strlen(response), 0);
    } else {
        perror("Comando no reconocido");
    }
}


int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    pthread_t tid[MAX_CLIENTS];
    int i = 0;

    // Crear el socket del servidor
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Error al crear el socket del servidor");
        exit(EXIT_FAILURE);
    }

    // Configurar la dirección del servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);

    // Enlazar el socket del servidor a la dirección y puerto especificados
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error al enlazar el socket del servidor");
        exit(EXIT_FAILURE);
    }

    // Escuchar conexiones entrantes
    if (listen(server_socket, MAX_CLIENTS) < 0) {
        perror("Error al escuchar conexiones entrantes");
        exit(EXIT_FAILURE);
    }

    printf("Servidor iniciado. Esperando conexiones de clientes...\n");

    // Aceptar conexiones de clientes y manejarlas en hilos separados
    while (1) {
        // Aceptar la conexión de un cliente
        if ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len)) < 0) {
            perror("Error al aceptar la conexión del cliente");
            continue;
        }

        // Crear un hilo para manejar la solicitud del cliente
        if (pthread_create(&tid[i++], NULL, handle_client, (void *)&client_socket) != 0) {
            perror("Error al crear el hilo para manejar la solicitud del cliente");
            close(client_socket);
            continue;
        }

        // Limpiar el array de hilos si es necesario
        if (i >= MAX_CLIENTS) {
            i = 0;
            while (tid[i] != 0) {
                if (pthread_join(tid[i++], NULL) != 0) {
                    perror("Error al limpiar el array de hilos");
                    break;
                }
            }
            i = 0;
        }
    }

    // Cerrar el socket del servidor (este código nunca se ejecutará)
    close(server_socket);

    return 0;
}
