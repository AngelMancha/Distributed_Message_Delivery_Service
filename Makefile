CC = gcc
CFLAGS = -Wall -Werror -Wextra -g
LDFLAGS = -L. -lpthread -lrt

SERVER = server.c

all: server
	rm ./DDBB/perfiles/*
	rm ./DDBB/mensajes/*
server: $(SERVER) gestiones.c funciones_server.c 
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

clean:
	rm -f server
	rm ./DDBB/*
