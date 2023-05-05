
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include "gestiones.h"
#define peticion_root "./DDBB/" // raiz para coger los ficheros
#define formato_fichero ".dat"   

int register_gestiones(struct perfil perfil){
    
    FILE *fichero_perfil;
    char nombre_fichero[50];

    sprintf(nombre_fichero, "%s%s%s", peticion_root, perfil.alias, formato_fichero);
    
    // Comprobamos la existencia de la clave

    if (access(nombre_fichero, F_OK) == 0) 
    {
        perror("register_gestiones(): El usuario ya existe");
        return 1;
    }

    perfil.status = malloc(MAXSIZE);
    strcpy(perfil.status, "Desconectado");

    fichero_perfil = fopen(nombre_fichero, "wb");

    if (fichero_perfil != NULL) 
    {
        fwrite(&perfil, sizeof(struct perfil), 1, fichero_perfil); 
        fclose(fichero_perfil);  
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
        return -1;
    }

    FILE *archivo = fopen(nombre_fichero, "r+b");
    
    if (archivo == NULL) 
    {
        perror("Modify_value_impl(): Error al abrir el archivo\n");
        return -1;
    }

    // Mover el puntero de posición al inicio del archivo
    fseek(archivo, 0, SEEK_SET);

    // Leer el registro original
    struct perfil perfil_original;
    fread(&perfil_original, sizeof(struct perfil), 1, archivo);

    // Mover el puntero de posición al inicio del archivo
    fseek(archivo, 0, SEEK_SET);

    // Escribir el nuevo registro
    fwrite(&perfil, sizeof(struct perfil), 1, archivo);

    fclose(archivo);


    return 0;
}