#include "claves.h"
#include <stdio.h>

void test_init(){
    printf("🧪 Iniciando prueba de init()...\n");
    int sucess = init();
    if (sucess == 0){
        printf("✅ (init): Success: la carpeta se limpió correctamente\n\n");
    } else {
        printf("❌ (init): Error: no se pudo limpiar la carpeta\n\n");
    }
}

void test_set_value(){
    printf("🧪 Iniciando pruebas de set_value()...\n");
    double V_value_2[3] = {1.0, 2.0, 4.0};
    int sucess = set_value(1, "hola", 3, V_value_2);
    if (sucess == 0){
        printf("✅ (set_value): Success: la tupla se añadió correctamente para realizar las pruebas\n\n");
    } else {
        printf("❌ (set_value): Error: la tupla no se pudo añadir\n\n");
    }
}

void test_modify_value(){
    printf("🧪 Iniciando pruebas de modify_key()...\n");
    double V_value_2[3] = {1.0, 2.0, 4.0};
    int sucess = modify_value(1, "hello", 3, V_value_2);
    if (sucess == 0){
        printf("✅ (modify_key): Success: la tupla fue modificada correctamente\n\n");
    } else {
        printf("❌ (modify_key): Error: no se pudo modificar la tupla\n\n");
    }
}

void test_delete_key(){
    printf("🧪 Iniciando pruebas de delete_key()...\n");
    int sucess = delete_key(1);
    if (sucess == 0){
        printf("✅ (delete_key): Success: la tupla fue eliminada correctamente\n\n");
    } else {
        printf("❌ (delete_key): Error: no se pudo eliminar la tupla\n\n");
    }
}

void test_exist(){
    printf("🧪 Iniciando pruebas de exists()...\n");
    int sucess = exist(1);
    if (sucess == 0){
        printf("✅ (exists): Success: se consiguió conocer la existencia de la tupla\n\n");
    } else {
        printf("❌ (exists): Error: no se pudo reconocer la existencia de la tupla\n\n");
    }
}

int main() {
    // En este main se deberá cambiar el test que se quiere ejecutar
    init();
    test_set_value();
    return 0;
}
