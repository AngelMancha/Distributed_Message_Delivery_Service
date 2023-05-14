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
#include "comunicacion.h"

int read_username(int sd, char *username);
int read_alias(int sd_client, char *alias);
int read_date(int sd, char *date);
int read_port(int sd, char * port);
int read_message(int sd, char *message);