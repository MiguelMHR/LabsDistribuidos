#include "tuplas.h"
#include "files.c"

//    -----  VARIABLES DE CONTROL  -----    //
pthread_mutex_t mutex_datos = PTHREAD_MUTEX_INITIALIZER;
// Este mutex controlará la ejecución de las funciones de manera atómica

bool_t
init_tuplas_1_svc(int *result, struct svc_req *rqstp)
{
	bool_t retval;
	retval = TRUE;
	*result = init();
	return retval;
}

bool_t
set_value_tuplas_1_svc(message msg, int *result,  struct svc_req *rqstp)
{
	bool_t retval;
	retval = TRUE;
	// Variables del set_value
	int key = msg.key_or_return;
	char value1[256];
	int n_value2 = msg.n_value2;
	double v_value2[32];
	// Copiar los valores de msg a las variables locales
	strcpy(value1, msg.value1);
	for (int i = 0; i < n_value2; i++) {
		v_value2[i] = msg.v_value2[i];
	}
	// Iniciar la sección crítica
	pthread_mutex_lock(&mutex_datos);
	// Llamar a la función set_value
	*result = set_value(key, value1, n_value2, v_value2);
	// Finalizar la sección crítica
	pthread_mutex_unlock(&mutex_datos);
	// Retornar el resultado
	return retval;
}

bool_t
get_value_tuplas_1_svc(int key, message *result,  struct svc_req *rqstp)
{
	bool_t retval;
	retval = TRUE;
	// Variables del get_value
	char value1[256];
	int n_value2;
	double v_value2[32];
	// Iniciar la sección crítica
	pthread_mutex_lock(&mutex_datos);
	// Llamar a la función get_value
	result->key_or_return = get_value(key, value1, &n_value2, v_value2);
	// Finalizar la sección crítica
	pthread_mutex_unlock(&mutex_datos);
	// Copiar los valores a result
	strcpy(result->value1, value1);
	result->n_value2 = n_value2;
	for (int i = 0; i < n_value2; i++) {
		result->v_value2[i] = v_value2[i];
	}
	// Retornar el resultado
	return retval;
}

bool_t
modify_value_tuplas_1_svc(message msg, int *result,  struct svc_req *rqstp)
{
	bool_t retval;
	retval = TRUE;
	// Variables del modify_value
	int key = msg.key_or_return;
	char value1[256];
	int n_value2 = msg.n_value2;
	double v_value2[32];
	// Copiar los valores de msg a las variables locales
	strcpy(value1, msg.value1);
	for (int i = 0; i < n_value2; i++) {
		v_value2[i] = msg.v_value2[i];
	}
	// Iniciar la sección crítica
	pthread_mutex_lock(&mutex_datos);
	// Llamar a la función modify_value
	*result = modify_value(key, value1, n_value2, v_value2);
	// Finalizar la sección crítica
	pthread_mutex_unlock(&mutex_datos);
	// Retornar el resultado
	return retval;
}

bool_t
delete_key_tuplas_1_svc(int key, int *result,  struct svc_req *rqstp)
{
	bool_t retval;
	retval = TRUE;
	// Iniciar la sección crítica
	pthread_mutex_lock(&mutex_datos);
	// Llamar a la función delete_key
	*result = delete_key(key);
	// Finalizar la sección crítica
	pthread_mutex_unlock(&mutex_datos);
	// Retornar el resultado
	return retval;
}

bool_t
exist_tuplas_1_svc(int key, int *result,  struct svc_req *rqstp)
{
	bool_t retval;
	retval = TRUE;
	// Iniciar la sección crítica
	pthread_mutex_lock(&mutex_datos);
	// Llamar a la función exist
	*result = exist(key);
	// Finalizar la sección crítica
	pthread_mutex_unlock(&mutex_datos);
	// Retornar el resultado
	return retval;
}

int
tuplas_1_freeresult (SVCXPRT *transp, xdrproc_t xdr_result, caddr_t result)
{
	xdr_free (xdr_result, result);
	return 1;
}
