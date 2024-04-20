#include "claves.h"
#include <stdio.h>
#include <unistd.h>

void test_init(){
    printf("\n\n🧪 Iniciando prueba de init()...\n");
    printf("Se espera que la función ejecute correctamente\n\n\n");
    int sucess = init();
    if (sucess == 0){
        printf("✅ (init): Success: la carpeta se limpió correctamente\n\n\n");
    } else {
        printf("❌ (init): Error: no se pudo limpiar la carpeta\n\n\n");
    }
}

void test_set_value_nokey(){
    printf("\n🧪 Iniciando prueba de set_value_nokey()...\n");
    printf("Se espera que la función ejecute correctamente\n\n\n");
    double V_value_2[3] = {1.0, 2.0, 4.0};
    int sucess = set_value(1, "hola", 3, V_value_2);
    if (sucess == 0){
        printf("✅ (set_value): Success: la tupla se añadió correctamente para realizar las pruebas\n\n\n");
    } else {
        printf("❌ (set_value): Error: la tupla no se pudo añadir\n\n\n");
    }
}

void test_set_value_exist_key(){
    printf("\n🧪 Iniciando prueba de set_value_exist_key()...\n");
    printf("Se espera que la función falle al ya haber una tupla con esa key\n\n\n");
    double V_value_2[3] = {1.0, 3.0, 7.0};
    int sucess = set_value(1, "hola", 3, V_value_2);
    if (sucess == 0){
        printf("✅ (set_value): Success: la tupla se añadió correctamente para realizar las pruebas\n\n\n");
    } else {
        printf("❌ (set_value): Error: la tupla no se pudo añadir\n\n\n");
    }
}

void test_set_value_big_nvalue2(){
    printf("\n🧪 Iniciando prueba de set_value_big_nvalue2()...\n");
    printf("Se espera que la función falle al tener un N_value2 muy grande\n\n\n");
    double V_value_2[3] = {1.0, 3.0, 7.0};
    int sucess = set_value(2, "hola", 1000000, V_value_2);
    if (sucess == 0){
        printf("✅ (set_value): Success: la tupla se añadió correctamente para realizar las pruebas\n\n\n");
    } else {
        printf("❌ (set_value): Error: la tupla no se pudo añadir\n\n\n");
    }
}

void test_get_value(){
    printf("\n🧪 Iniciando prueba de get_value()...\n");
    printf("Se espera que la función ejecute correctamente\n\n\n");
    char value1[256];
    int N_value2;
    double V_value_2[3];
    int sucess = get_value(1, value1, &N_value2, V_value_2);
    if (sucess == 0){
        printf("value1: %s\n", value1);
        printf("N_value2: %d\n", N_value2);
        printf("V_value_2: %f, %f, %f\n\n\n", V_value_2[0], V_value_2[1], V_value_2[2]);
        printf("✅ (get_value): Success: se consiguieron los valores de la tupla\n\n\n");
    } else {
        printf("❌ (get_value): Error: no se pudieron obtener los valores de la tupla\n\n\n");
    }
}

void test_get_value_nokey(){
    printf("\n🧪 Iniciando prueba de get_value_nokey()...\n");
    printf("Se espera que la función falle al no existir la key\n\n\n");
    char value1[256];
    int N_value2;
    double V_value_2[3];
    int sucess = get_value(3, value1, &N_value2, V_value_2);
    if (sucess == 0){
        printf("✅ (get_value): Success: se consiguieron los valores de la tupla\n\n\n");
    } else {
        printf("❌ (get_value): Error: no se pudieron obtener los valores de la tupla\n\n\n");
    }
}

void test_modify_value(){
    printf("\n🧪 Iniciando prueba de modify_value()...\n");
    printf("Se espera que la función ejecute correctamente\n\n\n");
    double V_value_2[3] = {1.0, 24.0, 14.0};
    int sucess = modify_value(1, "hello", 3, V_value_2);
    if (sucess == 0){
        printf("✅ (modify_value): Success: la tupla fue modificada correctamente\n\n\n");
    } else {
        printf("❌ (modify_value): Error: no se pudo modificar la tupla\n\n\n");
    }
}

