//
// Created by kai on 03/03/2024.
//
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "lines.h"
#define MAX_LINE 	256

int main(int argc, char *argv[]){
        int sd, sc;
	int val;
	int err;
        socklen_t size;

        struct sockaddr_in server_addr,  client_addr;
        char texto[MAX_LINE], rec[MAX_LINE];

        sd =  socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (sd < 0) {
                perror("Error in socket");
                exit(1);
        }

        val = 1;
        err = setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *) &val, sizeof(int));
        if (err < 0) {
                perror("Error in opction");
                exit(1);
        }

         err = bind(sd, (const struct sockaddr *)&server_addr, sizeof(server_addr));
	if (err == -1) {
		printf("Error en bind\n");
		return -1;
	}

    	err = listen(sd, SOMAXCONN);
	if (err == -1) {
		printf("Error en listen\n");
		return -1;
	}

        size = sizeof(client_addr);

        while(1){
                printf("esperando conexion\n");
                sc = accept(sd, (struct sockaddr *)&client_addr, (socklen_t *)&size);
                if (sc == -1) {
                        printf("Error en accept\n");
                        return -1;
                }
                while (1){
                        bzero((char *)&server_addr, sizeof(server_addr));
                        server_addr.sin_family      = AF_INET;
                        server_addr.sin_addr.s_addr = INADDR_ANY;
                        server_addr.sin_port        = htons(4200);
                        printf("conexión aceptada de IP: %s   Puerto: %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

                        err = readLine ( sc, texto, MAX_LINE);   // recibe la operació
                        if (err == -1) {
                                printf("Error en recepcion\n");
                                close(sc);
                                continue;
                        }
                        err = sendMessage( sc, rec, strlen(texto)); // recibe a
                        if (err == -1) {
                                printf("Error en recepcion\n");
                                close(sc);
                                continue;
                        }
                        close(sc);                      // cierra la conexión (sc)
                }
        }
 

	// Complete .....

	
        close (sd);
        return(0);
}
