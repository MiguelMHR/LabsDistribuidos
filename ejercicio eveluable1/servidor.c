#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

#define MAX_MSG_SIZE 256
#define SERVER_QUEUE_NAME "/Cola"

void handle_request(char *request) {
    // Aquí se implementaría la lógica para manejar las solicitudes
    // Por ejemplo, interpretar el mensaje, realizar operaciones y enviar respuestas
    printf("Recibido: %s\n", request);
}

int main() {
    mqd_t server_queue;
    struct mq_attr attr;
    char buffer[MAX_MSG_SIZE + 1];
    ssize_t bytes_read;

    // Crear la cola de mensajes del servidor
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    server_queue = mq_open(SERVER_QUEUE_NAME, O_CREAT | O_RDONLY, 0644, &attr);
    if (server_queue == (mqd_t)-1) {
        perror("mq_open");
        exit(1);
    }

    printf("Servidor iniciado. Esperando solicitudes...\n");

    while (1) {
        // Recibir mensajes de la cola de mensajes del servidor
        bytes_read = mq_receive(server_queue, buffer, MAX_MSG_SIZE, NULL);
        if (bytes_read == -1) {
            perror("mq_receive");
            exit(1);
        }

        // Añadir terminador nulo
        buffer[bytes_read] = '\0';

        // Procesar la solicitud en un nuevo proceso
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(1);
        } else if (pid == 0) { // Proceso hijo
            handle_request(buffer);
            exit(0);
        }
    }

    // Cerrar la cola de mensajes del servidor
    if (mq_close(server_queue) == -1) {
        perror("mq_close");
        exit(1);
    }

    return 0;
}
