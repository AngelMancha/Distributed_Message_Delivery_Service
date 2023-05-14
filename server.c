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
#include "gestiones.h"
#include "comunicacion.h"
#include "funciones_server.h"
#include "leer_campos.h"



#define MAXSIZE 1024


struct sockaddr_in address_thread; // direccion IP y Puerto Hilo
socklen_t addrlen_thread = sizeof(address_thread);

//mutex
pthread_mutex_t mutex_mensaje;
int mensaje_no_copiado;
pthread_cond_t cond_mensaje;

int envio_mensajes_pendientes(char *alias, char *IP, int port) {
    struct hostent *hp;
    char send_message[MAXSIZE] = "";
    strcat(send_message, "SEND_MESSAGE");

    int contador = num_mensajes_pendientes(alias);

    //obtener tods los mensajes pendientes y enviarlos
    char **cadena_pendientes = extraerMensajes(alias, contador);
    for (int i = 0; i < contador; i++) {
        
        //Se crea un socket por cada conexión
        int socket_thread = socket(AF_INET, SOCK_STREAM, 0);
        if (socket_thread == -1) {
            perror("Error al crear el socket\n");
            return 3; //MIRAAAAR
        }
        struct sockaddr_in thread_addr;
        hp = gethostbyname(IP);
        memcpy(&(thread_addr.sin_addr), hp->h_addr, hp->h_length);

        thread_addr.sin_family = AF_INET;
        thread_addr.sin_port = htons(port);
        printf(">>>>>>> %d\n", thread_addr.sin_port) ;
        
        
        int cod = connect(socket_thread, (struct sockaddr *)&thread_addr, sizeof(thread_addr));
        if (cod < 0) {
            perror("Error al conectar con el servidor\n");
            return 3; //MIRAAAAR
        }

        // ************ SEND_MESSAGE ************
        if (sendMessage(socket_thread, send_message, strlen(send_message)+1) < 0)
        {
            perror("write: ");
            return 3;
        }

        // // ************ ALIAS ************
        if (sendMessage(socket_thread, alias, strlen(alias)+1) < 0)
        {
            perror("write: ");
            return 3;
        }

        // // ************ ALIAS ************
        char *id = malloc(sizeof(int));
        sprintf(id, "%d", i);
        if (sendMessage(socket_thread, id, strlen(id)+1) < 0)
        {
            perror("write: ");
            return 3;
        }
        free(id);
        
        // ************ ENVIAR MENSAJE ************
        if (sendMessage(socket_thread, cadena_pendientes[i], strlen(cadena_pendientes[i])+1) < 0)
        {
            perror("write: ");
            return 3;
        }

        free(cadena_pendientes[i]);
        close(socket_thread);
    }
    free(cadena_pendientes);
    // close socket
    

    return 0;

}


int recibir_msj_socket(int sd_client, struct perfil *perfil, char *alias_dest, char *message) {
   
    char username [MAXSIZE];
    char alias [MAXSIZE];
    char date [MAXSIZE];
    char c_op[MAXSIZE];
    int res = 0;

    // *********** OPERATION CODE ***********
    if (readLine(sd_client, (char*) &c_op, MAXSIZE) < 0) {
        perror("Error al leer la respuesta");
        res = 3;
    }

    perfil->c_op = malloc(MAXSIZE);
    strcpy(perfil->c_op, c_op);

    // ************** REGISTER **************
    if (strcmp(c_op, "REGISTER")==0)
    {
        res = read_username(sd_client, username, perfil);
        res = read_alias(sd_client, alias);
        res = read_date(sd_client, date, perfil);

         // Reserva de memoria
        perfil->alias = malloc(MAXSIZE);
        strcpy(perfil->alias, alias);

    // ************* UNREGISTER *************
    } else if (strcmp(c_op, "UNREGISTER")==0) {

        res = read_alias(sd_client, alias);
        perfil->alias = malloc(MAXSIZE);
        strcpy(perfil->alias, alias);


    // ************** CONNECT **************
    } else if (strcmp(c_op, "CONNECT")==0) {

        char port[MAXSIZE];

        getpeername(sd_client, (struct sockaddr *)&address_thread, &addrlen_thread);
        char ipstr[INET_ADDRSTRLEN]; // pasar la ip a un formato legible
        inet_ntop(AF_INET, &(address_thread.sin_addr), ipstr, INET_ADDRSTRLEN);

        res = read_alias(sd_client, alias);
        res = read_port(sd_client, port);
        
        // Reserva de memoria
        perfil->alias = malloc(MAXSIZE);
        perfil->IP = malloc(MAXSIZE);
        strcpy(perfil->alias, alias);
        perfil->port = atoi(port);
        strcpy(perfil->IP, ipstr);


    // ************** DISCONNECT ************
    } else if (strcmp(c_op, "DISCONNECT")==0){

        res = read_alias(sd_client, alias);
        perfil->alias = malloc(MAXSIZE);
        strcpy(perfil->alias, alias);


    // ************** SEND **************
    } else if (strcmp(c_op, "SEND") == 0) { 

        // ********** ALIAS USER ***********
        res = read_alias(sd_client, alias);
        // ********** ALIAS DEST ***********
        res = read_alias(sd_client, alias_dest);
        // ********** MESSAGE ***********
        res = read_message(sd_client, message);

        // Reserva de memoria
        perfil->IP = malloc(MAXSIZE);
        strcpy(perfil->alias, alias);
}

    return res;
}

