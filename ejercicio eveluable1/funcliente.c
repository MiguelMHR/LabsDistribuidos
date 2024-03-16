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
#define KEY_MAX 32
#define MAX_VALUE1 256
#define MAX_VALUE2 32

struct funcliente
{
    int key;
    int operacion; 
    char *value1[256];
    int N_value2; 
    double *V_value2;
};

