#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "messages.h"

// Lectura cadena de caracteres:
 ssize_t readLine ( int fd, void *buffer, size_t n )
 {
     ssize_t numRead;  /* num of bytes fetched by last read() */
     size_t totRead;   /* total bytes read so far */
     char *buf;
     char ch;

     if (n <= 0 || buffer == NULL) {
         errno = EINVAL;
         return -1;
     }

     buf = buffer;
     totRead = 0;
     while (1)
     {
         numRead = read(fd, &ch, 1);     /* read a byte */

         if (numRead == -1) {
              if (errno == EINTR)        /* interrupted -> restart read() */
                  continue;
         else return -1;                 /* some other error */
         } else if (numRead == 0) {      /* EOF */
                 if (totRead == 0)       /* no bytes read; return 0 */
                      return 0;
                 else break;
         } else {                        /* numRead must be 1 if we get here*/
                 if (ch == '\n') break;
                 if (ch == '\0') break;
                 if (totRead < n - 1) {   /* discard > (n-1) bytes */
                      totRead++;
                     *buf++ = ch;
                 }
         }
     }

     *buf = '\0';
     return totRead;
 }

// Funciones de escritura de cadena de caracteres (la segunda es la importante para strings)
int sendMessage ( int socket, char * buffer, int len )
{
    int r;
    int l = len;

    do
    {
         r = write(socket, buffer, l);
         if (r < 0) {
             return (-1);   /* fail */
         }
         l = l - r;
         buffer = buffer + r;

    } while ((l>0) && (r>=0));
    return 0;
}

int writeLine ( int socket, char *buffer )
{
    return sendMessage(socket, buffer, strlen(buffer)+1);
}

// Ejemplo de uso de las funciones de escritura de cadena de caracteres:

// char buffer[256] ;
// strcpy(buffer, "Cadena a enviar...") ;
// writeLine(sd, buffer) ;        // se envía hasta el fin de cadena (incluido)
// sendMessage(sd, buffer, 256) ; // se envía 256 caracteres (no todos se usan)

// Funcion de lectura de integer (siendo éste una cadena de caracteres)

int read_int ( int socket, int *number )
{
   char buffer[MAX_ARRAY_LENGTH];
   char *endptr ;

   readLine(socket, buffer, MAX_ARRAY_LENGTH) ;
   (*number) = strtol(buffer, &endptr, 10) ; // n = atoi(buffer) ;
   if (endptr[0] != '\0') {
       printf("Error: %s no es un número en base %d\n", buffer, 10) ;
       return -1 ;
   }

   return 0 ;
}

// Funcion de escritura de integer (siendo transformado a una cadena de caracteres)
int write_int ( int socket, int number )
{
    char buffer[MAX_ARRAY_LENGTH];

    sprintf(buffer, "%d", number);
    sendMessage(socket, buffer, strlen(buffer)+1);
    return 0 ;
}

// Funcion que convierte un array de doubles a string
int doubles_to_str(double* arr, int arrSize, char* str) {
    // Calcula la longitud total necesitada por la cadena
    int totalLength = 0;
    for (int i = 0; i < arrSize; i++) {
        totalLength += snprintf(NULL, 0, "%lf", arr[i]);
    }
	// Añadimos las comas de cada número menos el último
	totalLength += arrSize-1;

    // Convertimos cada double en una cadena y los concatenamos mediante "-"
    int offset = 0;
    for (int i = 0; i < arrSize; i++) {
        offset += snprintf(str + offset, totalLength - offset, "%lf,", arr[i]);           
    }
    // Eliminamos la última coma
    str[strlen(str)-1] = '\0';
    return 0;
}

// Funcion que convierte una string de doubles a un vector de doubles 
// siendo la cadena de caracteres double de la forma "%lf,%lf,%lf...":
int str_to_double(char* str, double* array){
    // Calculamos el número de elementos
    int n = 0;
    for (size_t i = 0; i < strlen(str); i++) {
        if (str[i] == ',') {
            n++;
        }
    }
    n++; // Añadimos el último elemento
    // Convertimos la cadena a un array de doubles
    char* token = strtok(str, ",");
    int i = 0;
    while (token != NULL) {
        array[i] = atof(token);
        token = strtok(NULL, ",");
        i++;
    }
	return 0;
}


// Funcion de esritura de vector de doubles (siendo éste una cadena de caracteres):
int write_double_vector (int socket, double *vector, int arrSize) {	
	// Convertimos el array de doubles a string para writeLine
	char cast_str[MAX_ARRAY_LENGTH];
	doubles_to_str(vector, arrSize, cast_str);
	return writeLine(socket, cast_str);
}

int read_double_vector (int socket, double* vector) {
	// Leemos la cadena de caracteres y la convertimos a un array de doubles
	char str[MAX_ARRAY_LENGTH];
	readLine(socket, str, MAX_ARRAY_LENGTH);
	str_to_double(str, vector);
	return 0;
}

//     -----     Funciones de errores     -----

void read_int_error(int socket, int* number) {
    if (read_int(socket, number) == -1) {
        printf("Error: no se pudo leer el número\n\n");
        exit(1);
    }
}

void write_int_error(int socket, int number) {
    if (write_int(socket, number) == -1) {
        printf("Error: no se pudo escribir el número\n\n");
        exit(1);
    }
}

void write_double_vector_error(int socket, double* vector, int arrSize) {
    if (write_double_vector(socket, vector, arrSize) == -1) {
        printf("Error: no se pudo escribir el vector de doubles\n\n");
        exit(1);
    }
}

void read_double_vector_error(int socket, double* vector) {
    if (read_double_vector(socket, vector) == -1) {
        printf("Error: no se pudo leer el vector de doubles\n\n");
        exit(1);
    }
}

void readLine_error(int fd, void* buffer, size_t n) {
    if (readLine(fd, buffer, n) == -1) {
        printf("Error: no se pudo leer la cadena de caracteres\n\n");
        exit(1);
    }
}

void writeLine_error(int socket, char* buffer) {
    if (writeLine(socket, buffer) == -1) {
        printf("Error: no se pudo escribir la cadena de caracteres\n\n");
        exit(1);
    }
}
