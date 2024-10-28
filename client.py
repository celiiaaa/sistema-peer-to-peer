""" Implementación del cliente """

from enum import Enum
import argparse
import socket
import threading
import zeep


mutex = threading.Lock()

class client :

    # ******************** TYPES *********************
    # *
    # * @brief Return codes for the protocol methods
    class RC(Enum) :
        OK = 0
        ERROR = 1
        USER_ERROR = 2

    # ****************** ATTRIBUTES ******************
    _server = None
    _port = -1
    _quit = 0
    _username = ""
    _connection = False
    _listen_socket = None
    _current_users = {}

    # ******************** METHODS *******************

    @staticmethod
    def register(user) :
        """ Register a user in the system """
        cmd = "REGISTER\0"

        # Check the user name
        if user is None or len(user) > 256:
            print("c> ERROR: Invalid user name")
            return client.RC.USER_ERROR
                
        # Connect to the server
        try:
            client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            client_socket.connect((client._server, client._port))
        except Exception as e:
            print("c> ERROR: Conecting to the server:", str(e))
            return client.RC.ERROR
        
        # Send the data
        try:
            # command
            client_socket.sendall(cmd.encode("utf-8"))
            # datetime
            datetime = client.getdatetime() + "\0"
            client_socket.sendall(datetime.encode("utf-8"))
            # username
            user_send = user + "\0"
            client_socket.sendall(user_send.encode("utf-8"))
        except Exception as e:
            print("c> ERROR: Sending the request:", str(e))
            return client.RC.ERROR
        
        # Receive the response
        try:
            res = client.readline(client_socket)
        except Exception as e:
            print("c> ERROR: Receiving the response:", str(e))
            return client.RC.ERROR
        
        if res == "0":
            print("c> REGISTER OK")
        elif res == "1":
            print("c> USERNAME IN USE")
        else:
            print("c> REGISTER FAIL")
       
        # Close the connection
        try:
            client_socket.close()
        except Exception as e:
            print("Error closing the connection:", str(e))
            return client.RC.ERROR
        
        return client.RC.OK

    @staticmethod
    def unregister(user) :
        """ Unregister a user in the system """
        cmd = "UNREGISTER\0"

        # Check the user name
        if user is None or len(user) > 256:
            print("c> ERROR: Invalid user name")
            return client.RC.USER_ERROR
        
        # Connect to the server
        try:
            client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            client_socket.connect((client._server, client._port))
        except Exception as e:
            print("c> ERROR: Conecting to the server:", str(e))
            return client.RC.ERROR
        
        # Send the data
        try:
            # command
            client_socket.sendall(cmd.encode("utf-8"))
            # datetime
            datetime = client.getdatetime() + "\0"
            client_socket.sendall(datetime.encode("utf-8"))
            # username
            user_send = user + "\0"
            client_socket.sendall(user_send.encode("utf-8"))
        except Exception as e:
            print("c> ERROR: Sending the request:", str(e))
            return client.RC.ERROR
        
        # Receive the response
        try:
            res = client.readline(client_socket)
        except Exception as e:
            print("c> ERROR: Receiving the response:", str(e))
            return client.RC.ERROR
        
        if res == "0":
            print("c> UNREGISTER OK")
        elif res == "1":
            print("c> USER DOES NOT EXIST")
        else:
            print("c> UNREGISTER FAIL")
        
        # Close the connection
        try:
            client_socket.close()
        except Exception as e:
            print("Error closing the connection:", str(e))
            return client.RC.ERROR

        return client.RC.OK
    
    @staticmethod
    def connect(user) :
        """ Connect a user to the system """
        cmd = "CONNECT\0"

        # Check the user name
        if user is None or len(user) > 256:
            print("c> ERROR: Invalid user name")
            return client.RC.USER_ERROR
        
        # Check if the user is already connected
        if client._connection and user != client._username:
            print("c> ERROR: User already connected")
            return client.RC.ERROR
        
        # Create a socket for the listener, free port
        client._listen_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        try:
            client._listen_socket.bind(('0.0.0.0', 0))
            port = client._listen_socket.getsockname()[1]
        except Exception as e:
            print("c> ERROR: Creating the listener:", str(e))
            return client.RC.ERROR
        
        client._listen_socket.listen(10)

        # Create a thread listener
        global thread_client
        if not client._connection:
            client._connection = True
            thread_client = threading.Thread(target=client.connection_client, args=(port,))
            thread_client.daemon = True
            thread_client.start()

        # Connect to the server
        try:
            client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            client_socket.connect((client._server, client._port))
        except Exception as e:
            print("c> ERROR: Conecting to the server:", str(e))
            return client.RC.ERROR
        
        # Send the data
        try:
            # command
            client_socket.sendall(cmd.encode("utf-8"))
            # datetime
            datetime = client.getdatetime() + "\0"
            client_socket.sendall(datetime.encode("utf-8"))
            # username
            user_send = user + "\0"
            client_socket.sendall(user_send.encode("utf-8"))
            # port
            port_send = str(port) + "\0"
            client_socket.sendall(port_send.encode("utf-8"))
            # ip
            ip_send = client._server + "\0"
            client_socket.sendall(ip_send.encode("utf-8"))
        except Exception as e:
            print("c> ERROR: Sending the request:", str(e))
            return client.RC.ERROR
        
        # Receive the response
        try:
            res = client.readline(client_socket)
        except Exception as e:
            print("c> ERROR: Receiving the response:", str(e))
            return client.RC.ERROR
        
        if res == "0":
            print("c> CONNECT OK")
            client._username = user
        elif res == "1":
            print("c> CONNECT FAIL, USER DOES NOT EXIST")
        elif res == "2":
            print("c> USER ALREADY CONNECTED")
        else:
            print("c> CONNECT FAIL")
        
        # Close the connection
        try:
            client_socket.close()
        except Exception as e:
            print("Error closing the connection:", str(e))
            return client.RC.ERROR

        return client.RC.OK
    
    @staticmethod
    def disconnect(user) :
        """ Disconnect a user from the system """
        cmd = "DISCONNECT\0"

        if client._quit and not client._connection:
            print("c> BYE!")
            return client.RC.OK

        # Check the user name
        if user is None or len(user) > 256:
            print("c> ERROR: Invalid user name")
            return client.RC.USER_ERROR
        
        # Connect to the server
        try:
            client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            client_socket.connect((client._server, client._port))
        except Exception as e:
            print("c> ERROR: Conecting to the server:", str(e))
            return client.RC.ERROR
        
        # Send the data
        try:
            # command
            client_socket.sendall(cmd.encode("utf-8"))
            # datetime
            datetime = client.getdatetime() + "\0"
            client_socket.sendall(datetime.encode("utf-8"))
            # username
            user_send = user + "\0"
            client_socket.sendall(user_send.encode("utf-8"))
        except Exception as e:
            print("c> ERROR: Sending the request:", str(e))
            return client.RC.ERROR
        
        # Receive the response
        try:
            res = client.readline(client_socket)
        except Exception as e:
            print("c> ERROR: Receiving the response:", str(e))
            return client.RC.ERROR
        
        if res == "0":
            print("c> DISCONNECT OK")
            client._connection = False
            # Close the listener
            try:
                client._listen_socket.close()
            except:
                pass
            client._listen_socket = None
        elif res == "1":
            print("c> DISCONNECT FAIL, USER DOES NOT EXIST")
        elif res == "2":
            print("c> DISCONNECT FAIL, USER NOT CONNECTED")
        else:
            print("c> DISCONNECT FAIL")
        
        # Close the connection
        try:
            client_socket.close()
        except Exception as e:
            print("Error closing the connection:", str(e))
            return client.RC.ERROR
        
        return client.RC.OK

    @staticmethod
    def publish(fileName,  description) :
        """ Publish a file in the system """
        cmd = "PUBLISH\0"
        
        # Check the fileName and description
        if fileName == "" or fileName == None or description == "" or description == None:
            print("FileName or description not valid")
            return client.RC.ERROR
        
        # Connect to the server
        try:
            client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            client_socket.connect((client._server, client._port))
        except Exception as e:
            print("c> ERROR: Conecting to the server:", str(e))
            return client.RC.ERROR
        
        # Send the data
        try:
            # command
            client_socket.sendall(cmd.encode("utf-8"))
            # datetime
            datetime = client.getdatetime() + "\0"
            client_socket.sendall(datetime.encode("utf-8"))
            # username
            name = client._username + "\0"
            client_socket.sendall(name.encode("utf-8"))
            # filename
            file = fileName + "\0"
            client_socket.sendall(file.encode("utf-8"))
            # description
            desc = description + "\0"
            client_socket.sendall(desc.encode("utf-8"))
        except Exception as e:
            print("c> ERROR: Sending the request:", str(e))
            return client.RC.ERROR
        
        # Receive the response
        try:
            res = client.readline(client_socket)
        except Exception as e:
            print("c> ERROR: Receiving the response:", str(e))
            return client.RC.ERROR
        
        if res == "0":
            print("c> PUBLISH OK")
        elif res == "1":
            print("c> PUBLISH FAIL, USER DOES NOT EXIST")
        elif res == "2":
            print("c> PUBLISH FAIL, USER NOT CONNECTED")
        elif res == "3":
            print("c> PUBLISH FAIL, CONTENT ALREADY PUBLISHED")
        else:
            print("c> PUBLISH FAIL")

        # Close the connection
        try:
            client_socket.close()
        except Exception as e:
            print("Error closing the connection:", str(e))
            return client.RC.ERROR

        return client.RC.OK

    @staticmethod
    def delete(fileName) :
        """ Delete a file from the system """
        cmd = "DELETE\0"
        
        # Check the fileName
        if fileName == "" or fileName == None:
            print("ERROR: FileName not valid")
            return client.RC.ERROR
        
        # Connect to the server
        try:
            client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            client_socket.connect((client._server, client._port))
        except Exception as e:
            print("c> ERROR: Conecting to the server:", str(e))
            return client.RC.ERROR
        
        # Send the data
        try:
            # command
            client_socket.sendall(cmd.encode("utf-8"))
            # datetime
            datetime = client.getdatetime() + "\0"
            client_socket.sendall(datetime.encode("utf-8"))
            # username
            name = client._username + "\0"
            client_socket.sendall(name.encode("utf-8"))
            # filename
            file = fileName + "\0"
            client_socket.sendall(file.encode("utf-8"))
        except Exception as e:
            print("c> ERROR: Sending the request:", str(e))
            return client.RC.ERROR
        
        # Receive the response
        try:
            res = client.readline(client_socket)
        except Exception as e:
            print("c> ERROR: Receiving the response:", str(e))
            return client.RC.ERROR
        
        if res == "0":
            print("c> DELETE OK")
        elif res == "1":
            print("c> DELETE FAIL, USER DOES NOT EXIST")
        elif res == "2":
            print("c> DELETE FAIL, USER NOT CONNECTED")
        elif res == "3":
            print("c> DELETE FAIL, CONTENT NOT PUBLISHED")
        else:
            print("c> DELETE FAIL")

        # Close the connection
        try:
            client_socket.close()
        except Exception as e:
            print("Error closing the connection:", str(e))
            return client.RC.ERROR

        return client.RC.OK

    @staticmethod
    def listusers() :
        """ List all users in the system """
        cmd = "LIST_USERS\0"

        # Connect to the server
        try:
            client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            client_socket.connect((client._server, client._port))
        except Exception as e:
            print("c> ERROR: Conecting to the server:", str(e))
            return client.RC.ERROR
        
        # Send the data
        try:
            # command
            client_socket.sendall(cmd.encode("utf-8"))
            # datetime
            datetime = client.getdatetime() + "\0"
            client_socket.sendall(datetime.encode("utf-8"))
            # username
            user = client._username + "\0"
            client_socket.sendall(user.encode("utf-8"))
        except Exception as e:
            print("c> ERROR: Sending the request:", str(e))
            return client.RC.ERROR
        
        # Receive the response
        try:
            msg = client.readline(client_socket)
        except Exception as e:
            print("c> ERROR: Receiving the response:", str(e))
            return client.RC.ERROR
        
        lista_msg = msg.split()
        res = lista_msg[0]

        if res == "0":
            print("c> LIST_USERS OK")
            usuarios_dic = {}
            num = lista_msg[1]
            for i in range(2, len(lista_msg), 3):
                # Extraer el nombre de usuario, la dirección IP y el puerto
                user_name = lista_msg[i]
                ip = lista_msg[i+1]
                port = lista_msg[i+2]

                # Añadir la información del usuario al diccionario
                usuarios_dic[user_name] = [ip, port]

                # Imprimir la información del usuario
                print("\t", user_name, ip, port)
            client._current_users = usuarios_dic
        elif res == "1":
            print("c> LIST_USERS FAIL, USER DOES NOT EXIST")
        elif res == "2":
            print("c> LIST_USERS FAIL, USER NOT CONNECTED")
        else:
            print("c> LIST_USERS FAIL")
        
        return client.RC.OK

    @staticmethod
    def listcontent(user) :
        """ List all content published by a user """
        cmd = "LIST_CONTENT\0"

        # Connect to the server
        try:
            client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            client_socket.connect((client._server, client._port))
        except Exception as e:
            print("c> ERROR: Conecting to the server:", str(e))
            return client.RC.ERROR
        
        # Send the data
        try:
            # command
            client_socket.sendall(cmd.encode("utf-8"))
            # datetime
            datetime = client.getdatetime() + "\0"
            client_socket.sendall(datetime.encode("utf-8"))
            # username
            user_send = client._username + "\0"
            client_socket.sendall(user_send.encode("utf-8"))
            # remote username
            remote_user = user + "\0"
            client_socket.sendall(remote_user.encode("utf-8"))
        except Exception as e:
            print("c> ERROR: Sending the request:", str(e))
            return client.RC.ERROR
        
        # Receive the response
        try:
            msg = client.readline(client_socket)
        except Exception as e:
            print("c> ERROR: Receiving the response:", str(e))
            return client.RC.ERROR
        
        lista_msg = msg.split(" | ")
        
        lista = lista_msg[0].split()
        res = lista[0]

        if res == "0":
            print("c> LIST_CONTENT OK")
            num = lista[1]
            for i in range(1, int(num)+1):
                print("\t", lista_msg[i])
        elif res == "1":
            print("c> LIST_CONTENT FAIL, USER DOES NOT EXIST")
        elif res == "2":
            print("c> LIST_CONTENT FAIL, USER NOT CONNECTED")
        elif res == "3":
            print("c> LIST_CONTENT FAIL, REMOTE USER DOES NOT EXIST")
        else:
            print("c> LIST_CONTENT FAIL")

        return client.RC.OK

    @staticmethod
    def getfile(user,  remote_FileName,  local_FileName) :
        """ Get a file from other user """
        cmd = "GET_FILE"

        # Connect to the server
        try:
            socket_sd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            dir_sd = client._current_users[user][0]
            print("\tdir: ", dir_sd)
            port_sd = int(client._current_users[user][1])
            print("\tport: ", port_sd)
            socket_sd.connect((dir_sd, port_sd))
        except Exception as e:
            print("c> ERROR: Conecting to the server:", str(e))
            return client.RC.ERROR

        msg = cmd + " " + user + " " + client._username + " " + remote_FileName + " " + local_FileName + "\0"

        # Send the data
        try:
            socket_sd.sendall(msg.encode("utf-8"))
        except Exception as e:
            print("c> ERROR: Sending the request:", str(e))
            return client.RC.ERROR
        
        # Receive the response
        try:
            res = client.readline(socket_sd)
        except Exception as e:
            print("c> ERROR: Receiving the response:", str(e))
            return client.RC.ERROR

        if res == "0":
            print("c> GET_FILE OK")
        elif res == "1":
            print("c> GET_FILE FAIL, FILE NOT EXIST")
        elif res == "2":
            print("c> GET_FILE FAIL")

        return client.RC.ERROR
    
    @staticmethod
    def readline(socket) :
        """ Read a line from a socket """
        msg = ""
        while True:
            char = socket.recv(1).decode("utf-8")
            if char == "\0":
                break
            msg += char
        return msg

    @staticmethod
    def tratar_peticion(socket_sd) :
        """ Tratar una petición de un cliente """
        # print("\tTratando petición...")

        # Copy the socket in a new local socket
        mutex.acquire()
        new_sd = socket_sd
        mutex.release()

        # Receive the request
        try:
            msg = client.readline(new_sd)
        except Exception as e:
            print("c> ERROR: Conecting to the server:", str(e))
            return client.RC.ERROR

        # Process the request
        lista_msg = msg.split()
        op = lista_msg[0]


        if op == "GET_FILE":
            user = lista_msg[1]
            other_user = lista_msg[2]   # Usuario del que se quiere copiar el contenido
            my_file = lista_msg[3]      # Fichero deseado, el contenido deseado
            other_file = lista_msg[4]   # Fichero del otro usuario, donde copiar el contenido

            path = "data/" + user + "/" + my_file
            path2 = "data/" + other_user + "/" + other_file

            err = 0

            # Check if the file exists
            try:
                file1 = open(path, "r")
            except FileNotFoundError:
                res = "1\0"
                err = -1
            
            if err == 0:
                try:
                    with open(path, "r") as file1:
                        with open(path2, "w") as file2:
                            for line in file1:
                                file2.write(line)
                    res = "0\0"
                except Exception as e:
                    res = "2\0"
            
            # Send the response
            try:
                new_sd.sendall(res.encode("utf-8"))
            except Exception as e:
                print("c> ERROR: Sending the response:", str(e))
                return client.RC.ERROR
            
            new_sd.close()

        return client.RC.OK

    @staticmethod
    def connection_client(port):
        # print("\tCreando hilo...")
        # print("\tPort: ", port)
        while client._connection:
            new_sd, addr = client._listen_socket.accept()
            # print("\tConnected to: ", addr)

            # Create a thread to treat the request
            global thread_clsv
            thread_clsv = threading.Thread(target=client.tratar_peticion, args=(new_sd, ))
            thread_clsv.daemon = True
            thread_clsv.start()

    # WebService
    @staticmethod
    def getdatetime() :
        """ Get the date and time """
        wsdl_url = "http://localhost:8000/?wsdl"
        try:
            soap = zeep.Client(wsdl_url)
            result = soap.service.get_datetime()
            return result
        except Exception as e:
            print("c> ERROR: ", str(e))
            return None

    # *
    # **
    # * @brief Command interpreter for the client. It calls the protocol functions.
    @staticmethod
    def shell():
        while (True) :
            try :
                command = input("c> ")
                line = command.split(" ")
                if (len(line) > 0):

                    line[0] = line[0].upper()

                    if (line[0]=="REGISTER") :
                        if (len(line) == 2) :
                            client.register(line[1])
                        else :
                            print("Syntax error. Usage: REGISTER <userName>")

                    elif(line[0]=="UNREGISTER") :
                        if (len(line) == 2) :
                            client.unregister(line[1])
                        else :
                            print("Syntax error. Usage: UNREGISTER <userName>")

                    elif(line[0]=="CONNECT") :
                        if (len(line) == 2) :
                            client.connect(line[1])
                        else :
                            print("Syntax error. Usage: CONNECT <userName>")
                    
                    elif(line[0]=="PUBLISH") :
                        if (len(line) >= 3) :
                            #  Remove first two words
                            description = ' '.join(line[2:])
                            client.publish(line[1], description)
                        else :
                            print("Syntax error. Usage: PUBLISH <fileName> <description>")

                    elif(line[0]=="DELETE") :
                        if (len(line) == 2) :
                            client.delete(line[1])
                        else :
                            print("Syntax error. Usage: DELETE <fileName>")

                    elif(line[0]=="LIST_USERS") :
                        if (len(line) == 1) :
                            client.listusers()
                        else :
                            print("Syntax error. Use: LIST_USERS")

                    elif(line[0]=="LIST_CONTENT") :
                        if (len(line) == 2) :
                            client.listcontent(line[1])
                        else :
                            print("Syntax error. Usage: LIST_CONTENT <userName>")

                    elif(line[0]=="DISCONNECT") :
                        if (len(line) == 2) :
                            client.disconnect(line[1])
                        else :
                            print("Syntax error. Usage: DISCONNECT <userName>")

                    elif(line[0]=="GET_FILE") :
                        if (len(line) == 4) :
                            client.getfile(line[1], line[2], line[3])
                        else :
                            print("Syntax error. Usage: GET_FILE <userName> <remote_fileName> <local_fileName>")

                    elif(line[0]=="QUIT") :
                        if (len(line) == 1) :
                            client._quit = 1
                            client.disconnect(client._username)
                            break
                        else :
                            print("Syntax error. Use: QUIT")
                    else :
                        print("Error: command " + line[0] + " not valid.")
            except Exception as e:
                print("Exception: " + str(e))

    # *
    # * @brief Prints program usage
    @staticmethod
    def usage() :
        print("Usage: python3 client.py -s <server> -p <port>")


    # *
    # * @brief Parses program execution arguments
    @staticmethod
    def parseArguments(argv) :
        parser = argparse.ArgumentParser()
        parser.add_argument('-s', type=str, required=True, help='Server IP')
        parser.add_argument('-p', type=int, required=True, help='Server Port')
        args = parser.parse_args()

        if (args.s is None or args.p is None):
            parser.error("Usage: python3 client.py -s <server IP> -p <server port>")
            return False

        if ((args.p < 1024) or (args.p > 65535)):
            parser.error("Error: Port must be in the range 1024 <= port <= 65535")
            return False
        
        client._server = args.s
        client._port = args.p

        return True


    # ******************** MAIN *********************
    @staticmethod
    def main(argv) :
        if (not client.parseArguments(argv)) :
            client.usage()
            return

        client.shell()
        print("+++ FINISHED +++")
    

if __name__=="__main__":
    client.main([])
