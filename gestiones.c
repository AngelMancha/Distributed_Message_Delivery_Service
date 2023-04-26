
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
    char *alias = malloc(strlen(perfil.alias));

    sprintf(nombre_fichero, "%s%s%s", peticion_root, alias, formato_fichero);
    
    // Comprobamos la existencia de la clave

    if (access(nombre_fichero, F_OK) == 0) 
    {
        perror("set_value(): La clave existe");
        return -1;
    }

    fichero_perfil = fopen(nombre_fichero, "wb");

    if (fichero_perfil != NULL) 
    {
        fwrite(&perfil, sizeof(struct perfil), 1, fichero_perfil); 
        fclose(fichero_perfil);  
    } else 
    {
        printf("Registro(): No se pudo abrir el archivo.\n");
        return -1;
    }

    free(alias);
    return 0;
}

