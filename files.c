// ----------     LIBRERÍAS Y DEFINICIONES     ---------- //

// Librerías Básicas
#include <stdio.h>   
#include <stdlib.h> 
#include <string.h> 
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "claves.h"

// Definiciones básicas
#define MAX_STR_LENGTH 1024

// ----------     FUNCIONES AUXILIARES     ---------- //

// Función para limpiar el directorio de mensajes
int clean_directory(const char *dirname) {
    printf("\n[files][server] Limpiando directorio de mensajes\n");
    DIR *dir;
    struct dirent *entry;
    char path[MAX_STR_LENGTH];

    dir = opendir(dirname);
    if (dir == NULL) {
        perror("Error al abrir el directorio");
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
    printf("[files][server] Se limpió el directorio de mensajes\n\n");
    return 0;
}

// ----------     FUNCIONES PRINCIPALES     ---------- //

// Función para comprobar la existencia del mensaje
int exist(int key) {
    printf("\n[files][server] Comprobando existencia de la clave %d\n", key);
    FILE *file;
    char path[MAX_STR_LENGTH];
    // Combinamos el path con la key para obtener el nombre del archivo
    sprintf(path, "./mensajes/%d.txt", key);
    // Abrimos el archivo en modo lectura para comprobar si existe
    file = fopen(path, "r");
    // Si no existe, retornamos 0
    if (file==NULL) {
        printf("[files][server] La clave no existe\n\n");
        return 0;
    }
    // Si existe, retornamos 1 y cerramos archivo
    fclose(file);
    printf("[files][server] La clave existe\n\n");
    return 1;
}

// Función para reiniciar el directorio de mensajes
int init(){
    printf("\n[files][server] Inicializando directorio de mensajes\n");
    const char *dirname = "./mensajes";

    // Si no está creado, lo creamos
    if (fopen(dirname, "r") == NULL) {
        // Si no se pudo crear, retornamos -1
        if (mkdir(dirname, 0777) == -1) {
            perror("No se pudo crear el directorio de mensajes");
            return -1;
        }
    } 
    // Si ya existe, lo limpiamos
    else {    
        if (clean_directory(dirname) == -1) {
            printf("No se pudo limpiar el directorio de mensajes\n");
            return -1;
        }
    }
    printf("[files][server] Directorio de mensajes inicializado\n\n");
    return 0;
}

// Función para almacenar la tupla
int set_value(int key, char* value1, int N_value2, double* V_value_2) {
    printf("\n[files][server] Almacenando tupla con clave %d\n", key);
    if (exist(key)) {
        printf("[files][server] La clave ya existe\n\n");
        return -1;
    }
    else if (N_value2 <= 1 || N_value2 >= 32) {
        printf("El tamaño del vector no es válido\n\n");
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
            perror("No se pudo abrir el archivo");
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
        printf("[files][server] Se almacenó la tupla\n\n");
        return 0;
    }
}

// Función para obtener la tupla
int get_value(int key, char* value1, int* N_value2, double* V_value_2) {
    printf("\n[files][server] Obteniendo tupla con clave %d\n", key);
    if (!exist(key)) {
        printf("La clave no existe\n\n");
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
            perror("No se pudo abrir el archivo");
            return -1;
        }
        // Leemos la tupla del archivo
        fscanf(file, "%[^-]-%d-[", value1, N_value2);
        for (int i = 0; i < *N_value2; i++) {
            fscanf(file, "%lf,", &V_value_2[i]);
        }
        // Cerramos el archivo
        fclose(file);
        printf("[files][server] Se obtuvo la tupla\n\n");
        return 0;
    }
}

// Función para eliminar la tupla
int delete_key(int key) {
    printf("\n[files][server] Eliminando tupla con clave %d\n", key);
    if (!exist(key)) {
        printf("La clave no existe\n\n");
        return -1;
    }
    else {
        char path[MAX_STR_LENGTH];
        // Combinamos el path con la key para obtener el nombre del archivo
        sprintf(path, "./mensajes/%d.txt", key);
        // Eliminamos el archivo
        remove(path);
        printf("[files][server] Se eliminó la tupla\n\n");
        return 0;
    }
}

// Función para modificar la tupla
int modify_value(int key, char* value1, int N_value2, double* V_value_2) {
    printf("\n[files][server] Modificando tupla con clave %d\n", key);
    if (!exist(key)) {
        printf("La clave no existe\n\n");
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
            perror("No se pudo abrir el archivo");
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
        printf("[files][server] Se modificó la tupla\n\n");
        return 0;
    }
}