void tratar_mensaje(void *sd_client_tratar) 
{   		
    struct respuesta respuesta;	    //respuesta a la petición          
    int resultado;		            // resultado de la operación 
    int sd_client;
    struct perfil perfil;
    char IP[MAXSIZE];
    int port;

    // Para el posible envío de mensajes
    char alias_dest[MAXSIZE];
    char message[MAXSIZE];
    
    pthread_mutex_lock(&mutex_mensaje);

    sd_client = (* (int *) sd_client_tratar);
    //Como ya se ha copiado el mensaje, despetarmos al servidor 
    mensaje_no_copiado = false;
    pthread_cond_signal(&cond_mensaje);
	pthread_mutex_unlock(&mutex_mensaje);

    resultado = recibir_msj_socket(sd_client, &perfil, alias_dest, message);

    if (strcmp(perfil.c_op, "REGISTER") == 0) {
        perfil.status = "Desconectado";
        perfil.port = 0;
        resultado = register_gestiones(perfil);
        // liberar memoria
        
    
    } else if (strcmp(perfil.c_op, "UNREGISTER") == 0) {
        resultado = unregister_gestiones(perfil);
        // liberar memoria

        
    } else if (strcmp(perfil.c_op, "CONNECT") == 0) {
        resultado = connect_gestiones(perfil);

        //Comprobamos si tiene mensajes pendientes y si es así se los enviamos
        int connected = is_connected(perfil.alias, &port, IP);
        if (connected == 0 && num_mensajes_pendientes(perfil.alias) > 0) 
        {
            resultado = envio_mensajes_pendientes(perfil.alias, IP, port);

        }

       


        // liberar memoria


    }else if (strcmp(perfil.c_op, "DISCONNECT")==0){
        resultado = disconnect_gestiones(perfil);
        // liberar memoria

        
    } else if (strcmp(perfil.c_op, "SEND") == 0) {
        
        resultado = send_to_server_gestiones(perfil, alias_dest, message);
        int connected = is_connected(alias_dest, &port, IP);

        // Enviar mensajes
        if (connected == 0 && num_mensajes_pendientes(alias_dest) > 0) {
            resultado = envio_mensajes_pendientes(alias_dest, IP, port);
            
            int port_remitente;
            char IP_remitente[MAXSIZE];
            struct hostent *hp;
            char send_message[MAXSIZE] = "";
            is_connected(perfil.alias, &port_remitente, IP_remitente);
            
            //Se crea un socket por cada conexión
            int socket_thread = socket(AF_INET, SOCK_STREAM, 0);
            if (socket_thread == -1) {
                perror("Error al crear el socket\n");
                resultado = 3; //MIRAAAAR
            }
            struct sockaddr_in thread_addr;
            hp = gethostbyname(IP_remitente);
            memcpy(&(thread_addr.sin_addr), hp->h_addr, hp->h_length);

            thread_addr.sin_family = AF_INET;
            thread_addr.sin_port = htons(port_remitente);
            printf(">>>>>>> %d\n", thread_addr.sin_port) ;
            
            
            int cod = connect(socket_thread, (struct sockaddr *)&thread_addr, sizeof(thread_addr));
            if (cod < 0) {
                perror("Error al conectar con el servidor del hilo del remitente\n");
                resultado = 3; //MIRAAAAR
            }

            // ************ SEND_MESSAGE_ACK ************
            strcat(send_message, "SEND_MESS_ACK");
            if (sendMessage(socket_thread, send_message, strlen(send_message)+1) < 0)
            {
                perror("write: ");
                resultado = 3;
            }
            close(socket_thread);
        }

        

        // liberar memoria


    } else {
        printf("Error: código de operación no válido.\n");
        exit(-1);
    }

    respuesta.code_error = resultado;
    
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