
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

    fichero_perfil = fopen(nombre_fichero, "wb");
    fichero_mensajes = fopen(nombre_mensajes_pendientes, "wb");

    if (fichero_perfil != NULL) 
    {
        fwrite(&perfil, sizeof(struct perfil), 1, fichero_perfil); 
        fclose(fichero_perfil);  
    } else 
    {
        printf("Registro(): No se pudo abrir el archivo.\n");
        return 2;
    }

    if (fichero_mensajes != NULL) 
    {
        fclose(fichero_mensajes);  
    } else 
    {
        printf("Registro(): No se pudo abrir el archivo.\n");
        return 2;
    }

    printf("El usuario %s se ha registrado con éxito\n", perfil.alias);
   
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
        return 1;
    }
    
    //eliminamos el fichero
    if (remove(nombre_fichero) != 0) 
    {
        perror("Ha ocurrido un error al dar de baja al usuario");
        return 2;
    } 

    printf("Usuario %s dado de baja con éxito\n", perfil.alias);

    
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
        return 1;
    }


    FILE *archivo = fopen(nombre_fichero, "r+b");
    
    if (archivo == NULL) 
    {
        perror("Modify_value_impl(): Error al abrir el archivo\n");
        return 3;
    }

    // Mover el puntero de posición al inicio del archivo
    fseek(archivo, 0, SEEK_SET);

    // Leer el registro original
    struct perfil perfil_antiguo;
    fread(&perfil_antiguo, sizeof(struct perfil), 1, archivo);

    if (strcmp(perfil_antiguo.status, "Conectado") == 0){
        perror("El usuario ya está conectado");
        return 2;
    }
    //modificar el registro 
    perfil.sd_client = perfil_antiguo.sd_client;
    perfil.nombre = perfil_antiguo.nombre;
    perfil.fecha = perfil_antiguo.fecha;
    perfil.c_op = perfil_antiguo.c_op;
    perfil.status = "Conectado";

    // Mover el puntero de posición al inicio del archivo
    fseek(archivo, 0, SEEK_SET);

    // Escribir el nuevo registro
    fwrite(&perfil, sizeof(struct perfil), 1, archivo);

    fclose(archivo);


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
        return 1;
    }


    FILE *archivo = fopen(nombre_fichero, "r+b");
    
    if (archivo == NULL) 
    {
        perror("Modify_value_impl(): Error al abrir el archivo\n");
        return 3;
    }

    // Mover el puntero de posición al inicio del archivo
    fseek(archivo, 0, SEEK_SET);

    // Leer el registro original
    struct perfil perfil_antiguo;
    fread(&perfil_antiguo, sizeof(struct perfil), 1, archivo);

    if (strcmp(perfil_antiguo.status, "Desconectado") == 0){
        perror("El usuario no está conectado");
        return 2;
    }
    //modificar el registro 
    perfil.sd_client = perfil_antiguo.sd_client;
    perfil.nombre = perfil_antiguo.nombre;
    perfil.fecha = perfil_antiguo.fecha;
    perfil.c_op = perfil_antiguo.c_op;
    perfil.status = "Desconectado";

    // Mover el puntero de posición al inicio del archivo
    fseek(archivo, 0, SEEK_SET);

    // Escribir el nuevo registro
    fwrite(&perfil, sizeof(struct perfil), 1, archivo);

    fclose(archivo);


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
        perror("El usuario no está conectado");
        return 1;
    }

    *port = perfil.port;
    strcpy(IP,perfil.IP);
    return 0;

}

int obtener_ultimo_id(FILE *archivo, int num_mensajes) {
    struct mensaje mensaje;
    int ultimo_id;
    for (int i = 0; i < num_mensajes; i++) {
        if (i == num_mensajes) {
        fread(&mensaje, sizeof(struct mensaje), 1, archivo);
        }
        ultimo_id = mensaje.id;
    }
    
    return ultimo_id;

}

int send_to_server_gestiones(struct perfil perfil, char *destinatario, char *mensaje){
    //Esta función modifica el fichero que representa la clave key con los nuevos valores
    char nombre_fichero[50];
    char nombre_fichero_dest[50];
    int ultimo_id;
    struct mensaje mensaje_nuevo;
    int num_mensajes;
    
    //struct tupla_pet pet;
    sprintf(nombre_fichero, "%s%s%s", peticion_root, perfil.alias, extension_mensajes);
    // Comprobamos la existencia del usuario origen
    if (access(nombre_fichero, F_OK) != 0) 
    {
        perror("Error: El usuario remitente no existe");
        return 1;
    }
    sprintf(nombre_fichero_dest, "%s%s%s", peticion_root, destinatario, extension_mensajes);

    // Comprobamos la existencia del usuario destino
    if (access(nombre_fichero_dest, F_OK) != 0) 
    {
        perror("Error: El usuario destinatario no existe\n");
        return 1;
    }

    //abrimos el fichero que contiene los mensajes pendientes del usuario destinatario
    FILE *archivo = fopen(nombre_fichero_dest, "r+b");
    if (archivo == NULL) 
    {
        perror("send(): Error al abrir los mensajes pendientes del destinatario \n");
        return 2;
    }

    // Mover el puntero de posición al final del archivo
    fseek(archivo, 0, SEEK_END);

    //se crea el mensaje nuevo y se añade al array de mensajes pendientes del destinatario
    
    num_mensajes = num_mensajes_pendientes(perfil.alias);
    if (num_mensajes == 0) {
        ultimo_id = -1;
    }
    else {
        ultimo_id = obtener_ultimo_id(archivo, num_mensajes);
    }
    
    mensaje_nuevo.id = ultimo_id + 1;
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