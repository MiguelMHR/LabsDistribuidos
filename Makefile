CC = gcc
CPPFLAGS = -Wall -Wextra -Werror
LDLIBS = -lrt -lpthread
LDFLAGS = -L$(INSTALL_PATH)/lib/

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./lib

# Generación programas principales -> cliente y servidor
all: dir libclaves servidor cliente
# Creación directorio lib
dir:
	$(shell mkdir -p lib)
# Compilación librería compartida -> libclaves.so
libclaves: claves.c
	$(CC) $(LDFLAGS) $(CPPFLAGS) -fPIC -shared $^ -o ./lib/$@.so $(LDLIBS)
cliente: cliente.c
	$(CC) $(CPPFLAGS) $^ -o $@.out -L./lib -lclaves -Wl,-rpath=./lib $(LDLIBS)
servidor: servidor.c 
	$(CC) $(LDFLAGS) $(CPPFLAGS) $^ -o $@.out $(LDLIBS)  


# Limpieza archivos generados
clean:
	rm -f *.o *.so ./lib/*.so *.out -d ./lib ./mensajes/* ./mensajes
	@if [ ! -z "$(shell ls -A /dev/mqueue)" ]; then rm /dev/mqueue/*; fi