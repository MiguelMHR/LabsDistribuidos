#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define SERVER_PORT 12345
#define MAX_MSG_SIZE 256

// Función para enviar una solicitud al servidor y recibir la respuesta
int send_request(const char *request, char *response) {
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[MAX_MSG_SIZE];

    // Crear el socket del cliente
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Error al crear el socket del cliente");
        return -1;
    }

    // Configurar la dirección del servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    // Conectar al servidor
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error al conectar con el servidor");
        return -1;
    }

    // Enviar la solicitud al servidor
    if (send(client_socket, request, strlen(request), 0) < 0) {
        perror("Error al enviar la solicitud al servidor");
        return -1;
    }

    // Recibir la respuesta del servidor
    if (recv(client_socket, buffer, MAX_MSG_SIZE, 0) < 0) {
        perror("Error al recibir la respuesta del servidor");
        return -1;
    }

    // Copiar la respuesta recibida al buffer de respuesta
    strcpy(response, buffer);

    // Cerrar el socket del cliente
    close(client_socket);

    return 0;
}

int main() {
    char request[MAX_MSG_SIZE];
    char response[MAX_MSG_SIZE];

    // Ejemplo de solicitud: obtener el valor asociado a una clave
    snprintf(request, MAX_MSG_SIZE, "get|clave");

    // Enviar la solicitud al servidor y recibir la respuesta
    if (send_request(request, response) < 0) {
        fprintf(stderr, "Error al enviar la solicitud o recibir la respuesta del servidor\n");
        exit(EXIT_FAILURE);
    }

    // Procesar la respuesta recibida del servidor
    printf("Respuesta del servidor: %s\n", response);

    return 0;
}
