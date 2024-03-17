#include <stdio.h>
#include <dlfcn.h>

int main() {
    // Cargar la biblioteca dinámica
    void *handle;
    handle = dlopen("./libclaves.so", RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "Error al cargar la biblioteca dinámica: %s\n", dlerror());
        return 1;
    }

    // Definir los punteros de función para las operaciones de la API
    int (*init)();
    int (*exist)(int);
    int (*set_value)(int, char *, int, double *);
    int (*get_value)(int, char *, int *, double *);
    int (*modify_value)(int, char *, int, double *);
    int (*delete_key)(int);

    // Asignar las direcciones de las funciones
    init = dlsym(handle, "init");
    exist = dlsym(handle, "exist");
    set_value = dlsym(handle, "set_value");
    get_value = dlsym(handle, "get_value");
    modify_value = dlsym(handle, "modify_value");
    delete_key = dlsym(handle, "delete_key");

    // Comprobar errores al cargar las funciones
    if (!init || !exist || !set_value || !get_value || !modify_value || !delete_key) {
        fprintf(stderr, "Error al obtener el símbolo: %s\n", dlerror());
        dlclose(handle);
        return 1;
    }

    // Cerrar la biblioteca dinámica
    dlclose(handle);

    return 0;
}
