
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include "gestiones.h"
#define peticion_root "./DDBB/" // raiz para coger los ficheros
#define formato_fichero ".dat"   
#define extension_mensajes "_mensajes.dat"

int register_gestiones(struct perfil perfil){
    
    FILE *fichero_perfil;
    FILE *fichero_mensajes;	
    char nombre_fichero[50];
    char nombre_mensajes_pendientes[50];

    sprintf(nombre_fichero, "%s%s%s", peticion_root, perfil.alias, formato_fichero);
    sprintf(nombre_mensajes_pendientes, "%s%s%s", peticion_root, perfil.alias, extension_mensajes);
    
    // Comprobamos la existencia de la clave

    if (access(nombre_fichero, F_OK) == 0) 
    {
        perror("register_gestiones(): El usuario ya existe");
        return 1;
    }


    perfil.status = malloc(MAXSIZE);
    strcpy(perfil.status, "Desconectado");
    perfil.last_id = 0;

    fichero_perfil = fopen(nombre_fichero, "wb");
    fichero_mensajes = fopen(nombre_mensajes_pendientes, "wb");

    if (fichero_perfil != NULL) 
    {
        fwrite(&perfil, sizeof(struct perfil), 1, fichero_perfil); 
        fclose(fichero_perfil);  
    } else 
    {
        perror("Registro(): No se pudo abrir el archivo.\n");
        dprintf(2, "s> REGISTER %s FAIL\n", perfil.alias);
        
        return 2;
    }

    if (fichero_mensajes != NULL) 
    {
        fclose(fichero_mensajes);  
    } else 
    {
        printf("Registro(): No se pudo abrir el archivo.\n");
        dprintf(2, "s> REGISTER %s FAIL\n", perfil.alias);

        return 2;
    }

    dprintf(2, "s> REGISTER %s OK\n", perfil.alias);
    return 0;
}


int unregister_gestiones(struct perfil perfil)
{
    char nombre_fichero[50];

    sprintf(nombre_fichero, "%s%s%s", peticion_root, perfil.alias, formato_fichero);

    // Comprobamos si hay existencia del fichero
    if (access(nombre_fichero, F_OK) != 0) 
    {
        perror("unregister_gestiones(): La clave no existe");
        dprintf(2, "s> UNREGISTER %s FAIL\n", perfil.alias);
        return 1;
    }
    
    //eliminamos el fichero
    if (remove(nombre_fichero) != 0) 
    {
        perror("Ha ocurrido un error al dar de baja al usuario");
        dprintf(2, "s> UNREGISTER %s FAIL\n", perfil.alias);
        return 2;
    } 


    dprintf(2, "s> UNREGISTER %s OK\n", perfil.alias);
    return 0;

}


int connect_gestiones(struct perfil perfil) 
{
    //Esta función modifica el fichero que representa la clave key con los nuevos valores
    char str_key[20];
    char nombre_fichero[50];
    //struct tupla_pet pet;

    sprintf(str_key, "%s", perfil.alias);
    sprintf(nombre_fichero, "%s%s%s", peticion_root, str_key, formato_fichero);

    // Comprobamos la existencia de la clave
    if (access(nombre_fichero, F_OK) != 0) 
    {
        perror("modify_value(): El usuario no existe");
        dprintf(2, "s> CONNECT %s FAIL\n", perfil.alias);
        return 1;
    }


    FILE *archivo = fopen(nombre_fichero, "r+b");
    
    if (archivo == NULL) 
    {
        perror("Modify_value_impl(): Error al abrir el archivo\n");
        dprintf(2, "s> CONNECT %s FAIL\n", perfil.alias);
        return 3;
    }

    // Mover el puntero de posición al inicio del archivo
    fseek(archivo, 0, SEEK_SET);

    // Leer el registro original
    struct perfil perfil_antiguo;
    fread(&perfil_antiguo, sizeof(struct perfil), 1, archivo);

    if (strcmp(perfil_antiguo.status, "Conectado") == 0){
        perror("El usuario ya está conectado");
        dprintf(2, "s> CONNECT %s FAIL\n", perfil.alias);
        return 2;
    }
    //modificar el registro 
    perfil.sd_client = perfil_antiguo.sd_client;
    perfil.nombre = perfil_antiguo.nombre;
    perfil.fecha = perfil_antiguo.fecha;
    perfil.c_op = perfil_antiguo.c_op;
    perfil.last_id = perfil_antiguo.last_id;
    perfil.status = "Conectado";

    // Mover el puntero de posición al inicio del archivo
    fseek(archivo, 0, SEEK_SET);

    // Escribir el nuevo registro
    fwrite(&perfil, sizeof(struct perfil), 1, archivo);

    fclose(archivo);

    dprintf(2, "s> CONNECT %s OK\n", perfil.alias);
    return 0;
}


