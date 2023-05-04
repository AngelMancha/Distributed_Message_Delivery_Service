import subprocess
import sys
import PySimpleGUI as sg
from enum import Enum
import argparse
import pickle
import socket
import sys


class client :

    # ******************** TYPES *********************
    # *
    # * @brief Return codes for the protocol methods
    class RC(Enum) :
        OK: int = 0
        ERROR: int = 1
        USER_ERROR: int = 2

    # ****************** ATTRIBUTES ******************
    _server = None
    _port = -1
    _quit: int = 0
    _username: str = None
    _alias: str = None
    _date: str = None

    # ******************** METHODS *******************
    # *
    # * @param user - User name to register in the system
    # *
    # * @return OK if successful
    # * @return USER_ERROR if the user is already registered
    # * @return ERROR if another error occurred
    @staticmethod
    def  register(user, window):
        c_op = 0
        # sockets para conectar con servidor

        # Se comprueba que se hayan pasado los argumentos necesarios
        if len(sys.argv) < 3:
            print("Uso: client_register <host> <port>")
            exit()
        
        # Creamos el socket
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

        # Nos conectamos al servidor a través de los argumentos
        server_address = (client._server, client._port)
        print('connecting to {} port {}'.format(*server_address))
        sock.connect(server_address)
        
        # Enviamos la info al servidor
        try:
            ############ ALIAS ############
            sock.sendall(client._alias.encode("utf-8"))
            sock.sendall(b'\0')    

            ############ USERNAME ############
            sock.sendall(client._username.encode("utf-8"))
            sock.sendall(b'\0')
            
            ############ DATE ############
            sock.sendall(client._date.encode("utf-8"))
            sock.sendall(b'\0')
            
            ############ OP. CODE ############
            c_op = "REGISTER"
            sock.sendall(c_op.encode("utf-8"))
            sock.sendall(b'\0')
            
            ############ PORT ############
            sock.sendall(str(client._port).encode("utf-8"))
            sock.sendall(b'\0')
            
        except socket.error as e:
                print(f"Error al enviar los datos: {e}")
        finally:
            
            result = int.from_bytes(sock.recv(4), 'big')
            sock.close()

            #result = socket.ntohl(result)
            print("RESULT IN CLIENT " + str(result))
        # Comprobamos los results
        if result == 0:
            window['_SERVER_'].print("s> REGISTER OK")
            return client.RC.OK
        if result == 1:
            window['_SERVER_'].print("s> USERNAME IN USE")
            return client.RC.USER_ERROR
        if result == 2:
            window['_SERVER_'].print("s> REGISTER FAIL")
            return client.RC.ERROR

    # *
    # 	 * @param user - User name to unregister from the system
    # 	 *
    # 	 * @return OK if successful
    # 	 * @return USER_ERROR if the user does not exist
    # 	 * @return ERROR if another error occurred
    @staticmethod
    def  unregister(user, window):
        c_op = 0
        # sockets para conectar con servidor

        # Creamos el socket
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

        # Nos conectamos al servidor a través de los argumentos
        server_address = (client._server, client._port)
        print('connecting to {} port {}'.format(*server_address))
        sock.connect(server_address)
        
        # Enviamos la info al servidor
        try:
            ############ ALIAS ############
            sock.sendall(client._alias.encode("utf-8"))
            sock.sendall(b'\0')    

            ############ USERNAME ############
            sock.sendall(client._username.encode("utf-8"))
            sock.sendall(b'\0')
            
            ############ DATE ############
            sock.sendall(client._date.encode("utf-8"))
            sock.sendall(b'\0')
            
            ############ OP. CODE ############
            c_op = "UNREGISTER"
            sock.sendall(c_op.encode("utf-8"))
            sock.sendall(b'\0')
            
            ############ PORT ############
            sock.sendall(str(client._port).encode("utf-8"))
            sock.sendall(b'\0')
            
            
        except socket.error as e:
                print(f"Error al enviar los datos: {e}")
        finally:
            result = int.from_bytes(sock.recv(4), 'big')
            
            sock.close()

            #result = socket.ntohl(result)
            print("RESULT IN CLIENT " + str(result))
        # Comprobamos los results
        if result == 0:
            window['_SERVER_'].print("s> UNREGISTER OK")
            return client.RC.OK
        if result == 1:
            window['_SERVER_'].print("s> USERNAME DOES NOT EXIST")
            return client.RC.USER_ERROR
        if result == 2:
            window['_SERVER_'].print("s> UNREGISTER FAIL")
            return client.RC.ERROR
        


    # *
    # * @param user - User name to connect to the system
    # *
    # * @return OK if successful
    # * @return USER_ERROR if the user does not exist or if it is already connected
    # * @return ERROR if another error occurred
    @staticmethod
    def  connect(user, window):
        c_op = "CONNECT"
        # Creamos un objeto de socket
        client_socket = socket.socket()

        # Enlazamos el socket a una dirección IP y puerto libres seleccionados por el sistema operativo
        client_socket.bind(('localhost', 0))

        # Conectamos el socket al servidor y enviamos el número de puerto
        server_address = (client._server, client._port)
        client_socket.connect(server_address)
        
        try: 
            ############ ALIAS ############
            client_socket.sendall(client._alias.encode("utf-8"))
            client_socket.sendall(b'\0')    

            ############ USERNAME ############
            client_socket.sendall(client._username.encode("utf-8"))
            client_socket.sendall(b'\0')
            
            ############ DATE ############
            client_socket.sendall(client._date.encode("utf-8"))
            client_socket.sendall(b'\0')
            
            ############ CODE ############
            c_op = "CONNECT"
            client_socket.sendall(c_op.encode("utf-8"))
            client_socket.sendall(b'\0')
            
            ############ PORT ############
            # (1) Busca el primer puerto libre
            port = client_socket.getsockname()[1]
            client_socket.send(str(port).encode())

            print("Conectado al servidor en el puerto" + str(port))
        finally:
            result = int.from_bytes(client_socket.recv(4), 'big')
            
            client_socket.close()
        # Enviamos y recibimos datos con el servidor
        # data = "Hola, servidor!"
        # client_socket.sendall(data.encode())
        # response = client_socket.recv(1024).decode()
        # print(response)

        # Cerramos el socket
        client_socket.close()
        
        window['_SERVER_'].print("s> CONNECT OK")
        #  Write your code here
        return client.RC.ERROR


    # *
    # * @param user - User name to disconnect from the system
    # *
    # * @return OK if successful
    # * @return USER_ERROR if the user does not exist
    # * @return ERROR if another error occurred
    @staticmethod
    def  disconnect(user, window):
        window['_SERVER_'].print("s> DISCONNECT OK")
        #  Write your code here
        return client.RC.ERROR

    # *
    # * @param user    - Receiver user name
    # * @param message - Message to be sent
    # *
    # * @return OK if the server had successfully delivered the message
    # * @return USER_ERROR if the user is not connected (the message is queued for delivery)
    # * @return ERROR the user does not exist or another error occurred
    @staticmethod
    def  send(user, message, window):
        window['_SERVER_'].print("s> SEND MESSAGE OK")
        print("SEND " + user + " " + message)
        #  Write your code here
        return client.RC.ERROR

    # *
    # * @param user    - Receiver user name
    # * @param message - Message to be sent
    # * @param file    - file  to be sent

    # *
    # * @return OK if the server had successfully delivered the message
    # * @return USER_ERROR if the user is not connected (the message is queued for delivery)
    # * @return ERROR the user does not exist or another error occurred
    @staticmethod
    def  sendAttach(user, message, file, window):
        window['_SERVER_'].print("s> SENDATTACH MESSAGE OK")
        print("SEND ATTACH " + user + " " + message + " " + file)
        #  Write your code here
        return client.RC.ERROR

    @staticmethod
    def  connectedUsers(window):
        window['_SERVER_'].print("s> CONNECTED USERS OK")
        #  Write your code here
        return client.RC.ERROR


    @staticmethod
    def window_register():
        layout_register = [[sg.Text('Full Name:'),sg.Input('Text',key='_REGISTERNAME_', do_not_clear=True, expand_x=True)],
                            [sg.Text('Alias:'),sg.Input('Text',key='_REGISTERALIAS_', do_not_clear=True, expand_x=True)],
                            [sg.Text('Date of birth:'),sg.Input('',key='_REGISTERDATE_', do_not_clear=True, expand_x=True, disabled=True, use_readonly_for_disable=False),
                            sg.CalendarButton("Select Date",close_when_date_chosen=True, target="_REGISTERDATE_", format='%d-%m-%Y',size=(10,1))],
                            [sg.Button('SUBMIT', button_color=('white', 'blue'))]
                            ]

        layout = [[sg.Column(layout_register, element_justification='center', expand_x=True, expand_y=True)]]

        window = sg.Window("REGISTER USER", layout, modal=True)
        choice = None

        while True:
            event, values = window.read()

            if (event in (sg.WINDOW_CLOSED, "-ESCAPE-")):
                break

            if event == "SUBMIT":
                if(values['_REGISTERNAME_'] == 'Text' or values['_REGISTERNAME_'] == '' or values['_REGISTERALIAS_'] == 'Text' or values['_REGISTERALIAS_'] == '' or values['_REGISTERDATE_'] == ''):
                    sg.Popup('Registration error', title='Please fill in the fields to register.', button_type=5, auto_close=True, auto_close_duration=1)
                    continue

                client._username = values['_REGISTERNAME_']
                client._alias = values['_REGISTERALIAS_']
                client._date = values['_REGISTERDATE_']
                
                
                
                
                break
        window.Close()


    # *
    # * @brief Prints program usage
    @staticmethod
    def usage() :
        print("Usage: python3 py -s <server> -p <port>")


    # *
    # * @brief Parses program execution arguments
    @staticmethod
    def  parseArguments(argv) :
        parser = argparse.ArgumentParser()
        parser.add_argument('-s', type=str, required=True, help='Server IP')
        parser.add_argument('-p', type=int, required=True, help='Server Port')
        args = parser.parse_args()

        if (args.s is None):
            parser.error("Usage: python3 py -s <server> -p <port>")
            return False

        if ((args.p < 1024) or (args.p > 65535)):
            parser.error("Error: Port must be in the range 1024 <= port <= 65535");
            return False;

        client._server = args.s
        client._port = args.p

        return True


    def main(argv):

        if (not client.parseArguments(argv)):
            client.usage()
            exit()

        lay_col = [[sg.Button('REGISTER',expand_x=True, expand_y=True),
                sg.Button('UNREGISTER',expand_x=True, expand_y=True),
                sg.Button('CONNECT',expand_x=True, expand_y=True),
                sg.Button('DISCONNECT',expand_x=True, expand_y=True),
                sg.Button('CONNECTED USERS',expand_x=True, expand_y=True)],
                [sg.Text('Dest:'),sg.Input('User',key='_INDEST_', do_not_clear=True, expand_x=True),
                sg.Text('Message:'),sg.Input('Text',key='_IN_', do_not_clear=True, expand_x=True),
                sg.Button('SEND',expand_x=True, expand_y=False)],
                [sg.Text('Attached File:'), sg.In(key='_FILE_', do_not_clear=True, expand_x=True), sg.FileBrowse(),
                sg.Button('SENDATTACH',expand_x=True, expand_y=False)],
                [sg.Multiline(key='_CLIENT_', disabled=True, autoscroll=True, size=(60,15), expand_x=True, expand_y=True),
                sg.Multiline(key='_SERVER_', disabled=True, autoscroll=True, size=(60,15), expand_x=True, expand_y=True)],
                [sg.Button('QUIT', button_color=('white', 'red'))]
            ]


        layout = [[sg.Column(lay_col, element_justification='center', expand_x=True, expand_y=True)]]

        window = sg.Window('Messenger', layout, resizable=True, finalize=True, size=(1000,400))
        window.bind("<Escape>", "-ESCAPE-")


        while True:
            event, values = window.Read()

            if (event in (None, 'QUIT')) or (event in (sg.WINDOW_CLOSED, "-ESCAPE-")):
                sg.Popup('Closing Client APP', title='Closing', button_type=5, auto_close=True, auto_close_duration=1)
                break

            #if (values['_IN_'] == '') and (event != 'REGISTER' and event != 'CONNECTED USERS'):
             #   window['_CLIENT_'].print("c> No text inserted")
             #   continue

            if (client._alias == None or client._username == None or client._alias == 'Text' or client._username == 'Text' or client._date == None) and (event != 'REGISTER'):
                sg.Popup('NOT REGISTERED', title='ERROR', button_type=5, auto_close=True, auto_close_duration=1)
                continue

            if (event == 'REGISTER'):
                client.window_register()
                if (client._alias == None or client._username == None or client._alias == 'Text' or client._username == 'Text' or client._date == None):
                    sg.Popup('NOT REGISTERED', title='ERROR', button_type=5, auto_close=True, auto_close_duration=1)
                    continue
                window['_CLIENT_'].print('c> REGISTER ' + client._alias)
                client.register(client._alias, window)

            elif (event == 'UNREGISTER'):
                window['_CLIENT_'].print('c> UNREGISTER ' + client._alias)
                client.unregister(client._alias, window)


            elif (event == 'CONNECT'):
                window['_CLIENT_'].print('c> CONNECT ' + client._alias)
                client.connect(client._alias, window)


            elif (event == 'DISCONNECT'):
                window['_CLIENT_'].print('c> DISCONNECT ' + client._alias)
                client.disconnect(client._alias, window)


            elif (event == 'SEND'):
                window['_CLIENT_'].print('c> SEND ' + values['_INDEST_'] + " " + values['_IN_'])

                if (values['_INDEST_'] != '' and values['_IN_'] != '' and values['_INDEST_'] != 'User' and values['_IN_'] != 'Text') :
                    client.send(values['_INDEST_'], values['_IN_'], window)
                else :
                    window['_CLIENT_'].print("Syntax error. Insert <destUser> <message>")


            elif (event == 'SENDATTACH'):

                window['_CLIENT_'].print('c> SENDATTACH ' + values['_INDEST_'] + " " + values['_IN_'] + " " + values['_FILE_'])

                if (values['_INDEST_'] != '' and values['_IN_'] != '' and values['_FILE_'] != '') :
                    client.sendAttach(values['_INDEST_'], values['_IN_'], values['_FILE_'], window)
                else :
                    window['_CLIENT_'].print("Syntax error. Insert <destUser> <message> <attachedFile>")


            elif (event == 'CONNECTED USERS'):
                window['_CLIENT_'].print("c> CONNECTEDUSERS")
                client.connectedUsers(window)



            window.Refresh()

        window.Close()


if __name__ == '__main__':
    client.main([])
    print("+++ FINISHED +++")
