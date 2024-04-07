// ----------     LIBRERÍAS Y DEFINICIONES     ---------- //

#include "claves.h"
#include "messages.h"
#include "comms.c"

// Librerías básicas
#include <fcntl.h>      /* For O_* constants */
#include <sys/stat.h>   /* For mode constants */
#include <sys/types.h>  /* For mode constants */
#include <sys/socket.h> /* For socket, bind, listen, accept */
#include <arpa/inet.h>  /* For sockaddr_in */
#include <netinet/in.h> /* For sockaddr_in */
#include <string.h>     /* For strlen, strcpy, sprintf... */
#include <unistd.h>     /* For sleep */
#include <stdio.h>      /* For printf */
#include <stdlib.h>     /* For exit */

// Error en localhost -> en vez de 127.0.0.1 lo pone a 255.255.255.255
// Esto se usa para una comprobación a futuro, pero el programa usa la variable de entorno
#define localhost "127.0.0.1"

// ----------     FUNCIONES AUXILIARES    ---------- //

// Función para obtener la variable de entorno IP_TUPLAS
char* get_ip_tuplas() {
    printf("[comms][client] Obteniendo IP_TUPLAS\n");
    // Obtenemos la variable de entorno IP_TUPLAS
	char* ip_tuplas = getenv("IP_TUPLAS");
    // Si no se ha establecido la variable de entorno, se muestra un error
	if (ip_tuplas == NULL) {
		printf("No ha sido establecida IP_TUPLAS\n\n\n");
		exit(1);
	}
    printf("[comms][client] Obtenido IP_TUPLAS: %s\n", ip_tuplas);
	return ip_tuplas;
}

// Función para obtener la variable de entorno PORT_TUPLAS
int get_port_tuplas() {
    printf("[comms][client] Obteniendo PORT_TUPLAS\n");
    // Obtenemos la variable de entorno PORT_TUPLAS
	char* port_tuplas = getenv("PORT_TUPLAS");
	char *end;
    // Si no se ha establecido la variable de entorno, se muestra un error
	if (port_tuplas == NULL) {
		printf("No ha sido establecida PORT_TUPLAS\n\n\n");
		exit(1);
	}
    // Pasamos de string a int
	int port_tuplas_int = strtol(port_tuplas, &end, 10);
    // Comprobamos que el puerto sea válido
    if (port_tuplas_int < 1024 || port_tuplas_int > 65535) {
        printf("Puerto inválido: %d\n", port_tuplas_int);
        exit(1);
    }
    printf("[comms][client] Obtenido PORT_TUPLAS: %d\n", port_tuplas_int);
	return port_tuplas_int;
} 

// ----------    FUNCIONES SOCKETS    ---------- //
int socket_setup() {
    printf("[comms][client] Creando socket\n");
    // Creamos el socket
    int sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd == -1) {
		perror("Error creando el socket");
		return(-1);
	}

    // Configuramos la dirección del servidor
    printf("[comms][client] Configurando dirección del servidor\n");
	struct sockaddr_in server_addr;
	bzero((char *)&server_addr, sizeof(server_addr));
    char* ip_tuplas = get_ip_tuplas();
    // si no tenemos en el sistema localhost como 127.0.0.1 lo cambiamos con el valor del #define
	if(strcmp(ip_tuplas, "localhost") == 0)
		strcpy(ip_tuplas, localhost);
	int port_tuplas = get_port_tuplas();
	server_addr.sin_addr.s_addr = inet_addr(ip_tuplas);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port_tuplas);

    // Conectamos el socket al servidor
    printf("[comms][client] Conectando con el servidor\n");
	if (connect(sd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        // Si no se puede conectar, se muestra un error
		printf("Error conectando al servidor\n");
		printf("IP Tuplas: %s\n", ip_tuplas);
		printf("Port: %d\n", ntohs(server_addr.sin_port));
		printf("IP: %s\n", inet_ntoa(server_addr.sin_addr));
		exit(1);
	}
    printf("[comms][client] Conexión establecida con el servidor\n");
	return sd;
}

// ----------     FUNCIONES PRINCIPALES    ---------- //
int init() {
    // Creamos el socket y nos conectamos al servidor
    printf("[comms][client] Iniciando la operación init\n");
	int sd = socket_setup();
	// Enviamos la operación
    printf("[comms][client] Enviando la operación init al servidor\n");
	write_int_error (sd, 1);
	// Obtenemos la respuesta
    int* respuesta = malloc(sizeof(int));
	read_int_error(sd, respuesta);
    printf("[comms][client] Se ha obtenido respuesta del servidor\n");
	// Cerramos el socket
    printf("[comms][client] Cerrando el socket\n\n\n");
	close(sd);
    // Liberamos puntero
    int res = *respuesta;
    free(respuesta);
	// Devolvemos la respuesta del servidor
	return res;
}

