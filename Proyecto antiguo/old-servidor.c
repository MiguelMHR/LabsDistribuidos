// ----------     LIBRERÍAS Y DEFINICIONES     ---------- //

// Librerías Básicas
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <pthread.h>
#include <mqueue.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>

// Límites del path, value1 y N_value2
#define MAX_STR_LENGTH 1024
#define MAX_VALUE1_LENGTH 256
#define MAX_N_VALUE2 32
#define MQ_SERVER "/mq_server"

// ----------     ESTRUCTURAS     ---------- //

// Para poder comunicarse, se debe establecer un código de operación
// para saber qué debe ejecutar el servidor. Los códigos de operación son:
//     1: init()
//     2: set_value()
//     3: get_value()
//     4: modify_value()
//     5: delete_key()
//     6: exists()
// La variable operation guardará la opción elegida por el cliente

typedef struct {
    // Tupla:
    int key;
    char value1[MAX_VALUE1_LENGTH];
    int N_value2;
    double V_value_2[MAX_N_VALUE2];
    // Operación y nombre
    int operation;
    char q_name[MAX_STR_LENGTH]; // Nombre de la cola de respuesta
} request;

typedef struct {
    int return_value; // Error (-1) o éxito (0)
    // Tupla:
    char value1[MAX_VALUE1_LENGTH];
    int N_value2;
    double V_value_2[MAX_N_VALUE2];
} response;

// Estrcutura de la message queue de la solicitud
struct mq_attr requestAttributes = {
	.mq_flags = 0,				   // Flags (ignored for mq_open())
	.mq_maxmsg = 10,			   // Max. # of messages on queue
	.mq_msgsize = sizeof(request), // Max. message size (bytes)
	.mq_curmsgs = 0,			   // # of messages currently in queue
};

// Estructura de la message queue de la respuesta
struct mq_attr responseAttributes = {
	.mq_flags = 0,					// Flags (ignored for mq_open())
	.mq_maxmsg = 1,					// Max. # of messages on queue (only 1 response)
	.mq_msgsize = sizeof(response), // Max. message size (bytes)
	.mq_curmsgs = 0,				// # of messages currently in queue
};

// ----------     VARIABLES DE CONTROL     ---------- //
pthread_mutex_t mutex_comunicacion = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_lectura = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int closed = 0;     // Variable para controlar la ejecución del hilo
int failed_sem = 0; // Variable para controlar la ejecución del semáforo
sem_t escritor;
int sem_iniciado = 0; // Variable para controlar la inicialización del semáforo
int n_lectores = 0; // Variable para controlar el número de lectores

// ----------     FUNCIONES AUXILIARES     ---------- //

// Función para limpiar el directorio de mensajes
int clean_directory(const char *dirname) {
    DIR *dir;
    struct dirent *entry;
    char path[MAX_STR_LENGTH];

    dir = opendir(dirname);
    if (dir == NULL) {
        printf("Error al abrir el directorio");
        return -1;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {
            snprintf(path, sizeof(path), "%s/%s", dirname, entry->d_name);
            // Se elimina el archivo del directorio
            remove(path);
        }
    }
    closedir(dir);
    return 0;
}

// Función para inicializar el semáforo de forma segura
void init_sem() {
    pthread_mutex_lock(&mutex_lectura);
    if (sem_iniciado == 0) {
        sem_init(&escritor, 0, 1);
        sem_iniciado = 1;
    }
    pthread_mutex_unlock(&mutex_lectura);
}

// ----------     FUNCIONES PRINCIPALES     ---------- //

// Función para comprobar la existencia del mensaje
int exists(int key) {
    FILE *file;
    char path[MAX_STR_LENGTH];
    // Combinamos el path con la key para obtener el nombre del archivo
    sprintf(path, "./mensajes/%d.txt", key);
    // Abrimos el archivo en modo lectura para comprobar si existe
    file = fopen(path, "r");
    // Si no existe, retornamos 0
    if (file==NULL) {
        return 0;
    }
    // Si existe, retornamos 1 y cerramos archivo
    fclose(file);
    return 1;
}

// Función para reiniciar el directorio de mensajes
int init(){
    const char *dirname = "./mensajes";

    // Si no está creado, lo creamos
    if (fopen(dirname, "r") == NULL) {
        // Si no se pudo crear, retornamos -1
        if (mkdir(dirname, 0777) == -1) {
            printf("No se pudo crear el directorio de mensajes");
            return -1;
        }
    } 
    // Si ya existe, lo limpiamos
    else {    
        if (clean_directory(dirname) == -1) {
            printf("No se pudo limpiar el directorio de mensajes");
            return -1;
        }
    }
    return 0;
}

