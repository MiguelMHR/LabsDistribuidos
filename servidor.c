// ----------     LIBRERÍAS Y DEFINICIONES     ---------- //

#include "files.c"
#include "messages.h"

// Librerías Básicas
#include <pthread.h>
#include <mqueue.h>
#include <fcntl.h>
#include <semaphore.h>

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

// Función para inicializar el semáforo de forma segura
void init_sem() {
    pthread_mutex_lock(&mutex_lectura);
    if (sem_iniciado == 0) {
        sem_init(&escritor, 0, 1);
        sem_iniciado = 1;
    }
    pthread_mutex_unlock(&mutex_lectura);
}

// ----------     FUNCION PRINCIPAL    ---------- //

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
		O_CREAT | O_WRONLY,	  // Open flags (O_WRONLY for sender)
		S_IRUSR | S_IWUSR,	  // User read/write permission
		&responseAttributes); // response queue attributes on "claves.h"
    mq_send(return_queue, (char *) &res, sizeof(response), 0);
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
