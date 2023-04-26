#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "gestiones.h"
#include "comunicacion.h"

#define MAXSIZE 255



//mutex
pthread_mutex_t mutex_mensaje;
int mensaje_no_copiado;
pthread_cond_t cond_mensaje;

void tratar_mensaje(void *mess) 
{

    struct perfil mensaje;	        //mensaje local 		
    struct respuesta respuesta;	        //respuesta a la petición          
    int resultado;		                // resultado de la operación 

    pthread_mutex_lock(&mutex_mensaje);

    //copia la petición a la variable mensaje
    mensaje = (*(struct perfil *) mess);
    
    //Como ya se ha copiado el mensaje, despetarmos al servidor 
    mensaje_no_copiado = false;
    pthread_cond_signal(&cond_mensaje);
	pthread_mutex_unlock(&mutex_mensaje);


    //leemos y ejecutamos la petición

    switch(mensaje.c_op) {
    case 0:
        resultado = register_gestiones(mensaje);
        break;

    default:
        printf("Error: código de operación no válido.\n");
        exit(-1);
        break;
}

    
    respuesta.code_error = resultado;
    
    
    // Se envian todos los campos de la respuesta

    // if (write ( ((struct peticion *)mess)->sd_client, &respuesta.tupla_peticion.clave, sizeof(int)) < 0)
    // {
    //     perror("write: ");
    // }
    // if (write ( ((struct peticion *)mess)->sd_client, &respuesta.tupla_peticion.valor1, sizeof(respuesta.tupla_peticion.valor1)) < 0)
    // {
    //     perror("write: ");  
    // }
    // if (write ( ((struct peticion *)mess)->sd_client, &respuesta.tupla_peticion.valor2, sizeof(int)) < 0)
    // {
    //     perror("write: ");  
    // }
    
    // char valor3[100];
    // sprintf(valor3, "%lf", respuesta.tupla_peticion.valor3);


    // if (write(((struct peticion *)mess)->sd_client, &valor3, sizeof(valor3)) < 0)
    // {
    //     perror("write: ");
    // }

    if (write ( ((struct perfil *)mess)->sd_client, &respuesta.code_error, sizeof(int)) < 0)
    {
        perror("write: ");
    }
    
    close (((struct perfil *)mess)->sd_client);


	pthread_exit(0);
}


int main(int argc, char *argv[]){
    struct perfil perfil;
	pthread_attr_t t_attr;		
   	pthread_t thid;

    char *username;
    char *alias;
    char *date;
    int c_op;

    int len_username;
    int len_alias;
    int len_date;

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
        
        // se establece la conexión con el cliente
        sd_client = accept(sd_server, (struct sockaddr *)&address,  (socklen_t*)&addrlen) ;
       	if (sd_client <= 0)
	     {
		    perror("accept");
		    return -1;
	    }
        

        // Se reciben todos los campos de la petición del cliente
        
        if (read(sd_client, (char*) &len_username, sizeof(int)) < 0) {
            perror("Error al leer la longitud de username");
            return -1;
        }
        
        len_username = ntohl(len_username);
        username = malloc(len_username + 1);
        if (read(sd_client, (char*) username, strlen(username)) < 0) {
            perror("Error al leer el username");
            free(username);
            return -1;
        }

        if (read(sd_client, (char*) &len_alias, sizeof(int)) < 0) {
            perror("Error al leer la longitud del alias");
            return -1;
        }

        len_alias = ntohl(len_alias);
        alias = malloc(len_alias + 1);
        if (read(sd_client, (char*) alias, strlen(alias)) < 0) {
            perror("Error al leer el alias");
            free(alias);
            return -1;
        }


        if (read(sd_client, (char*) &len_date, sizeof(int)) < 0) {
            perror("Error al leer la longitud de la fecha");
            return -1;
        }
        
        len_date = ntohl(len_date);
        date = malloc(len_date + 1);
        if (read(sd_client, (char*) date, sizeof(date)) < 0) {
            perror("Error al leer la fecha");
            free(date);
            return -1;
        }

        if (read(sd_client, (char*) &c_op, sizeof(int)) < 0) {
            perror("Error al leer la respuesta");
            return -1;
        }

        c_op = ntohl(c_op);


        //se rellena la estructura de la petición
        strcpy(perfil.nombre, username);
        strcpy(perfil.alias, alias);
        strcpy(perfil.fecha, date);
        perfil.c_op = c_op;
        perfil.sd_client = sd_client;

 
        if (pthread_create(&thid, &t_attr, (void *)tratar_mensaje, (void *)&perfil) == 0) {
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