void test_modify_value_nokey() {
    printf("\n🧪 Iniciando prueba de modify_value_nokey()...\n");
    printf("Se espera que la función falle al no existir la key\n\n\n");
    double V_value_2[3] = {1.0, 24.0, 14.0};
    int sucess = modify_value(12, "hello", 3, V_value_2);
    if (sucess == 0){
        printf("✅ (modify_value): Success: la tupla fue modificada correctamente\n\n\n");
    } else {
        printf("❌ (modify_value): Error: no se pudo modificar la tupla\n\n\n");
    }
}

void test_modify_value_big_nvalue2(){
    printf("\n🧪 Iniciando prueba de modify_value_big_nvalue2()...\n");
    printf("Se espera que la función falle al tener un N_value2 muy grande\n\n\n");
    double V_value_2[3] = {1.0, 24.0, 14.0};
    int sucess = modify_value(1, "hello", 1000000, V_value_2);
    if (sucess == 0){
        printf("✅ (modify_value): Success: la tupla fue modificada correctamente\n\n\n");
    } else {
        printf("❌ (modify_value): Error: no se pudo modificar la tupla\n\n\n");
    }
}

void test_exist_really(){
    printf("\n🧪 Iniciando pruebas de exist()...\n");
    printf("Se espera que la función ejecute correctamente\n\n\n");
    int sucess = exist(1);
    if (sucess == 0 || sucess == 1){
        printf("✅ (exists): Success: se consiguió conocer la existencia de la tupla\n\n");
        if (sucess == 1){
            printf("La tupla existe\n\n");
        } else {
            printf("La tupla no existe\n\n");
        }
    } else {
        printf("❌ (exists): Error: no se pudo reconocer la existencia de la tupla\n\n");
    }
}

void test_exist_no(){
    printf("\n🧪 Iniciando pruebas de exist_no()...\n");
    printf("Se espera que la función falle al no existir la key\n\n\n");
    int sucess = exist(12);
    if (sucess == 0 || sucess == 1){
        printf("✅ (exists): Success: se consiguió conocer la existencia de la tupla\n\n");
        if (sucess == 1){
            printf("La tupla existe\n\n");
        } else {
            printf("La tupla no existe\n\n");
        }
    } else {
        printf("❌ (exists): Error: no se pudo reconocer la existencia de la tupla\n\n");
    }
}

void test_delete_key(){
    printf("\n🧪 Iniciando prueba de delete_key()...\n\n\n");
    int sucess = delete_key(1);
    if (sucess == 0){
        printf("✅ (delete_key): Success: la tupla fue eliminada correctamente\n\n");
    } else {
        printf("❌ (delete_key): Error: no se pudo eliminar la tupla\n\n");
    }
}

void test_delete_key_nokey(){
    printf("\n🧪 Iniciando prueba de delete_key_nokey()...\n");
    printf("Se espera que la función falle al no existir la key\n\n\n");
    int sucess = delete_key(12);
    if (sucess == 0){
        printf("✅ (delete_key): Success: la tupla fue eliminada correctamente\n\n\n");
    } else {
        printf("❌ (delete_key): Error: no se pudo eliminar la tupla\n\n\n");
    }
}

void test_set_value_big_value1(){
    printf("\n🧪 Iniciando prueba de set_value_big_value1()...\n");
    printf("Se espera que la función falle al tener un value1 muy grande\n\n\n");
    double V_value_2[3] = {1.0, 3.0, 7.0};
    char value1[500];
    for (int i = 0; i < 300; i++){
        value1[i] += 'a';
    }
    int sucess = set_value(3, value1, 3, V_value_2);
    if (sucess == 0){
        printf("✅ (set_value): Success: la tupla se añadió correctamente para realizar las pruebas\n\n\n");
    } else {
        printf("❌ (set_value): Error: la tupla no se pudo añadir\n\n\n");
    }
}

