
/* Servicios*/
#include "comunicacion.h"

int register_gestiones(struct perfil perfil);
int unregister_gestiones(struct perfil perfil);
int connect_gestiones(struct perfil perfil);
int disconnect_gestiones(struct perfil perfil);
int send_to_server_gestiones(struct perfil perfil, char *destinatario, char *mensaje);
int is_connected(char* destinatario, int * port, char *IP);
int num_mensajes_pendientes(char *destinatario);
char **extraerMensajes(char *destinatario, int numMensajes);
char **extraerRemitentes(char *destinatario, int numMensajes);
