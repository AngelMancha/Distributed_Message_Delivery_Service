#include <unistd.h>
#include "comunicacion.h"

int sendMessage(int socket, char *buffer, int len);
ssize_t readLine(int fd, void *buffer, size_t n);
int read_username(int sd, char *username, struct perfil *perfil);
int read_alias(int sd_client, char *alias);
int read_date(int sd, char *date, struct perfil *perfil);
int read_port(int sd, char * port);
int read_message(int sd, char *message);