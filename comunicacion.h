#ifndef COMUNICACION_H
#define COMUNICACION_H

struct perfil {
    int sd_client;
    char *nombre;
    char *alias;
    char *fecha;
    int c_op;
};


struct respuesta {
    int code_error;
};
#endif