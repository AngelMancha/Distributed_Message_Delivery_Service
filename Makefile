CC = gcc
CFLAGS = -Wall -Werror -Wextra -g
LDFLAGS = -L. -lpthread -lrt

SERVER = server.c

all: server
	rm ./DDBB/*
server: $(SERVER) gestiones.c funciones_server.c leer_campos.c
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

clean:
	rm -f server
	rm ./DDBB/*
