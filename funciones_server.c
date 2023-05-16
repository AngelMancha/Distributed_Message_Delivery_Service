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
#include "gestiones.h"
#include "comunicacion.h"
#include <errno.h>
#include "funciones_server.h"

#define MAXSIZE 1024


int sendMessage(int socket, char *buffer, int len)
{
    int r;
    int l = len;
    do { 
        r = write(socket, buffer, l);
        l = l - r;
        buffer = buffer + r;
    } while ((l>0) && (r>=0));

    if (r < 0)
        return (-1); /* fallo */
    else 
        return(0); /* se ha enviado longitud */
}


ssize_t readLine(int fd, void *buffer, size_t n)
{
	ssize_t numRead;  /* num of bytes fetched by last read() */
	size_t totRead;	  /* total bytes read so far */
	char *buf;
	char ch;
	if (n <= 0 || buffer == NULL) { 
		errno = EINVAL;
		return -1; 
	}

	buf = buffer;
	totRead = 0;
	
	for (;;) {
        	numRead = read(fd, &ch, 1);	/* read a byte */
        	if (numRead == -1) {	
            		if (errno == EINTR)	/* interrupted -> restart read() */
                		continue;
            	else
			return -1;		/* some other error */
        	} else if (numRead == 0) {	/* EOF */
            		if (totRead == 0)	/* no byres read; return 0 */
                		return 0;
			else
                		break;
        	} else {			/* numRead must be 1 if we get here*/
            		if (ch == '\n')
                		break;
            		if (ch == '\0')
                		break;
            		if (totRead < n - 1) {		/* discard > (n-1) bytes */
				totRead++;
				*buf++ = ch; 
			}
		} 
	}
	*buf = '\0';
    	return totRead;
}


/// FUNCIONES PARA LEER LOS CAMPOS DE LOS SOCKETS
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