//
// Created by kai on 03/03/2024.
//
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include "lines.h"


#define MAX_LINE 	256

int main(int argc, char *argv[])
{

        int sd;
        struct sockaddr_in server_addr;
        struct hostent *hp;
        int err;

        if (argc != 3) {
                printf("Usage: client <serverAddress> <serverPort>\n");
                exit(0);
        }

	// the socket is create
        sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (sd < 0){
                perror("Error in socket");
                exit(1);
        }

	// Obtain Server address 
        bzero((char *)&server_addr, sizeof(server_addr));
        hp = gethostbyname (argv[1]);
        if (hp == NULL) {
                perror("Error en gethostbyname");
                exit(1);
        }

	memcpy (&(server_addr.sin_addr), hp->h_addr, hp->h_length);
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(atoi(argv[2]));

	// Complete.....
	// First: establish the connection
        char texto[MAX_LINE], rec[MAX_LINE];
        scanf("%s", texto);

        err = connect(sd, (struct sockaddr *) &server_addr,  sizeof(server_addr));
	if (err == -1) {
		printf("Error en connect\n");
		return -1;
	}
        while(1){
                err = readLine(sd, texto, MAX_LINE);  // envía la operacion
	        if (err == -1){
		        printf("Error en envio\n");
		        return -1;
	        }

    	        err = sendMessage(sd, texto, strlen(texto) + 1);  // envía la operacion
	        if (err == -1){
		        printf("Error en envio\n");
		        return -1;
	        }

    	        err = readLine(sd, rec, MAX_LINE);     // recibe la respuesta
	        if (err == -1){
		        printf("Error en recepcion\n");
		        return -1;
	        }
        }

	
    	printf("Resultado es %s \n", rec);

        close (sd);

        return(0);
}