// Función para almacenar la tupla
int set_value(int key, char* value1, int N_value2, double* V_value_2) {
    if (exists(key)) {
        printf("La clave ya existe\n");
        return -1;
    }
    else if (N_value2 <= 1 || N_value2 >= 32) {
        printf("El tamaño del vector no es válido\n");
        return -1;
    }
    else {
        FILE *file;
        char path[MAX_STR_LENGTH];
        // Combinamos el path con la key para obtener el nombre del archivo
        sprintf(path, "./mensajes/%d.txt", key);
        // Abrimos el archivo en modo escritura para almacenar la tupla
        file = fopen(path, "w");
        // Si no se pudo abrir, retornamos -1
        if (!file) {
            printf("No se pudo abrir el archivo");
            return -1;
        }
        // Escribimos la tupla en el archivo
        fprintf(file, "%s-", value1);
        fprintf(file, "%d-", N_value2);
        fprintf(file, "[");
        for (int i = 0; i < N_value2; i++) {
            fprintf(file, "%lf,", V_value_2[i]);
        }
        // Para eliminar la última coma
        fseek(file, -1, SEEK_END);  // Mueve el puntero al final del archivo, luego retrocede un carácter
        int fd = fileno(file);      // Obtiene el descriptor de archivo
        off_t pos = ftell(file);    // Obtiene la posición actual del puntero del archivo
        ftruncate(fd, pos);         // Trunca el archivo en la posición actual

        fprintf(file, "]");
        // Cerramos el archivo
        fclose(file);
        return 0;

    }
}

// Función para obtener la tupla
int get_value(int key, char* value1, int* N_value2, double* V_value_2) {
    if (!exists(key)) {
        printf("La clave no existe\n");
        return -1;
    }
    else {
        FILE *file;
        char path[MAX_STR_LENGTH];
        // Combinamos el path con la key para obtener el nombre del archivo
        sprintf(path, "./mensajes/%d.txt", key);
        // Abrimos el archivo en modo lectura para leer la tupla
        file = fopen(path, "r");
        // Si no se pudo abrir, retornamos -1
        if (!file) {
            printf("No se pudo abrir el archivo");
            return -1;
        }
        // Leemos la tupla del archivo
        fscanf(file, "%[^-]-%d-[", value1, N_value2);
        for (int i = 0; i < *N_value2; i++) {
            fscanf(file, "%lf,", &V_value_2[i]);
        }
        // Cerramos el archivo
        fclose(file);
        return 0;
    }
}

// Función para eliminar la tupla
int delete_key(int key) {
    if (!exists(key)) {
        printf("La clave no existe\n");
        return -1;
    }
    else {
        char path[MAX_STR_LENGTH];
        // Combinamos el path con la key para obtener el nombre del archivo
        sprintf(path, "./mensajes/%d.txt", key);
        // Eliminamos el archivo
        remove(path);
        return 0;
    }
}

// Función para modificar la tupla
int modify_value(int key, char* value1, int N_value2, double* V_value_2) {
    if (!exists(key)) {
        printf("La clave no existe\n");
        return -1;
    }
    else if (N_value2 <= 1 || N_value2 >= 32) {
        printf("El tamaño del vector no es válido\n");
        return -1;
    }
    else {
        FILE *file;
        char path[MAX_STR_LENGTH];
        // Combinamos el path con la key para obtener el nombre del archivo
        sprintf(path, "./mensajes/%d.txt", key);
        // Abrimos el archivo en modo escritura para modificar la tupla
        file = fopen(path, "w");
        // Si no se pudo abrir, retornamos -1
        if (!file) {
            printf("No se pudo abrir el archivo");
            return -1;
        }
        // Escribimos la tupla en el archivo
        fprintf(file, "%s-", value1);
        fprintf(file, "%d-", N_value2);
        fprintf(file, "[");
        for (int i = 0; i < N_value2; i++) {
            fprintf(file, "%lf,", V_value_2[i]);
        }
        // Para eliminar la última coma
        fseek(file, -1, SEEK_END);  // Mueve el puntero al final del archivo, luego retrocede un carácter
        int fd = fileno(file);      // Obtiene el descriptor de archivo
        off_t pos = ftell(file);    // Obtiene la posición actual del puntero del archivo
        ftruncate(fd, pos);         // Trunca el archivo en la posición actual

        fprintf(file, "]");
        // Cerramos el archivo
        fclose(file);
        return 0;
    }
}

// ----------     FUNCIONES COLAS     ---------- //

