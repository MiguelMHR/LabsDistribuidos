#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include "claves.h" // Incluir las funciones definidas en claves.c

#define MAX_CLIENTS 5 // Número máximo de hilos (clientes) que pueden ejecutarse simultáneamente
#define MAX_MSG_SIZE 256

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // Mutex para proteger el acceso a las claves
pthread_cond_t cond_cp = PTHREAD_COND_INITIALIZER; // Condition variable para esperar hasta que se haya copiado

int is_copied = 0; // Variable que indica si se ha copiado

// Función para escribir todos los datos al socket
int write_all(int client_socket, char *buffer, size_t total) {
    size_t escritos = 0;
    ssize_t result = 0;

    while (escritos != total) {
        result = write(client_socket, buffer + escritos, total - escritos);
        if (-1 == result) {
            return -1;
        }
        escritos += result;
    }
    return escritos;
}

// Función para leer todos los datos del socket
int read_all(int client_socket, char *buffer, size_t total) {
    size_t leidos = 0;
    ssize_t result = 0;

    while (leidos != total) {
        result = read(client_socket, buffer + leidos, total - leidos);
        if (-1 == result) {
            return -1;
        }
        if (result == 0) {
            break;
        }
        leidos += result;
    }
    return leidos;
}

// Función para manejar la solicitud de un cliente
void *handle_client(void *arg) {
    int client_socket = *((int *)arg);
    char request[MAX_MSG_SIZE];

    // Leer la solicitud del cliente
    if (read_all(client_socket, request, MAX_MSG_SIZE) < 0) {
        perror("Error al leer la solicitud del cliente");
        close(client_socket);
        pthread_exit(NULL);
    }

    pthread_mutex_lock(&mutex) ;
    is_copied = 1 ;
    pthread_cond_signal(&copied) ;
    pthread_mutex_unlock(&mutex) ;

    close(client_socket);
    pthread_exit(NULL);
}

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
        if (write_all(client_socket, response, strlen(response)) < 0) {
            perror("Error al enviar la respuesta al cliente");
        }
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
        if (write_all(client_socket, response, strlen(response)) < 0) {
            perror("Error al enviar la respuesta al cliente");
        }
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
            if (write_all(client_socket, response, strlen(response)) < 0) {
                perror("Error al enviar la respuesta al cliente");
            }
            for (int i = 0; i < N_value2; i++) {
                snprintf(response, MAX_MSG_SIZE, "%f", V_value2[i]);
                if (write_all(client_socket, response, strlen(response)) < 0) {
                    perror("Error al enviar la respuesta al cliente");
                }
            }
        } else {
            snprintf(response, MAX_MSG_SIZE, "%d", result);
            if (write_all(client_socket, response, strlen(response)) < 0) {
                perror("Error al enviar la respuesta al cliente");
            }
        }
        free(V_value2);
    } else if (strcmp(token, "delete") == 0) {
        // Llamar a la función delete_key de claves.c con el argumento adecuado
        // Obtener el argumento
        char *key = strtok(NULL, "|");
        int result = delete_key(atoi(key));
        // Enviar la respuesta al cliente
        snprintf(response, MAX_MSG_SIZE, "%d", result);
        if (write_all(client_socket, response, strlen(response)) < 0) {
            perror("Error al enviar la respuesta al cliente");
        }
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
        if (write_all(client_socket, response, strlen(response)) < 0) {
            perror("Error al enviar la respuesta al cliente");
        }
    } else if (strcmp(token, "exist") == 0) {
        // Llamar a la función exist de claves.c con el argumento adecuado
        // Obtener el argumento
        char *key = strtok(NULL, "|");
        int result = exist(atoi(key));
        // Enviar la respuesta al cliente
        snprintf(response, MAX_MSG_SIZE, "%d", result);
        if (write_all(client_socket, response, strlen(response)) < 0) {
            perror("Error al enviar la respuesta al cliente");
        }
    } else {
        perror("Comando no reconocido");
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <puerto>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);

    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    pthread_t tid[MAX_CLIENTS];
    int i = 0;

    // Inicializar mutex y condición
    if (pthread_mutex_init(&mutex, NULL) != 0) {
        perror("Error al inicializar el mutex");
        exit(EXIT_FAILURE);
    }
    if (pthread_cond_init(&cond_cp, NULL) != 0) {
        perror("Error al inicializar la variable de condición");
        exit(EXIT_FAILURE);
    }
    // Crear el socket del servidor
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Error al crear el socket del servidor");
        exit(EXIT_FAILURE);
    }

    // Configurar la dirección del servidor
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

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

    printf("Servidor iniciado en el puerto %d. Esperando conexiones de clientes...\n", port);

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

        // wait data is copied
        pthread_mutex_lock(&mutex) ;
        while (!is_copied) {
            pthread_cond_wait(&copied, &mutex) ;
        }
        is_copied = 0 ;
        pthread_mutex_unlock(&mutex) ;

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
    
    // Destruir mutex y condición
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_cp);

    return 0;
}