int disconnect_gestiones(struct perfil perfil) 
{
    //Esta función modifica el fichero que representa la clave key con los nuevos valores
    char str_key[20];
    char nombre_fichero[50];
    //struct tupla_pet pet;

    sprintf(str_key, "%s", perfil.alias);
    sprintf(nombre_fichero, "%s%s%s", peticion_root, str_key, formato_fichero);

    // Comprobamos la existencia de la clave
    if (access(nombre_fichero, F_OK) != 0) 
    {
        perror("modify_value(): El usuario no existe");
        dprintf(2, "s> DISCONNECT %s FAIL\n", perfil.alias);
        return 1;
    }


    FILE *archivo = fopen(nombre_fichero, "r+b");
    
    if (archivo == NULL) 
    {
        perror("Modify_value_impl(): Error al abrir el archivo\n");
        dprintf(2, "s> DISCONNECT %s FAIL\n", perfil.alias);
        return 3;
    }

    // Mover el puntero de posición al inicio del archivo
    fseek(archivo, 0, SEEK_SET);

    // Leer el registro original
    struct perfil perfil_antiguo;
    fread(&perfil_antiguo, sizeof(struct perfil), 1, archivo);

    if (strcmp(perfil_antiguo.status, "Desconectado") == 0){
        perror("El usuario ya está desconectado");
        dprintf(2, "s> DISCONNECT %s FAIL\n", perfil.alias);
        return 2;
    }
    //modificar el registro 
    perfil.sd_client = perfil_antiguo.sd_client;
    perfil.nombre = perfil_antiguo.nombre;
    perfil.fecha = perfil_antiguo.fecha;
    perfil.c_op = perfil_antiguo.c_op;
    perfil.last_id = perfil_antiguo.last_id;
    perfil.status = "Desconectado";

    // Mover el puntero de posición al inicio del archivo
    fseek(archivo, 0, SEEK_SET);

    // Escribir el nuevo registro
    fwrite(&perfil, sizeof(struct perfil), 1, archivo);

    fclose(archivo);

    dprintf(2, "s> DISCONNECT %s OK\n", perfil.alias);
    return 0;
}

int is_connected(char* destinatario, int * port, char *IP)
{   

    char nombre_fichero[50];

    sprintf(nombre_fichero, "%s%s%s", peticion_root, destinatario, formato_fichero);

    FILE *archivo = fopen(nombre_fichero, "r+b");
    
    if (archivo == NULL) 
    {
        perror("Error al abrir el archivo\n");
        return 3;
    }

    // Mover el puntero de posición al inicio del archivo
    fseek(archivo, 0, SEEK_SET);

    // Leer el registro
    struct perfil perfil;
    fread(&perfil, sizeof(struct perfil), 1, archivo);

    if (strcmp(perfil.status, "Conectado") != 0){
       
        return 1;
    }

    *port = perfil.port;
    strcpy(IP,perfil.IP);
    return 0;

}

int obtener_ultimo_id(char*alias) {
    char nombre_fichero_dest_perfil[50];
    struct perfil perfil_dest;
    //char nombre_fichero[50];
    int last_id; 

    sprintf(nombre_fichero_dest_perfil, "%s%s%s", peticion_root, alias, formato_fichero);
    //dprintf(2, "El nombre del fichero es: %s\n", nombre_fichero);
    // Comprobamos la existencia del usuario origen
    if (access(nombre_fichero_dest_perfil, F_OK) != 0) 
    {
        perror("Error: El usuario remitente no existe");
        return 1;
    }

    FILE *archivo_perfil = fopen(nombre_fichero_dest_perfil, "r+b");
    if (archivo_perfil == NULL) 
    {
        perror("send(): Error al abrir los mensajes pendientes del remitente \n");
        return 2;
    }

    fread(&perfil_dest, sizeof(struct perfil), 1, archivo_perfil);

    last_id = perfil_dest.last_id;
    fclose(archivo_perfil);
    
    return last_id;

}

