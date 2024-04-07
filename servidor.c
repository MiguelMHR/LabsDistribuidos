// ----------     LIBRERÍAS Y DEFINICIONES     ---------- //

#include "files.c"
#include "comms.c"
#include "messages.h"

// Librerías Básicas
#include <pthread.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

// ----------     VARIABLES DE CONTROL     ---------- //
pthread_mutex_t mutex_comunicacion = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_datos = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int busy = 0;     // Variable para controlar la ejecución del hilo

// ----------     FUNCION PRINCIPAL    ---------- //

// Función para ejecutar la operación pedida y enviar la respuesta al cliente
void procesar_peticion(Peticion* pet) {
    printf("[comms][server] Procesando petición\n");
    // Variables iniciales
    int operation_copy, client_sd;
    // Creamos variables para guardar los valores de la tupla procesada
    char value1cpy[MAX_VALUE1_LENGTH] = "";
    int* N_value2cpy = malloc(sizeof(int));
    double* V_value_2cpy = calloc(MAX_N_VALUE2, sizeof(double));
    
    // Bloqueamos el mutex a la hora de copiar la peticion al hilo
    pthread_mutex_lock(&mutex_comunicacion);
    printf("[comms][server] Copiando petición al hilo de manera atómica\n");
    operation_copy = pet->operation;
    client_sd = pet->socket;
    busy = 0;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex_comunicacion);
    // Se ejecuta la operación pedida protegida por un mutex
    pthread_mutex_lock(&mutex_datos);
    printf("[comms][server] Ejecutando operación de manera atómica\n");
    printf("[comms][server] Operacion elegida: %d\n", operation_copy);
    Parametros parametros;
    int error = 0;
    switch (operation_copy) {
        // Todas las operaciones deben ser controladas para evitar condiciones de carrera
        case 1:
            // ---- INIT ---- //
            printf("[comms][server] Ejecutando init\n");
            init();
            break;
        case 2:
            // ---- SET_VALUE ---- //
            // Leemos los parámetros de la petición
            printf("[comms][server] Leyendo parámetros de la petición\n");
            read_int_error(client_sd, &parametros.key);
            readLine_error(client_sd, parametros.value1, MAX_VALUE1_LENGTH);
            read_int_error(client_sd, &parametros.n_value2);
            read_double_vector_error(client_sd, parametros.V_value_2);
            // ejecutamos la operación
            printf("[comms][server] Ejecutando set_value\n");
            error = set_value(parametros.key, parametros.value1, parametros.n_value2, parametros.V_value_2);
            break;
        case 3:
            // ---- GET_VALUE ---- //
            // Leemos la key de la petición
            printf("[comms][server] Leyendo parámetros de la petición\n");
            read_int_error(client_sd, &parametros.key);
            // Ejecutamos la operación
            printf("[comms][server] Ejecutando get_value\n");
            error = get_value(parametros.key, value1cpy, N_value2cpy, V_value_2cpy);    
            break;
        case 4:
            // ---- MODIFY_VALUE ---- //
            // Leemos los parámetros de la petición
            printf("[comms][server] Leyendo parámetros de la petición\n");
            read_int_error(client_sd, &parametros.key);
            readLine_error(client_sd, parametros.value1, MAX_VALUE1_LENGTH);
            read_int_error(client_sd, &parametros.n_value2);
            read_double_vector_error(client_sd, parametros.V_value_2);
            // Ejecutamos la operación
            printf("[comms][server] Ejecutando modify_value\n");
            error = modify_value(parametros.key, parametros.value1, parametros.n_value2, parametros.V_value_2);
            break;
        case 5:
            // ---- DELETE_KEY ---- //
            // Leemos la key de la petición
            printf("[comms][server] Leyendo parámetros de la petición\n");
            read_int_error(client_sd, &parametros.key);
            // Ejecutamos la operación
            printf("[comms][server] Ejecutando delete_key\n");
            error = delete_key(parametros.key);
            break;
        case 6:
            // ---- EXISTS ---- //
            printf("[comms][server] Leyendo parámetros de la petición\n");
            // Leemos la key de la petición
            read_int_error(client_sd, &parametros.key);
            // Ejecutamos la operación
            printf("[comms][server] Ejecutando exists\n");
            error = exists(parametros.key);
            break;
    }
    // Liberamos el mutex para que otro hilo pueda ejecutar una operación
    pthread_mutex_unlock(&mutex_datos);
    
    // Enviamos la respuesta al cliente
    printf("[comms][server] Enviando respuesta al cliente\n");
    // Enviamos el resultado de la operación
    write_int_error(client_sd, error);
    // Si la operación es get_value y ha sido exitosa, enviamos los valores de la tupla
    if (operation_copy == 3 && error == 0) {
        // Enviamos value1
        writeLine_error(client_sd, value1cpy);
        // Enviamos N_value2
        write_int_error(client_sd, *N_value2cpy);
        // Enviamos V_value_2
        write_double_vector_error(client_sd, V_value_2cpy, *N_value2cpy);
    }
    printf("[comms][server] Respuesta enviada\n");
    printf("[comms][server] Cerrando socket cliente y liberando variables\n\n");
    close(client_sd);
    free(N_value2cpy);
    free(V_value_2cpy);
    return;
};

