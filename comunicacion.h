#ifndef COMUNICACION_H
#define COMUNICACION_H

#define MAXSIZE 1024

struct perfil {
    int sd_client;
    char *nombre;
    char *alias;
    char *fecha;
    char *c_op;
};


struct respuesta {
    int code_error;
};
#endif