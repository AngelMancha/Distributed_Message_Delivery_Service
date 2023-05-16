#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include "leer_campos.h"
#include "comunicacion.h"
#include "funciones_server.h"


int read_username(int sd, char *username, struct perfil *perfil) {
    if (readLine(sd, (char*) username, MAXSIZE) < 0) {
            perror("Error al leer el username");
            return -1;
        }
    perfil->nombre = malloc(MAXSIZE);
    strcpy(perfil->nombre, username);

    return 0;
}

int read_alias(int sd, char *alias) {
   if (readLine(sd, (char*) alias, MAXSIZE) < 0) {
            perror("Error al leer el alias");
            return 3;
        }
    return 0;
}


int read_date(int sd, char *date, struct perfil *perfil) {
    if (readLine(sd, (char*) date, MAXSIZE) < 0) {
        perror("Error al leer la fecha");
        return 3;
    }
    perfil->fecha = malloc(MAXSIZE);
    strcpy(perfil->fecha, date);
    return 0;
}

int read_port(int sd, char * port) {
    if (readLine(sd, (char*) port, MAXSIZE) < 0) {
            perror("Error al leer el sd_client");
            return -1;
        }
    return 0;
}

int read_message(int sd, char *message) {
    if (readLine(sd, (char*) message, MAXSIZE) < 0) {
            perror("Error al leer el sd_client");
            return -1;
        }
    return 0;
}

void reservar_memoria_perfil(struct perfil *perfil, int size) {
    perfil->nombre = malloc(size * sizeof(char));
    if (perfil->nombre == NULL) {
        fprintf(stderr, "Failed to allocate memory for nombre\n");
        exit(1);
    }
    perfil->alias = malloc(size * sizeof(char));
    if (perfil->alias == NULL) {
        fprintf(stderr, "Failed to allocate memory for alias\n");
        exit(1);
    }
    perfil->fecha = malloc(size * sizeof(char));
    if (perfil->fecha == NULL) {
        fprintf(stderr, "Failed to allocate memory for fecha\n");
        exit(1);
    }
    perfil->c_op = malloc(size * sizeof(char));
    if (perfil->c_op == NULL) {
        fprintf(stderr, "Failed to allocate memory for c_op\n");
        exit(1);
    }
    perfil->status = malloc(size * sizeof(char));
    if (perfil->status == NULL) {
        fprintf(stderr, "Failed to allocate memory for status\n");
        exit(1);
    }
    perfil->IP = malloc(size * sizeof(char));
    if (perfil->IP == NULL) {
        fprintf(stderr, "Failed to allocate memory for IP\n");
        exit(1);
    }
}

void liberar_memoria_perfil(struct perfil *perfil) {
    if (perfil == NULL) {
        return;
    }

    free(perfil->nombre);
    free(perfil->alias);
    free(perfil->fecha);
    free(perfil->c_op);
    free(perfil->status);
    free(perfil->IP);

}