// ----------     FUNCIÓN MAIN     ---------- //
int main(int argc, char** argv){
    printf("[comms][server] Iniciando servidor (thread, mutex y socket)\n\n");
    // ! Atributos de los hilos
    pthread_attr_t attr;                                         // Atributos del hilo
    pthread_attr_init(&attr);                                    // Inicializamos los atributos
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED); // Configuramos el hilo como detached (se limpia automáticamente)

    // ! Inicialización del mutex y la condición
    pthread_mutex_init(&mutex_comunicacion, NULL); // Inicializamos el mutex
    pthread_cond_init(&cond, NULL);                // Inicializamos la condición

    // ! Implementación del socket
    int sd, newsd, ret;
    socklen_t size;
    struct sockaddr_in server_addr, client_addr;

    // Comprobamos si se han pasado bien los argumentos 
    if (argc != 2) {
        printf("Uso: %s <puerto>\n", argv[0]);
        return 0;
    }
    // Convertimos el puerto a entero y comprobamos si es válido
    int puerto = atoi(argv[1]);
    if (puerto < 1024 || puerto > 65535) {
        printf("Puerto inválido: %d\n", puerto);
        return(-1);
    }

    // (1) creación de un socket
    // * NO tiene dirección asignada aquí
    printf("[comms][server] Creando socket\n");
    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0) {
        perror("Error en la creación del socket");
        return -1;
    }

    // (2) obtener la dirección
    printf("[comms][server] Obteniendo dirección\n");
    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(puerto);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // (3) asigna dirección a un socket
    // * host = INADDR_ANY -> cualquier dirección del host
    // * port = 0 -> el sistema selecciona el primer puerto libre
    // * port = 1...1023 -> puertos reservados (puede necesitar ser root la ejecución)
    printf("[comms][server] Asignando dirección al socket\n");
    ret = bind(sd,(struct sockaddr *)& server_addr, sizeof(server_addr)) ;
    if (ret < 0) {
        perror("Error en bind");
        return -1;
    }

    // (4) preparar para aceptar conexiones
    // * listen permite definir el número máximo de peticiones pendientes a encolar
    // * SOMAXCONN está en sys/socket.h
    printf("[comms][server] Preparando para aceptar conexiones\n");
    ret = listen(sd, SOMAXCONN);
    if (ret < 0) {
        perror("Error en listen");
        return -1;
    }
    printf("[comms][server] Servidor iniciado\n\n");

    while (1) {
        // (5) aceptar nueva conexión (newsd) desde socket servidor (sd)
        // * bloquea al servidor hasta que se produce la conexión
        // * sd permite acceptar conexiones y newsd permitirá trabajar con cliente
        printf("[comms][server] Esperando petición del cliente\n");
        size = sizeof(struct sockaddr_in);
        newsd = accept (sd, (struct sockaddr *) &client_addr, &size);
        if (newsd < 0) {
           perror("Error en el accept");
           return -1;
        }

        // Para ayudar a la depuración,
        // se imprime la IP y puerto del cliente que se conecta
        // client_addr almacena la IP y el puerto del proceso cliente
        printf("[comms][server] Conexión aceptada de IP: %s y puerto: %d\n",
                inet_ntoa(client_addr.sin_addr),
                ntohs(client_addr.sin_port));

        
        // ! Recibimos la petición del cliente
        Peticion client_request;
        int operation_code;
        printf("[comms][server] Leyendo petición del cliente\n");
        ssize_t bytes_read = read_int(newsd, &operation_code);
        if (bytes_read == -1) {
            perror("Error leyendo la petición");
            return(-1);
        }

        // ! Creamos la petición
        client_request.socket = newsd;
        client_request.operation = operation_code;
        
        // ! Creamos un hilo para procesar la petición
        pthread_t thread; // create threads to handle the requests as they come in
        printf("[comms][server] Creando hilo para procesar la petición\n");
        pthread_create(&thread, &attr, (void *)procesar_peticion, (void *)&client_request);
        printf("[comms][server] La petición ha sido procesada correctamente\n");
        
        // Esperando a que termine el thread
        pthread_mutex_lock(&mutex_comunicacion);
        printf("[comms][server] Esperando a que termine el hilo\n");
        while (busy == 1) {
            pthread_cond_wait(&cond, &mutex_comunicacion);  
        }
        busy = 1;                                           
        pthread_mutex_unlock(&mutex_comunicacion);          
        printf("[comms][server] Hilo terminado\n\n");
    }
    // ! Destruimos los mutex, la condición y cerramos el socket (no debería llegar aquí)
    pthread_mutex_destroy(&mutex_comunicacion);
    pthread_mutex_destroy(&mutex_datos);
    pthread_cond_destroy(&cond);
    close(sd);

    return 0;
}