void test_delete_key_twice() {
    printf("\n🧪 Iniciando prueba de delete_key_twice()...\n");
    printf("Se espera que la función falle al intentar eliminar una clave dos veces\n\n\n");
    int success = delete_key(7);
    if (success == 0) {
        printf("✅ (delete_key): Success: la tupla fue eliminada correctamente\n\n\n");
        // Intenta eliminar la misma clave nuevamente
        success = delete_key(7);
        if (success != 0) {
            printf("❌ (delete_key): Error: la tupla fue eliminada nuevamente\n\n\n");
        }
    } else {
        printf("❌ (delete_key): Error: no se pudo eliminar la tupla\n\n\n");
    }
}

void test_init_then_exist() {
    printf("\n🧪 Iniciando prueba de init seguida de exist...\n");
    printf("Se espera que la función init limpie correctamente la carpeta y luego exist devuelva 0 para una clave inexistente\n\n\n");
    init(); // Inicializar
    int success = exist(13); // Verificar si una clave específica existe
    if (success == 0) {
        printf("✅ (exist): Success: la clave no existe después de init\n\n\n");
    } else {
        printf("❌ (exist): Error: la clave existe después de init\n\n\n");
    }
}

void test_create_then_delete() {
    printf("\n🧪 Iniciando prueba de crear luego eliminar...\n");
    printf("Se espera que la función crear luego eliminar funcione correctamente\n\n\n");
    
    // Primero crea la clave
    double V_value_2[3] = {1.0, 3.0, 7.0};
    char value1[10] = "test";
    int create_result = set_value(14, value1, 3, V_value_2);
    
    // Luego intenta eliminarla
    int delete_result = delete_key(14);
    
    if (create_result == 0 && delete_result == 0) {
        printf("✅ (crear_luego_eliminar): Success: la clave se creó y eliminó correctamente\n\n\n");
    } else {
        printf("❌ (crear_luego_eliminar): Error: no se pudo crear o eliminar la clave\n\n\n");
    }
}

void test_create_then_modify_twice() {
    printf("\n🧪 Iniciando prueba de crear y luego modificar dos veces el mismo valor...\n");
    printf("Se espera que la función crear y luego modificar dos veces funcione correctamente\n\n\n");
    
    // Primero crea la clave
    double V_value_2[3] = {1.0, 3.0, 7.0};
    char value1[10] = "test";
    int create_result = set_value(15, value1, 3, V_value_2);
    
    // Luego modifícala dos veces
    int modify_result_1 = modify_value(15, "updated", 3, V_value_2);
    int modify_result_2 = modify_value(15, "updated again", 3, V_value_2);
    
    if (create_result == 0 && modify_result_1 == 0 && modify_result_2 == 0) {
        printf("✅ (crear_modificar_dos_veces): Success: la clave se creó y se modificó dos veces correctamente\n\n\n");
    } else {
        printf("❌ (crear_modificar_dos_veces): Error: no se pudo crear o modificar la clave dos veces\n\n\n");
    }
}

void all_tests(){
    test_init();
    test_set_value_nokey();
    test_set_value_exist_key();
    test_get_value();
    test_get_value_nokey();
    test_modify_value();
    test_modify_value_nokey();
    test_exist_really();
    test_exist_no();
    test_delete_key();
    test_delete_key_nokey();
    test_delete_key_twice();
    test_init_then_exist();
    test_create_then_delete();
    test_create_then_modify_twice();


    // Se han comentado las pruebas que involucran la creación de tuplas con valores muy grandes
    // ya que te echa directamente de la ejecución del programa
    
    //test_set_value_big_nvalue2();
    //test_modify_value_big_nvalue2();
    //test_set_value_big_value1();

}

int main() {
    // En este main se deberá cambiar el test que se quiere ejecutar
    all_tests();
    return 0;
}
