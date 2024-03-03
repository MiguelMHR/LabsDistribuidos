//
// Created by kai on 03/03/2024.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>


int buscar(){
  fd = open(infile, O_RDONLY);
  if(fd < 0){
    printf("Error abriendo fichero env.txt %s \n", infile);
    exit(-1);
  }
}