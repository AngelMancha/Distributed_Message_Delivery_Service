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
#include "gestiones.h"
#include "comunicacion.h"
#include <errno.h>

#define MAXSIZE 1024


//mutex
pthread_mutex_t mutex_mensaje;
int mensaje_no_copiado;
pthread_cond_t cond_mensaje;

//función que actúa como cliente dentro del servidor para conectarse al puerto del cliente
// void connection_client(void *mess)
// {   struct comunicacion_client mensaje;

//     dprintf(2, "He llegado a connection_client\n");

//     mensaje = (*(struct comunicacion_client *) mess);

//     struct sockaddr_in address; // direccion del servidor
//     int sd_client; // socket del cliente
//     int addrlen = sizeof(address); // longitud de la direccion
    
//     // open socket
//      if ((sd_client = socket(AF_INET, SOCK_STREAM, 0)) == 0)
//      {
//         perror("socket: ");
//      }
//     // connect to server
//     if (connect(sd_client, (struct sockaddr *)&mensaje.IP, sizeof(addrlen)) < 0)
//     {
//         perror("connect ");

//     }
// }

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



int recibir_msj_socket(int sd_client, struct perfil *perfil) {
   
    char username [MAXSIZE];
    char alias [MAXSIZE];
    char date [MAXSIZE];
    char c_op[MAXSIZE];

   
    dprintf(2, "DESCRIPTO %d\n", sd_client);

    // *********** OPERATION CODE ***********
    if (readLine(sd_client, (char*) &c_op, MAXSIZE) < 0) {
        perror("Error al leer la respuesta");
        return -1;
    }

    dprintf(2, "cop: %s\n", c_op);

    if (strcmp(c_op, "REGISTER")==0)
    {
        // *********** USERNAME ***********

        if (readLine(sd_client, (char*) username, MAXSIZE) < 0) {
            perror("Error al leer el username");
            return -1;
        }

        dprintf(2, "username: %s\n", username);
        
        // ********** ALIAS ***********
        if (readLine(sd_client, (char*) alias, MAXSIZE) < 0) {
            perror("Error al leer el alias");
            return -1;
        }

        dprintf(2, "alias: %s\n", alias);

        //     *********** DATE ***********
        // date = malloc(len_date + 1);
        if (readLine(sd_client, (char*) date, MAXSIZE) < 0) {
            perror("Error al leer la fecha");
            return -1;
        }

        dprintf(2, "date: %s\n", date);

         // Reserva de memoria
        perfil->nombre = malloc(MAXSIZE);
        perfil->alias = malloc(MAXSIZE);
        perfil->fecha = malloc(MAXSIZE);
        perfil->c_op = malloc(MAXSIZE);

        //se rellena la estructura de la petición
        strcpy(perfil->nombre, username);
        strcpy(perfil->alias, alias);

        strcpy(perfil->fecha, date);

        strcpy(perfil->c_op, c_op);
    
    } else if (strcmp(c_op, "UNREGISTER")==0) {
        // ********** ALIAS ***********
        if (readLine(sd_client, (char*) alias, MAXSIZE) < 0) {
            perror("Error al leer el alias");
            return -1;
        }
        perfil->alias = malloc(MAXSIZE);
        perfil->c_op = malloc(MAXSIZE);

        strcpy(perfil->c_op, c_op);
        strcpy(perfil->alias, alias);

    } else if (strcmp(c_op, "CONNECT")==0) {
        char port[MAXSIZE];
        // ********** ALIAS ***********
        if (readLine(sd_client, (char*) alias, MAXSIZE) < 0) {
            perror("Error al leer el alias");
            return -1;
        }
        if (readLine(sd_client, (char*) port, MAXSIZE) < 0) {
            perror("Error al leer el alias");
            return -1;
        }

        int port_perfil = atoi(port);

        perfil->alias = malloc(MAXSIZE);
        perfil->c_op = malloc(MAXSIZE);

        strcpy(perfil->c_op, c_op);
        strcpy(perfil->alias, alias);

        perfil->port = port_perfil;
    }


    return 0;
}

