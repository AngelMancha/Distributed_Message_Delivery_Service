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
    int num_mensajes_recibidos;
};


struct respuesta {
    int code_error;
};

struct mensaje{
    char mensaje[MAXSIZE];
    unsigned int id;
    char remitente[MAXSIZE];
    //int status; //1 pending y 0 read
};



#endif