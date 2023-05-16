# Proyecto-Distribuidos


## Compilación
Para la compilación del programa se dispone de un fichero Makefile que permite crear un ejecutable para el servidor. Una vez se ha creado el ejecutable del servidor, se puede lanzar de la siguiente manera.
```C
:~$ ./server <port>
```
Dónde <port> es el puerto en el que se quiere ejecutar el cliente. Seguidamente, para lanzar el cliente se debe poner el siguiente comando.

```C
:~$ python3 ./client.py -s <localhost> -p <port>
```
Dónde <port> debe ser el mismo puerto que el indicado para el cliente.

Además, como se ha implementado el servicio Web, será necesario lanzarlo para poder hacer un uso completo de la aplicación de mensajería. Para ello, debemos lanzar las siguientes líneas en otra terminal diferente.

```C
:~$ python3 ./ws-server.py
:~$ python3 -mzeep http://localhost:5000/?wsdl
```

