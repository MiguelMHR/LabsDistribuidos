CC = gcc
CFLAGS = -Wall -Wextra -fPIC
LDFLAGS = -shared

all: servidor cliente libclaves.so

servidor: servidor.o
	$(CC) $(CFLAGS) -o servidor servidor.o -ldl -pthread

cliente: funclientes.o
	$(CC) $(CFLAGS) -o cliente funclientes.o

libclaves.so: claves.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o libclaves.so claves.o

servidor.o: servidor.c
	$(CC) $(CFLAGS) -c servidor.c

claves.o: claves.c claves.h
	$(CC) $(CFLAGS) -c -o claves.o claves.c

funclientes.o: funclientes.c
	$(CC) $(CFLAGS) -c funclientes.c

clean:
	rm -f *.o *.so servidor cliente
