#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>


struct mensaje{
    char id[10];
    char mensaje[100];
};

int main(){

    struct mensaje mensajes[3];

    strcpy(mensajes[0].id, "1");
    strcpy(mensajes[0].mensaje, "Hola");
    strcpy(mensajes[1].id, "2");
    strcpy(mensajes[1].mensaje, "Adios");
    strcpy(mensajes[2].id, "2");
    strcpy(mensajes[2].mensaje, "Adios");

    int condition = 1;

    int contador = 0;
    int i=0;
    while(condition==1){
        if (mensajes[i].id != NULL) {
            printf("Elemento nÃºmero %d del array de estructuras\n", contador);
            contador++;
            
        }
        else{
            condition = 0;
        }
        
    }
}