int set_value(int key, char* value1, int N_value2, double* V_value_2) {
    printf("[comms][client] Iniciando la operación set_value\n");
    // Comprobamos que value1 no tenga más de 256 caracteres
    printf("[comms][client] Comprobando que value1 no tenga más de 256 caracteres\n");
    if (strlen(value1) > 256) {
        printf("El tamaño de value1 no es válido\n\n\n");
        return -1;
    }
	// Creamos el socket y nos conectamos al servidor
	int sd = socket_setup();
	// Enviamos la operación
    printf("[comms][client] Enviando la operación set_value al servidor\n");
	write_int_error(sd, 2);
	// Enviamos la información necesaria:
    printf("[comms][client] Enviando la información necesaria al servidor\n");
	write_int_error(sd, key);
	writeLine_error(sd, value1);
	write_int_error(sd, N_value2);
	write_double_vector_error(sd, V_value_2, N_value2);
	// Obtenemos la respuesta
    int* respuesta = malloc(sizeof(int));
	read_int_error(sd, respuesta);
    printf("[comms][client] Se ha obtenido respuesta del servidor\n");
	// Cerramos el socket
    printf("[comms][client] Cerrando el socket\n\n\n");
	close(sd);
	// Liberamos puntero
    int res = *respuesta;
    free(respuesta);
	// Devolvemos la respuesta del servidor
	return res;
}

int get_value(int key, char* value1, int* N_value2, double* V_value_2) {
    printf("[comms][client] Iniciando la operación get_value\n");
    // Creamos el socket y nos conectamos al servidor
    int sd = socket_setup();
    // Enviamos la operación
    printf("[comms][client] Enviando la operación get_value al servidor\n");
    write_int_error(sd, 3);
    // Enviamos la clave
    printf("[comms][client] Enviando la clave al servidor\n");
    write_int_error(sd, key);
    // Obtenemos la respuesta
    int* respuesta = malloc(sizeof(int));
    read_int_error(sd, respuesta);
    printf("[comms][client] Se ha obtenido respuesta del servidor\n");
    // Si la respuesta es 0, obtenemos los valores de la tupla
    if (*respuesta == 0) {
        printf("[comms][client] Obteniendo los valores de la tupla\n");
        readLine_error(sd, value1, MAX_VALUE1_LENGTH);
        read_int_error(sd, N_value2);
        read_double_vector_error(sd, V_value_2);
    }
    else {
        printf("[comms][client] No se ha podido obtener los valores de la tupla\n");
    }
    // Cerramos el socket
    printf("[comms][client] Cerrando el socket\n\n\n");
    close(sd);
    // Liberamos puntero
    int res = *respuesta;
    free(respuesta);
	// Devolvemos la respuesta del servidor
	return res;
}

int modify_value(int key, char* value1, int N_value2, double* V_value_2) {
    printf("[comms][client] Iniciando la operación modify_value\n");
    // Comprobamos que value1 no tenga más de 256 caracteres
    printf("[comms][client] Comprobando que value1 no tenga más de 256 caracteres\n");
    if (strlen(value1) > 256) {
        printf("El tamaño de value1 no es válido\n\n\n");
        return -1;
    }
    // Creamos el socket y nos conectamos al servidor
    int sd = socket_setup();
    // Enviamos la operación
    printf("[comms][client] Enviando la operación modify_value al servidor\n");
    write_int_error(sd, 4);
    // Enviamos la información necesaria:
    printf("[comms][client] Enviando la información necesaria al servidor\n");
    write_int_error(sd, key);
    writeLine_error(sd, value1);
    write_int_error(sd, N_value2);
    write_double_vector_error(sd, V_value_2, N_value2);
    // Obtenemos la respuesta
    int* respuesta = malloc(sizeof(int));
    read_int_error(sd, respuesta);
    printf("[comms][client] Se ha obtenido respuesta del servidor\n");
    // Cerramos el socket
    printf("[comms][client] Cerrando el socket\n\n\n");
    close(sd);
    // Liberamos puntero
    int res = *respuesta;
    free(respuesta);
	// Devolvemos la respuesta del servidor
	return res;
}

int delete_key(int key) {
    printf("[comms][client] Iniciando la operación delete_key\n");
    // Creamos el socket y nos conectamos al servidor
    int sd = socket_setup();
    // Enviamos la operación
    printf("[comms][client] Enviando la operación delete_key al servidor\n");
    write_int_error(sd, 5);
    // Enviamos la clave
    printf("[comms][client] Enviando la clave al servidor\n");
    write_int_error(sd, key);
    // Obtenemos la respuesta
    int* respuesta = malloc(sizeof(int));
    read_int_error(sd, respuesta);
    printf("[comms][client] Se ha obtenido respuesta del servidor\n");
    // Cerramos el socket
    printf("[comms][client] Cerrando el socket\n\n\n");
    close(sd);
    // Liberamos puntero
    int res = *respuesta;
    free(respuesta);
	// Devolvemos la respuesta del servidor
	return res;
}

int exist(int key) {
    printf("[comms][client] Iniciando la operación exist\n");
    // Creamos el socket y nos conectamos al servidor
    int sd = socket_setup();
    // Enviamos la operación
    printf("[comms][client] Enviando la operación exist al servidor\n");
    write_int_error(sd, 6);
    // Enviamos la clave
    printf("[comms][client] Enviando la clave al servidor\n");
    write_int_error(sd, key);
    // Obtenemos la respuesta
    int* respuesta = malloc(sizeof(int));
    read_int_error(sd, respuesta);
    printf("[comms][client] Se ha obtenido respuesta del servidor\n");
    // Cerramos el socket
    printf("[comms][client] Cerrando el socket\n\n\n");
    close(sd);
    // Liberamos puntero
    int res = *respuesta;
    free(respuesta);
	// Devolvemos la respuesta del servidor
	return res;
}

