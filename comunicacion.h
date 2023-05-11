#ifndef COMUNICACION_H
#define COMUNICACION_H

#define MAXSIZE 1024

struct perfil {
    int sd_client;
    char *nombre;
    char *alias;
    char *fecha;
    char *c_op;
    char * status;
    char *IP;
    int port;
    struct mensaje *mensajes;
};


struct respuesta {
    int code_error;
};

struct mensaje{
    char *mensaje;
    unsigned int id;
    char *remitente;
};



#endif