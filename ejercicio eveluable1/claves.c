//
// Created by kai on 03/03/2024.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>

#define DIRECTORIO "clientes"
#define KEY_MAX 256

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
    if (chdir(DIRECTORIO) == -1) {
        // Error al cambiar al directorio clientes
        return -1;
    }
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
  
    DIR *dir = opendir(DIRECTORIO);
    if (!dir) {
        // Error al abrir el directorio
        return -1;
    }
    
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] != '.') { 
            // Ignorar DIRECTORIOs '.' y '..'
            char filename[KEY_MAX];
            snprintf(filename, KEY_MAX, "%s/%s", DIRECTORIO, entry->d_name);
            if (unlink(filename) == -1) {
                closedir(dir);
                return -1;
            }
        }
    }
    closedir(dir);
    return 0;
}

int set_value(int key, char *value1, int N_value2, double *V_value2) {
    char nombre_archivo[32]; // Tamaño de un int
    sprintf(nombre_archivo, "%d.txt", key); // Genera el nombre del archivo usando la key
    
    if (exist(nombre_archivo) == 0) {
        FILE *archivo;
        archivo = fopen(nombre_archivo, "w"); // Abre el archivo en modo escritura
        if (archivo == NULL) {
            // No se pudo crear el archivo
            return -1;
        }

        // Escribe en el archivo la key y value1
        fprintf(archivo, "%i \n%s \n", key, value1);
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

int get_value(int key, char *value1, int *N_value2, double *V_value2);


int modify_value(int key, char *value1, int N_value2, double *V_value2);


int delete_key(int key);
