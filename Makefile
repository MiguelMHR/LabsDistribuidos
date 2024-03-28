CC = gcc
CPPFLAGS = -I$(INSTALL_PATH)/include -Wall -Wextra 
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
	$(CC) $(LDFLAGS) $(CPPFLAGS) $(LDLIBS) -fPIC -shared $^ -o ./lib/$@.so
cliente: cliente.c
	$(CC) $(CPPFLAGS) $(LDLIBS) $^ -o $@.out -L./lib -lclaves -Wl,-rpath=./lib
servidor: servidor.c 
	$(CC) $(LDFLAGS) $(CPPFLAGS) $(LDLIBS) $^ -o $@.out    


# Limpieza archivos generados
clean:
	rm -f *.o *.so ./lib/*.so *.out -d ./lib
	@if [ ! -z "$(shell ls -A /dev/mqueue)" ]; then rm /dev/mqueue/*; fi