void tratar_mensaje(void *sd_client_tratar) 
{   
    
    dprintf(2, "He llegado a tratar\n");
    //struct perfil mensaje;	                    //mensaje local 		
    struct respuesta respuesta;	                //respuesta a la petición          
    int resultado;		                        // resultado de la operación 
    int sd_client;

    
    
    struct perfil perfil;
    

    pthread_mutex_lock(&mutex_mensaje);

    sd_client = (* (int *) sd_client_tratar);
    
    
    //Como ya se ha copiado el mensaje, despetarmos al servidor 
    mensaje_no_copiado = false;
    pthread_cond_signal(&cond_mensaje);
	pthread_mutex_unlock(&mutex_mensaje);

    resultado = recibir_msj_socket(sd_client, &perfil);

    //leemos y ejecutamos la petición
     dprintf(2, "cop: %s\n", perfil.c_op);

    if (strcmp(perfil.c_op, "REGISTER") == 0) {
        dprintf(2, "\n\nSe ha hecho una llamada al método REGISTER\n");
        perfil.status = "Desconectado";
        perfil.port = 0;
        resultado = register_gestiones(perfil);
        
    } else if (strcmp(perfil.c_op, "UNREGISTER") == 0) {
        resultado = unregister_gestiones(perfil);
        
    } else if (strcmp(perfil.c_op, "CONNECT") == 0) {

        dprintf(2, "\n\nESTOY EN EL CONNECT\n\n");

        // //creamos un hilo que actúe como cliente para conectarse al puerto del cliente
        // if (pthread_create(&thid, &t_attr, (void *)connection_client, (void *)&mensaje) == 0) {
        //     // se espera a que el thread copie el mensaje
        // }
        // else {
        //     printf("Error: no se ha podido crear el thread connection_client.\n");
        //     exit(-1);
        // }
        dprintf(2, "USERNAME EN CONNECT ES %s\n", perfil.nombre);
        resultado = connect_gestiones(perfil);

    }
    else {
        printf("Error: código de operación no válido.\n");
        exit(-1);
    }


    
    respuesta.code_error = resultado;
    dprintf(2, "Respuesta: %d\n", respuesta.code_error);
    
    // Traducción de la respuesta a formato de red

    respuesta.code_error = htonl(respuesta.code_error);

    // Se envian todos los campos de la respuesta
    if (write (sd_client, &respuesta.code_error, sizeof(int)) < 0)
    {
        perror("write: ");
    }

    else
    {
        printf("Respuesta enviada CORRECTAMENTE JAJAJA YA QUISIERAS\n");
    }
    
    close (sd_client);


	pthread_exit(0);
}



int main(int argc, char *argv[]){
    dprintf(2, "SERVER\n");

	pthread_attr_t t_attr;		
   	pthread_t thid;


    struct sockaddr_in address; // direccion del servidor
    int sd_server; // socket del servidor
    int sd_client; // socket del cliente
    int addrlen = sizeof(address); // longitud de la direccion
    int opt = 1; // opcion para el setsockopt
    int port_number;

    if (argc < 2) 
    {
        printf("Por favor especifique el puerto\n");
        return -1;
    } else {
        port_number = atoi(argv[1]);
    }

     // open server sockets
     if ((sd_server = socket(AF_INET, SOCK_STREAM, 0)) == 0)
     {
        perror("socket: ");
        return -1;
     }

    // socket options
    if (setsockopt(sd_server, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
     {
         perror("setsockopt: ");
         return -1;
     }


    // bind + listen
     address.sin_family      = AF_INET ;
     address.sin_addr.s_addr = INADDR_ANY ;
     address.sin_port        = htons(port_number) ;


    if (bind(sd_server, (struct sockaddr *)&address,  sizeof(address)) < 0)
    {
        perror("bind: ");
        return -1;
    }

    if (listen(sd_server, 3) < 0)
    {
        perror("listen: ") ;
        return -1;
    }


	pthread_mutex_init(&mutex_mensaje, NULL);
	pthread_cond_init(&cond_mensaje, NULL);
	pthread_attr_init(&t_attr);

	// atributos de los threads, threads independientes que no necesitan hacer un join al final
	pthread_attr_setdetachstate(&t_attr, PTHREAD_CREATE_DETACHED);
    mensaje_no_copiado = true;

    while(1) {

        sd_client = accept(sd_server, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (sd_client <= 0) {
            perror("accept");
            return -1;
        }


        dprintf(2, "Hola he llegado aqui 2\n");
        if (pthread_create(&thid, &t_attr, (void *)tratar_mensaje, (void *)&sd_client) == 0) {
            // se espera a que el thread copie el mensaje 
			pthread_mutex_lock(&mutex_mensaje);
			while (mensaje_no_copiado)
				pthread_cond_wait(&cond_mensaje, &mutex_mensaje);
			mensaje_no_copiado = true;
			pthread_mutex_unlock(&mutex_mensaje);
        }
			
    }

    return 0;
    
}