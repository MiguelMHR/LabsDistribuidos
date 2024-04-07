CC = gcc
CPPFLAGS = -Wall -Wextra -Werror
LDLIBS = -lrt -lpthread
LDFLAGS = -L$(INSTALL_PATH)/lib/

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./lib

# Generación programas principales -> cliente y servidor
all: dir libclaves servidor cliente 
.PHONY : all

# Creación directorio lib
dir:
	$(shell mkdir -p lib)

# Creación librería compartida
libclaves: claves.c
	$(CC) $(LDFLAGS) $(CPPFLAGS) -fPIC -shared $^ -o ./lib/$@.so $(LDLIBS)

# Compilación programas principales
servidor: servidor.c 
	$(CC) $(LDFLAGS) $(CPPFLAGS) $^ -o $@.out $(LDLIBS)  

cliente: cliente.c
	$(CC) $(CPPFLAGS) $^ -o $@.out -L./lib -lclaves -Wl,-rpath=./lib $(LDLIBS)

# Limpieza archivos generados
clean:
	rm -f *.o *.so ./lib/*.so *.out -d ./lib ./mensajes/* ./mensajes

.SUFFIXES:
.PHONY : clean