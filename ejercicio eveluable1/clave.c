//
// Created by kai on 03/03/2024.
//
#include "clave.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>

#define DIRECTORIO "clientes"
#define KEY_MAX 32
#define MAX_VALUE1 256
#define MAX_VALUE2 32

struct respuesta
{
    int respuesta; 
    int key;
    char *value1[256];
    int N_value2; 
    double *V_value2;
};

int crearDirectorio() {

    DIR *dir = opendir(DIRECTORIO);
    int respuesta;
    if (dir) {
        // Directorio ya existe
        respuesta = 1;
        closedir(dir);
    } 
    else if (ENOENT == errno) {
        if (mkdir(DIRECTORIO, 0777) == -1) {
            // No se ha podido crear directorio
            respuesta = 0;
        }
        else {
            // Directorio creado exitosamente
            respuesta = 1;  
        }
    } 
    else {
        // Error al abrir el directorio
        respuesta = 0;
    }
    return respuesta;
}

int exist(int key){
    // Si no existe el directorio que contiene los archivos, lo crea
    if (crearDirectorio() == 0){
        return -1;
    }

    char nombre_archivo[KEY_MAX + strlen(DIRECTORIO)]; // Tamaño de un int + el nombre del directorio
    sprintf(nombre_archivo, "%s/%i.txt", DIRECTORIO, key); // Genera el nombre del archivo usando la key

    int fd = open(nombre_archivo, O_RDONLY);
    if(fd != -1){
        // Se ha encontrado el archivo
        close(fd);
        return 1;
    }
    else{
        // No se ha encontrado
        return 0;
    }
}

int init(){

    if (crearDirectorio() == 0){
        return -1;
    }

    char comando[256];
    sprintf(comando, "rm -rf %s", DIRECTORIO);
    
    // Ejecutar el comando para borrar el directorio y todos sus contenidos
    int resultado = system(comando);
    
    if (resultado == 0) {
        return 0; // Borrado exitoso
    } 
    else {
        return -1; // Error al borrar el directorio
    }
}

int set_value(int key, char *value1, int N_value2, double *V_value2){
    
    if (exist(key) == 0) {

        if (N_value2 < 1 || N_value2 > 32) {
            // N_value2 fuera de rango
            return -1; 
        }

        char nombre_archivo[KEY_MAX + strlen(DIRECTORIO)]; // Tamaño de un int + el nombre del directorio
        sprintf(nombre_archivo, "%s/%i.txt", DIRECTORIO, key); // Genera el nombre del archivo usando la key

        FILE *archivo;
        // Abre el archivo en modo escritura
        archivo = fopen(nombre_archivo, "w"); 
        if (archivo == NULL) {
            // No se pudo crear el archivo
            return -1;
        }

        // Escribe en el archivo la key y value1
        fprintf(archivo, "%i \n%s \n%i \n", key, value1, N_value2);
        // Escribe en el archivo el double
        for (int i = 0; i < N_value2; i++) {
            fprintf(archivo, "%f ", V_value2[i]);
        }
        fclose(archivo);
        return 0;
    } 
    else {
        return -1;
    }
}

int get_value(int key, char *value1, int *N_value2, double *V_value2) {
    struct respuesta resp;
    resp.respuesta = -1; // Por defecto, respuesta de error

    if (exist(key) == 1) {
        char nombre_archivo[KEY_MAX + strlen(DIRECTORIO)]; // Tamaño de un int + longitud del nombre del directorio
        sprintf(nombre_archivo, "%s/%i.txt", DIRECTORIO, key); // Genera el nombre del archivo usando la clave

        FILE *archivo;
        archivo = fopen(nombre_archivo, "r");

        if (archivo == NULL) {
            // No se pudo abrir el archivo
            return resp;
        }

        // Lee los valores del archivo
        if (fscanf(archivo, "%d", &(resp.key)) != 1) {
            // Error al leer el valor de la clave
            fclose(archivo);
            return resp;
        }

        if (fgets(resp.value1, MAX_VALUE1, archivo) == NULL) {
            // Error al leer value1
            fclose(archivo);
            return resp;
        }

        if (fscanf(archivo, "%d", &(resp.N_value2)) != 1) {
            // Error al leer N_value2
            fclose(archivo);
            return resp;
        }

        if (resp.N_value2 < 1 || resp.N_value2 > MAX_VALUE2) {
            // N_value2 fuera de rango
            fclose(archivo);
            return resp;
        }

        for (int i = 0; i < resp.N_value2; i++) {
            if (fscanf(archivo, "%lf", &(resp.V_value2[i])) != 1) {
                // Error al leer los valores de V_value2
                fclose(archivo);
                return resp;
            }
        }

        fclose(archivo);
        // Lectura exitosa
        resp.respuesta = 0;
        return resp;
    } else {
        // El archivo no existe
        return resp;
    }
}


int modify_value(int key, char *value1, int N_value2, double *V_value2){
    
    if (exist(key) == 1){

        if (N_value2 < 1 || N_value2 > 32) {
            // N_value2 fuera de rango
            return -1; 
        }

        char nombre_archivo[KEY_MAX + strlen(DIRECTORIO)]; // Tamaño de un int + el nombre del directorio
        sprintf(nombre_archivo, "%s/%i.txt", DIRECTORIO, key); // Genera el nombre del archivo usando la key

        FILE *archivo;
        archivo = fopen(nombre_archivo, "w"); 
        
        if (archivo == NULL) {
            // No se pudo abrir el archivo
            return -1;
        }

        // Escribe en el archivo la key y value1
        fprintf(archivo, "%i \n%s \n%i \n", key, value1, N_value2);
        // Escribe en el archivo el double
        for (int i = 0; i < N_value2; i++) {
            fprintf(archivo, "%f ", V_value2[i]);
        }
        fclose(archivo);
        return 0;
    } 
    else {
        return -1;
    }
};


int delete_key(int key){

    if (exist(key) == 1) {
        
        char nombre_archivo[KEY_MAX + strlen(DIRECTORIO)]; // Tamaño de un int + el nombre del directorio
        sprintf(nombre_archivo, "%s/%i.txt", DIRECTORIO, key); // Genera el nombre del archivo usando la key

        if (unlink(filename) == -1) {
            return -1;
        }
        return 0;
    }
    else {
        return -1;
    }
};
