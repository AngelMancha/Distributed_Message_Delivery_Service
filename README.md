# Distibuted Message Delivery Service

The objective of this work is to develop a message notification service between users connected to a network through the use of sockets. The functionalities that will be developed are to register users (register), unregister users (unregister), connect users to the application (connect), disconnect users (disconnect), send messages (send), and see the connected users (connected users).

The design follows a client-service structure that communicates through sockets and has the particularity of having been developed in Python (the client along with the interface) and C (the server). Furthermore, a web service has been developed in order to assist in the sending of messages.

## Demo
![visualizador](www/media/inicio.png)
![visualizador](www/media/juan.png)
![visualizador](www/media/maira.png)
![visualizador](www/media/ambos.png)
## Functionalities

- **Register**: It registers an user in the service with an alias and birth date.
- **Unregister**: It unregisters users from the service.
- **Connect**:  In order for an user to receive and send messages, it must be connected to the system.
- **Disconnect**: It disconnects from the system a connected user.
- **Send_message**: It is what allows users to communicate with other users and send them messages. If the receptor is disconnected from the system, messages will be queued, and when that user reconnects, it will receive them all.
- **Connected_users**: This function allows connected users to see in real time which and how many users are connected to the system right now.

## Deployment

```C
make
```
Since a web service has been implemented, it will need to be launched in first place in order to make full use of the messaging application. To do this, we must launch the following lines in two different consoles.

```C
python3 ./ws-server.py
```
```C
python3 -mzeep http://localhost:5000/?wsdl
```
Once the web service is running, the server is launched, specifiying which port it will be listening to.
```C
./server <port>
```
Finally, we launch as many clients as desired in different consoles, where the port is the same as the one specified in the server deployment. IP "localhost" may be changed to the desired IP
```C
python3 ./client.py -s localhost -p <port>
```