// Función para ejecutar la operación pedida y enviar la respuesta al cliente
void procesar_peticion(request* req) {
    // Se crea una copia de la petición para no modificar la request original
    request req_copy = *req;
    // Para evitar una ejecución múltiple, se cambia la condición a cerrado
    pthread_mutex_lock(&mutex_comunicacion);
    closed = 0;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex_comunicacion);

    // Se ejecuta la operación pedida
    response res;
    switch (req_copy.operation) {
        // Todas las operaciones deben ser controladas para evitar condiciones de carrera
        case 1:
            init_sem();
            sem_wait(&escritor);
            res.return_value = init();
            sem_post(&escritor);
            break;
        case 2:
            init_sem();
            sem_wait(&escritor);
            res.return_value = set_value(req_copy.key, req_copy.value1, req_copy.N_value2, req_copy.V_value_2);
            sem_post(&escritor);
            break;
        case 3:
            // Creamos variables para guardar los valores de la tupla procesada
            char value1cpy[MAX_VALUE1_LENGTH] = "";
            int* N_value2cpy = malloc(sizeof(int));
            double* V_value_2cpy = calloc(MAX_N_VALUE2, sizeof(double));
            *N_value2cpy = 0;

            // Ejecutamos la operación de manera concurrente
            init_sem();
            pthread_mutex_lock(&mutex_lectura);
            n_lectores++;
            if (n_lectores == 1) {
               if (sem_trywait(&escritor) != 0) {
                    pthread_mutex_unlock(&mutex_lectura);
                    sem_post(&escritor);
                    res.return_value = -1;
                    // Guardamos los valores obtenidos
                    strcpy(res.value1, value1cpy);
                    res.N_value2 = *N_value2cpy;
                    for (int i = 0; i < *N_value2cpy; i++) {
                        res.V_value_2[i] = V_value_2cpy[i];
                    }
                    free(V_value_2cpy);
                    free(N_value2cpy);
                    break;
                }
            }
            pthread_mutex_unlock(&mutex_lectura);
            res.return_value = get_value(req_copy.key, value1cpy, N_value2cpy, V_value_2cpy);
            pthread_mutex_lock(&mutex_lectura);
            n_lectores--;
            if (n_lectores == 0) {
                sem_post(&escritor);
            }
            pthread_mutex_unlock(&mutex_lectura);

            // Guardamos los valores obtenidos
            strcpy(res.value1, value1cpy);
            res.N_value2 = *N_value2cpy;
            for (int i = 0; i < *N_value2cpy; i++) {
                res.V_value_2[i] = V_value_2cpy[i];
            }
            free(V_value_2cpy);
            free(N_value2cpy);
            break;
        case 4:
            init_sem();
            sem_wait(&escritor);
            res.return_value = modify_value(req_copy.key, req_copy.value1, req_copy.N_value2, req_copy.V_value_2);
            sem_post(&escritor);
            break;
        case 5:
            init_sem();
            sem_wait(&escritor);
            res.return_value = delete_key(req_copy.key);
            sem_post(&escritor);
            break;
        case 6:
            init_sem();
            pthread_mutex_lock(&mutex_lectura);
            n_lectores++;
            if (n_lectores == 1) {
               if (sem_trywait(&escritor) != 0) {
                    pthread_mutex_unlock(&mutex_lectura);
                    sem_post(&escritor);
                    res.return_value = -1;
                    break;
               }
            }
            pthread_mutex_unlock(&mutex_lectura);
            res.return_value = exists(req_copy.key);
            pthread_mutex_lock(&mutex_lectura);
            n_lectores--;
            if (n_lectores == 0) {
                sem_post(&escritor);
            }
            pthread_mutex_unlock(&mutex_lectura);
            break;
        default:
            res.return_value = -1;
            break;
    }
    // Enviamos la respuesta al cliente
    mqd_t return_queue = mq_open(
		req_copy.q_name,	  // Queue name
		O_CREAT | O_RDONLY,	  // Open flags (O_WRONLY for sender)
		S_IRUSR | S_IWUSR,	  // User read/write permission
		&responseAttributes); // response queue attributes on "claves.h"
    mq_send(return_queue, (char *) &res, sizeof(response), 0);
    printf("nombre servidor: %s\n", req_copy.q_name);
    mq_close(return_queue);
}

// ----------     FUNCIÓN MAIN     ---------- //
int main(){
    // ! Thread attributes
    pthread_attr_t attr;                                         // Thread attributes
    pthread_attr_init(&attr);                                    // Initialize the attribute
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED); // Set the attribute to detached

    // ! Mutex & Condition variables
    pthread_mutex_init(&mutex_comunicacion, NULL); // Initialize the mutex
    pthread_cond_init(&cond, NULL);   // Initialize the condition variable

    // of messages sent/set of messages received and so we dont have to force break the loop
    while (1)
    {
        // * Open the queue
        mqd_t cliente = mq_open(
            MQ_SERVER,           // Queue name
            O_CREAT | O_RDONLY,  // Open flags (O_RDONLY for receiver)
            S_IRUSR | S_IWUSR,   // User read/write permission
            &requestAttributes); // Assign queue attributes

        // * Request (message)
        request req;

        // * Receive the message
        mq_receive(cliente, (char *)&req, sizeof(request), NULL);                   
        // ! We create a thread for each request and execute the function deal_with_request
        pthread_t thread; // create threads to handle the requests as they come in

        pthread_create(&thread, &attr, (void *)procesar_peticion, (void *)&req);

        // ! Wait for the thread to finish (child copies the descriptor)
        pthread_mutex_lock(&mutex_comunicacion); // Lock the mutex
        while (closed == 1)
        {
            pthread_cond_wait(&cond, &mutex_comunicacion); // Wait for the condition variable
        }
        closed = 0;                  // Set the thread as busy
        pthread_mutex_unlock(&mutex_comunicacion); // Unlock the mutex
    }
    return 0;
}
