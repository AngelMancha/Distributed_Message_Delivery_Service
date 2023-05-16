#ifndef COMUNICACION_H
#define COMUNICACION_H

#define MAXSIZE 1024

struct perfil {
    int sd_client;
    char *nombre;
    char *alias;
    char *fecha;
    char *c_op;
    char status[MAXSIZE];
    char *IP;
    int port;
    int last_id;
};


struct respuesta {
    char code_error[2];
};

struct mensaje{
    char mensaje[MAXSIZE];
    unsigned int id;
    char remitente[MAXSIZE];
    //int status; //1 pending y 0 read
};



#endif