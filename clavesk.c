#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

void error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int main() {
    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;

    // Obtener el valor de la variable de entorno IP_TUPLAS
    char *ip_tuplas = getenv("IP_TUPLAS");
    if (ip_tuplas == NULL) {
        printf("Variable de entorno IP_TUPLAS no definida\n");
        exit(EXIT_FAILURE);
    }

    // Obtener el valor de la variable de entorno PORT_TUPLAS
    char *port_tuplas = getenv("PORT_TUPLAS");
    if (port_tuplas == NULL) {
        printf("Variable de entorno PORT_TUPLAS no definida\n");
        exit(EXIT_FAILURE);
    }

    // Convertir el puerto a entero
    int port = atoi(port_tuplas);

    // Crear descriptor de archivo del socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        error("Fallo al crear el socket");
    }

    memset(&servaddr, 0, sizeof(servaddr));

    // Asignar IP, PUERTO
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(ip_tuplas); // Convertir IP a formato adecuado
    servaddr.sin_port = htons(port);

    // Vincular el socket recién creado a la IP y verificación
    if ((bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))) != 0) {
        error("Fallo al vincular el socket");
    }

    // Ahora el servidor está listo para escuchar y verificación
    if ((listen(sockfd, 5)) != 0) {
        error("Fallo al escuchar");
    }

    // Aceptar el paquete de datos del cliente y verificación
    while (1) {
        socklen_t len = sizeof(cli);
        if ((connfd = accept(sockfd, (struct sockaddr *)&cli, &len)) < 0) {
            error("Fallo al aceptar la conexión del servidor");
        }
        // Recibir datos del cliente
        int key, operation, N_value2;
        char q_name[50], value1[50];
        double V_value_2[32];

        // Recibir clave
        if (recv(connfd, &key, sizeof(int), 0) == -1) {
            error("Fallo al recibir la clave");
        }

        // Recibir operación
        if (recv(connfd, &operation, sizeof(int), 0) == -1) {
            error("Fallo al recibir la operación");
        }

        // Recibir nombre de cola
        if (recv(connfd, q_name, sizeof(q_name), 0) == -1) {
            error("Fallo al recibir el nombre de cola");
        }

        // Recibir valor1
        if (recv(connfd, value1, sizeof(value1), 0) == -1) {
            error("Fallo al recibir valor1");
        }

        // Recibir N_value2
        if (recv(connfd, &N_value2, sizeof(int), 0) == -1) {
            error("Fallo al recibir N_value2");
        }

        // Recibir V_value_2
        if (recv(connfd, V_value_2, N_value2, 0) == -1) {
            error("Fallo al recibir V_value_2");
        }

        // Procesar datos recibidos

        // Enviar respuesta al cliente
        int return_value = 0; // Valor de retorno de ejemplo
        if (send(connfd, &return_value, sizeof(int), 0) == -1) {
            error("Fallo al enviar la respuesta");
        }

        // Cerrar la conexión
        close(connfd);
    }

    // Cerrar el socket de escucha
    close(sockfd);

    return 0;
}