int send_to_server_gestiones(struct perfil perfil, char *destinatario, char *mensaje){
    //Esta función modifica el fichero que representa la clave key con los nuevos valores

    char nombre_fichero[50];
    char nombre_fichero_dest_msg[50];
    char nombre_fichero_dest_perf[50];

    struct mensaje mensaje_nuevo;
    struct perfil perfil_dest;
   
    
    //struct tupla_pet pet;
    sprintf(nombre_fichero, "%s%s%s", peticion_root, perfil.alias, extension_mensajes);
    // Comprobamos la existencia del usuario origen
    if (access(nombre_fichero, F_OK) != 0) 
    {
        perror("Error: El usuario remitente no existe");
        return 1;
    }
    sprintf(nombre_fichero_dest_msg, "%s%s%s", peticion_root, destinatario, extension_mensajes);

    // Comprobamos la existencia del usuario destino
    if (access(nombre_fichero_dest_msg, F_OK) != 0) 
    {
        perror("Error: El usuario destinatario msg no existe\n");
        return 1;
    }

    sprintf(nombre_fichero_dest_perf, "%s%s%s", peticion_root, destinatario, formato_fichero);

    // Comprobamos la existencia del usuario destino
    if (access(nombre_fichero_dest_perf, F_OK) != 0) 
    {
        perror("Error: El usuario destinatario perfil no existe\n");
        return 1;
    }

    FILE *archivo_perfil = fopen(nombre_fichero_dest_perf, "r+b");
    if (archivo_perfil == NULL) 
    {
        perror("send(): Error al abrir los mensajes pendientes del remitente \n");
        return 2;
    }

    fread(&perfil_dest, sizeof(struct perfil), 1, archivo_perfil);
    

    mensaje_nuevo.id = perfil_dest.last_id + 1;
    perfil_dest.last_id = perfil_dest.last_id + 1;

    //dprintf(2, "\n\n\n\n\nEL ID DEL MENSAJE ES: %d\n\n\n\n\n", mensaje_nuevo.id);

    // Mover el puntero de posición al inicio del archivo
    fseek(archivo_perfil, 0, SEEK_SET);

    // Escribir el nuevo registro
    fwrite(&perfil_dest, sizeof(struct perfil), 1, archivo_perfil);

    fclose(archivo_perfil);



    //abrimos el fichero que contiene los mensajes pendientes del usuario destinatario
    FILE *archivo = fopen(nombre_fichero_dest_msg, "r+b");
    if (archivo == NULL) 
    {
        perror("send(): Error al abrir los mensajes pendientes del destinatario \n");
        return 2;
    }

    // Mover el puntero de posición al final del archivo
    fseek(archivo, 0, SEEK_END);


 
    strcpy(mensaje_nuevo.mensaje, mensaje);
    strcpy(mensaje_nuevo.remitente, perfil.alias);
    

    fwrite(&mensaje_nuevo, sizeof(struct mensaje), 1, archivo);

    fclose(archivo);


    return 0;

}



int num_mensajes_pendientes(char *destinatario){
    char nombre_fichero[50];
    sprintf(nombre_fichero, "%s%s%s", peticion_root, destinatario, extension_mensajes);


    FILE *fp = fopen(nombre_fichero, "rb");
    if (fp == NULL) {
        printf("Error al abrir el archivo");
        exit(1);
    }
    
    int contador = 0;
    struct mensaje mensaje;
    while (fread(&mensaje, sizeof(struct mensaje), 1, fp) == 1) {
        contador++;
    }

    fclose(fp);
    return contador;
}

