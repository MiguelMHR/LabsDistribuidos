#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int write_all ( int newsd, char *buffer, size_t total )
{
    size_t escritos = 0 ;
    ssize_t result  = 0 ;

    while (escritos != total) // mientras queda por escribir...
    {
       result = write(newsd, buffer+escritos, total-escritos) ;
       // puede que write NO escriba todo lo solicitado de una vez
       if (-1 == result) {
           return -1 ;
       }

       escritos = escritos + result ;
    }

    return escritos ;
}

int main ( int argc, char **argv )
{
    int sd, newsd, ret;
    socklen_t size;
    struct sockaddr_in server_addr, client_addr;

    if (argc != 2) {
        printf("Uso: %s <puerto>\n", argv[0]);
        return 0;
    }

    int puerto = atoi(argv[1]) ;

    // (1) creación de un socket
    // * NO tiene dirección asignada aquí
    sd = socket(AF_INET, SOCK_STREAM, 0) ;
    if (sd < 0) {
        perror("Error en la creación del socket: ");
        return -1;
    }

    // (2) obtener la dirección
    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(puerto);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // (3) asigna dirección a un socket
    // * host = INADDR_ANY -> cualquier dirección del host
    // * port = 0 -> el sistema selecciona el primer puerto libre
    // * port = 1...1023 -> puertos reservados (puede necesitar ser root la ejecución)
    ret = bind(sd,(struct sockaddr *)& server_addr, sizeof(server_addr)) ;
    if (ret < 0) {
        perror("Error en bind: ") ;
        return -1 ;
    }

    // (4) preparar para aceptar conexiones
    // * listen permite definir el número máximo de peticiones pendientes a encolar
    // * SOMAXCONN está en sys/socket.h
    ret = listen(sd, SOMAXCONN);
    if (ret < 0) {
        perror("Error en listen: ") ;
        return -1 ;
    }

    while (1)
    {
       // (5) aceptar nueva conexión (newsd) desde socket servidor (sd)
       // * bloquea al servidor hasta que se produce la conexión
       // * sd permite acceptar conexiones y newsd permitirá trabajar con cliente
       size = sizeof(struct sockaddr_in) ;
       newsd = accept (sd, (struct sockaddr *) &client_addr, &size);
       if (newsd < 0) {
           perror("Error en el accept");
           return -1 ;
       }

       // Para ayudar a la depuración,
       // se imprime la IP y puerto del cliente que se conecta
       // client_addr almacena la IP y el puerto del proceso cliente
       printf("conexión aceptada de IP:%s y puerto:%d\n",
               inet_ntoa(client_addr.sin_addr),
                   ntohs(client_addr.sin_port));

       // Preparar el mensaje a enviar: 1024 bytes con "hola mundo"
       char buffer[1024] ;
       strcpy(buffer, "Hola mundo") ;

       // (6) transferir datos sobre newsd
       size_t escritos = write_all(newsd, buffer, sizeof(buffer)) ;
       if (escritos < 0) {
           printf("Error al escribir buffer\n") ;
       }

       // (7) cerrar socket conectado
       close(newsd);
    }

    // (8) cerrar socket de servicio
    close(sd);

} /* fin main */