char **extraerMensajes(char *destinatario, int numMensajes) {

    char nombre_fichero[50];
    sprintf(nombre_fichero, "%s%s%s", peticion_root, destinatario, extension_mensajes);

    FILE *fp = fopen(nombre_fichero, "rb");
    if (fp == NULL) {
        printf("Error al abrir el archivo");
        exit(1);
    }

    // Crear un array de strings para almacenar los mensajes
    char **mensajes = malloc(numMensajes * sizeof(char *));
    if (mensajes == NULL) {
        printf("Error al asignar memoria");
        exit(1);
    }
    
    // Leer cada estructura y extraer el campo "mensaje"
    struct mensaje mensaje;
    for (int i = 0; i < numMensajes; i++) {
        fread(&mensaje, sizeof(struct mensaje), 1, fp);
        mensajes[i] = malloc(strlen(mensaje.mensaje) + 1);
        if (mensajes[i] == NULL) {
            printf("Error al asignar memoria");
            exit(1);
        }
        strcpy(mensajes[i], mensaje.mensaje);
    }
    fclose(fp);
    
    // eliminar contenido
    FILE *fp_remove = fopen(nombre_fichero, "wb");
    fclose(fp_remove);

    // y devuelve un puntero a un puntero a puntero que apunta a un array de strings donde
    // están los mensjaes del destinatario
    return mensajes;
}


char **extraerRemitentes(char *destinatario, int numMensajes) {

    char nombre_fichero[50];
    sprintf(nombre_fichero, "%s%s%s", peticion_root, destinatario, extension_mensajes);

    FILE *fp = fopen(nombre_fichero, "rb");
    if (fp == NULL) {
        printf("Error al abrir el archivo");
        exit(1);
    }

    // Crear un array de strings para almacenar los mensajes
    char **remitentes = malloc(numMensajes * sizeof(char *));
    if (remitentes == NULL) {
        printf("Error al asignar memoria");
        exit(1);
    }
    
    // Leer cada estructura y extraer el campo "mensaje"
    struct mensaje mensaje;
    for (int i = 0; i < numMensajes; i++) {
        fread(&mensaje, sizeof(struct mensaje), 1, fp);

        remitentes[i] = malloc(strlen(mensaje.remitente) + 1);
        if (remitentes[i] == NULL) {
            printf("Error al asignar memoria");
            exit(1);
        }
        strcpy(remitentes[i], mensaje.remitente);
    }
    fclose(fp);
    
    // y devuelve un puntero a un puntero a puntero que apunta a un array de strings donde
    // están los mensjaes del destinatario
    return remitentes;
}



// void print_last_id(char *destinatario){
//     struct perfil perfil_dest;
//     char nombre_fichero_dest_perf[50];
//     sprintf(nombre_fichero_dest_perf, "%s%s%s", peticion_root, destinatario, formato_fichero);

//     // Comprobamos la existencia del usuario destino
//     if (access(nombre_fichero_dest_perf, F_OK) != 0) 
//     {
//         perror("Error: El usuario destinatario perfil no existe\n");

//     }

//     FILE *archivo_perfil = fopen(nombre_fichero_dest_perf, "r+b");
//     if (archivo_perfil == NULL) 
//     {
//         perror("send(): Error al abrir los mensajes pendientes del remitente \n");
       
//     }

//     fread(&perfil_dest, sizeof(struct perfil), 1, archivo_perfil);
//     dprintf(2, "\n\n\nEL ID DEL PERFIL DEST ES: %d\n\n\n", perfil_dest.last_id);

//     fclose(archivo_perfil);


// }



int *extraerIDs(char *destinatario, int numMensajes) {

    char nombre_fichero[50];
    sprintf(nombre_fichero, "%s%s%s", peticion_root, destinatario, extension_mensajes);

    FILE *fp = fopen(nombre_fichero, "rb");
    if (fp == NULL) {
        printf("Error al abrir el archivo");
        exit(1);
    }

    // Crear un array de enteros para almacenar los IDs
    int *ids = malloc(numMensajes * sizeof(int));
    if (ids == NULL) {
        printf("Error al asignar memoria");
        exit(1);
    }
    
    // Leer cada estructura y extraer el campo "id"
    struct mensaje mensaje;
    for (int i = 0; i < numMensajes; i++) {
        fread(&mensaje, sizeof(struct mensaje), 1, fp);
        //dprintf(2, "EL REMITENTE DEL MENSAJEEEEEEEEEEEE ES: %d\n", mensaje.id);
        ids[i] = mensaje.id;

    }
    fclose(fp);
    
    // Devuelve un puntero a un array de enteros donde están los IDs de las estructuras
    return ids;
}
