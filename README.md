# Sistema peer-to-peer
*Autora: Celia Patricio Ferrer.*

Este documento recoge todas las decisiones que se han llevado a cabo, así como las funcionalidades que se han desarrollado para cubrir todos los aspectos de este sistema de ficheros.

## Indice

- [Objetivo](#1-objetivo)
- [Arquitectura](#2-arquitectura)
- [Compilación y ejecución](#3-compilación-y-ejecución)
- [Operaciones](#4-operaciones)
    - [Registro de un usuario](#41-registro-de-un-usuario)
    - [Baja de un usuario](#42-baja-de-un-usuario)
    - [Conexión de un usuario](#43-conexión-de-un-cliente)
    - [Publicación de un fichero](#44--publicación-de-un-fichero)
    - [Eliminación de un fichero](#45-eliminación-de-un-fichero)
    - [Listado de usuarios](#46-listado-de-usuarios-conectados)
    - [Listado de publicaciones](#47-listado-de-publicaciones-de-un-usuario)
    - [Desconexión de un usuario](#48-desconexión-de-un-usuario)
    - [Transferencia de un fichero](#49-transferencia-de-un-fichero)
    - [Cierre el servicio](#410-cierre-del-servicio)
- [Implementación](#5-implementación)
    - [Cliente](#51-implementación-del-cliente)
    - [Servicio Web](#52-implementación-del-servicio-web)
    - [Servidor](#53-implementación-del-servidor)
    - [RPC](#54-implementación-del-sistema-rpc)
- [Pruebas realizadas](#6-pruebas-realizadas)
- [Conclusiones](#7-conclusiones)

---
---

## 1. Objetivo

Esta práctica tiene como objetivo global desarrollar un sistema *peer-to-peer* de distribución de fichero entre clientes. 

En la primera parte del proyecto, se incluye la comunicación de procesos usando sockets TCP. Para ellos, se desarrolla una aplicación de distribuicón de ficheros entre clientes.

Mientras que, la segunda parte se centra 
en los principales conceptos relacionados con las llamadas a procedimientos remotos y los servicios web.

---
---

## 2. Arquitectura
La arquitectura que se ha diseñado para este sistema es modular y escalable con el fin de facilitar la anexión de las dos partes solicitadas. Se basa en un modelo descentralizado peer–to–peer, que permite la interacción entre clientes directamente sin la necesidad de depender de un servidor centralizado para el intercambio de ficheros. Además de las dos partes adicionales anexadas a este sistema para satisfacer la segunda parte.

![Arquitectura del sistema](img/img1.png)

De esta forma, el cliente solicita diferentes operaciones al servidor y además, permite la recepción de solicitudes de archivos directamente de otros clientes. Mientras que el servidor, recibe las peticiones y gestiona la base de datos, en donde se guarda la información relevante de los clientes. Por un lado, el servidor recibe las peticiones y devuelve la respuesta correspondiente, con el valor de salida tras realizar la operación pedida. Y por otro lado, el cliente envía la operación y lee la respuesta recibida. 

La integración del servicio web en la parte del cliente se utiliza para la obtenición de la fecha y la hora actual. Este servicio web devuelve una cadena con la fecha y la hora actual y es llamado por el cliente cada vez que desea enviar una petición, para enviar la petición junto a la fecha y a la hora realizada. Cuando el servidor principal recibe la fecha y la hora actual, actuará como cliente con el servidor RPC. Para ello, envía la petición de imprimir la operación por pantalla y envía el nombre del usuario, la operación solicitada y la cadena de caracteres con la fecha y la hora. 

---
---

## 3. Compilación y ejecución

Hay dos posibilidades para compilar y ejecutar este servicio. La primera se trata de una predefinida en la que se han preparado dos scripts que realizan la compilación y la apertura de las terminales necesarias. Además, de escribir los comandos necesarios para la ejecución global del servicio.

En primer lugar, en una terminal en donde esté abierto el proyecto, se debe dar los permisos necesaios con el comando:

```sh
chmod +x run_server.sh run_client.sh
```

Posteriormente, en esta terminal, se debe escribir el siguiente comando, para ejecutar la parte del servidor: 

```sh
run_server.sh
```

Y luego este otro comando, para ejecutar la parte del cliente: 

```sh
run_client.sh
```

El primer script se encarga de la parte del servidor (server.c y RPC server) que realiza la compilación necesaria con el comando *make* y abre dos terminales *gnome* en donde se ejecutan el servidor y el servidor de RPC, se da por defecto el valor del puerto 8080. Mientras que, el otro script se encarga de la parte del cliente (client.py y ws-client.py) que abre también dos terminales gnome y ejecuta el cliente y el WebService, se da por defecto los valores localhost y 8080 para el servidor y el puerto, respectivamente.

---

Por otro lado, esta segunda posibilidad es para realizar este proceso de forma manual. Para ello, se debe abrir cuatro terminales para cada componente del servicio. En una de ellas se debe realizar la compilación de la parte del servidor con el comando *make*. Una va a ser para el servidor RPC que se debe ejecutar con el comando siguiente:

```sh
./server_rpc
````

Otra terminal va a ser para el servidor del sistema que se debe ejecutar con el comando:

```sh
./server
````

Otra, para el WebService del cliente, que se debe ejecutar con el comando:

```sh
python3 ws-client.py
```

Y la última es para el cliente de Python, que se debe ejecutar con el comando siguiente:

```sh
python3 client.py -s <server_ip> -p <server_port>
```

Donde *server_ip* y *server_port* son la dirección IP y el puerto que se desee.

---
---

## 4. Operaciones

En esta sección se detallan las funcionalidades que tiene este servicio, así como de su funcionamiento. 

El cliente se trata de un intérprete de comandos multihilo que almacena información relevante como la dirección IP, el puerto, el nombre del usuario conectado, entre otros. El programa detecta errores en la línea de comandos y ejecuta diferentes procedimientos para las diferentes operaciones introducidas.

A continuación se describen las distintas operaciones que el cliente puede realizar en este servicio.

### 4.1. Registro de un usuario.

Cuando un cliente quiere registrarse en el servicio, se debe introducir el comando: 

```sh 
REGISTER <username>
```

Donde *username* es el nombre que el usuario decida y el que utilizará para hacer uso de la aplicación. Al realizar esto, el sistema internamente verifica que el nombre sea válido, para ello debe tener una longitud menor a 256 caracteres. Internamente, el sistema verifica que ese nombre de usurio sea único en el sistema y muestra por pantalla el resultado.

### 4.2. Baja de un usuario.

Por el contrario, cuando un cliente quiere darse de baja del servicio, se debe introducir el comando: 

```sh 
UNREGISTER <username>
```

Donde *username* es el nombre que inicialmente ha utilizado previamente para el registro. Internamente, el sistema verifica que el usurio exista en el registro y muestra por pantalla el resultado.

### 4.3. Conexión de un cliente.

Si el cliente quiere conectarse al sistema, se debe introducir el siguiente comando:

```sh 
CONNECT <username>
```

Donde *username* es el nombre del usuario que ha sido utilizado para el registro. El sistema internamente verifica que el usuario previamente esté registrado y si ya hay un cliente conectado, debido a que solo es posible la conexión de un único cliente por terminal. 

En este momento, el cliente estará conectado al sistema y podrá ver el contenido de otros usuarios, la lista de usuarios conectados y descargarse ficheros de otros usuarios conectados.

### 4.4.  Publicación de un fichero.

Si un cliente quiere realizar una publicación de un archivo, debe introducir el comando: 

```sh
PUBLISH <filename> <description>
````

Donde *filename* es el nombre del archivo y *description* una breve descripción del fichero a publicar. Para la realización de esta operación, es necesario que el usuario previamente esté conectado al sistema. 

### 4.5. Eliminación de un fichero.

Por el contrario, si un usuario desea eliminar una publicación suya, debe introducir el comando: 

```sh
DELETE <filename>
```

Donde filename es el nombre del archivo a publicar. Internamente, el sistema verifica que dicho fichero exista en la base de datos como publicado por ese usuario.

### 4.6. Listado de usuarios conectados.

Cuando un cliente desee obtener la lista con todos los usuarios conectados al sistema, deberá introducir el comando: 

```sh
LIST_USERS
```

Para ello, el usuario debe estar registrado y conectado al sistema. Como requisito para la operación *GET_FILE*, se deberá listar a los usuarios conectados justo antes de obtener el fichero publicado de otro usuario.

### 4.7. Listado de publicaciones de un usuario.

Si un cliente quiere ver las publicaciones realizadas por otro usuario del servicio, debe introducir el comando: 

```sh
LIST_CONTENT <username>
```

Donde *username* es el nombre del usuario cuyas publicaciones se desean ver (al que se le llamará usuario remoto). Como se mencionó con anterioridad, esta operación requiere que el usuario que lo solicita, se encuentre conectado al sistema.

### 4.8. Desconexión de un usuario.

Cuando un cliente quiere desconectarse del servicio, debe introducir el comando: 

```sh
DISCONNECT <username>
```

Donde *username* es el nombre que ha utilizado para registrarse y conectarse al sistema. Un usuario que se desee deconectar del servicio, previamente debe estar registrado y conectado.

### 4.9. Transferencia de un fichero.

Cuando el usuario registrado y conectado al sistema quiera obtener el fichero de otro usuario registrado y conectado al sistema, debe introducir el siguiente comando:

```sh
GET_FILE <remote_username> <remote_filename> <local_filename>
```

Donde *remote_username* es el nombre del usuario cuya publicación se quiere obtener, *remote_filename* es el nombre del fichero remoto que se quiere obtener y *local_filename* es el nombre del fichero local en donde se va a copiar el contenido. 

Como ya se ha mencionado, es necesario que, anteriormente, se ejecute la operación de listar los usuarios conectados. Al realizar esta operación, se verifica internamente que el usuario exista y esté conectado, al igual que el usuario remoto. Además, se comprueba lo mismo con los archivos.

### 4.10. Cierre del servicio.

En caso de que el usuario quiera cerrar el servicio, debe introducir el comando:

```sh
QUIT
```

Al recibir este comando, el sistema internamente desconectará al usuario si este está conectado y cerrará el servicio.

---
---

## 5. Implementación

En esta siguiente sección se detallan los aspectos relevantes llevados a cabo en cada componente.

### 5.1. Implementación del cliente.

De forma general, el sistema internamente al leer una operación en el intérprete de comandos, crea un *socket TCP/IP*. Posteriormente, se conecta al servidor con la función *connect* e indicando la dirección IP y el puerto especificado por línea de comandos en la ejecución inicial. Envía los datos necesarios para la solicitud de la operación y espera a la respuesta correspondiente. Finalmente, cierra la conexión con el servidor. Cuando el cliente recibe la respuesta dada por el servidor, dependiendo de esta muestra el mensaje correspondiente.

1. Registro de un usuario.
    
    El sistema internamente verifica que el *username* sea válido (una longitud menor a 256 caracteres). A continuación, procede a conectarse al servidor para enviar la operación, que incluye el comando, la fecha y hora y el nombre de usuario, a través del socket al servidor. Después, dependiendo de la respuesta que recibe, imprime por pantalla un mensaje. Este mensaje puede ser si el registro se completó con éxito, si el nombre está en uso o no se pudo realizar el registro por cualquier problema.

2. Baja de un usuario.
    
    Con esta operación, el sistema internamente verifica que el *username* sea válido. A continuación, se conecta al servidor y envía la operación, que incluye el comando, la fecha y hora y el nombre de usuario, a través del socket. Una vez que recibe la respuesta, verifica cuál ha sido e imprime por pantalla un mensaje. Es decir, si la eliminación del registro del usuario se completó con éxito, si el usuario no está previamente registrado o si no se pudo realizar por cualquier otro problema.

3. Conexión de un cliente.
    
    Al realizar esta operación, como las anteriores, el sistema internamente verifica que el *username* sea válido. Además, verifica si ya hay un cliente conectado, ya que solo es posible la conexión de un único cliente por cada terminal. En cuyo caso, se muestra el mensaje de error por pantalla. 
    
    A diferencia de las otras operaciones, en esta función se crea un servidor de escucha en un hilo separado. Para ello, crea un socket TCP/IP, configurado para escuchar cualquier interfaz de la red y un puerto libre. A continuación, se crea un nuevo hilo de ejecución cuya función a ejecutar es connection_client, se configura para que se cierre automáticamente y se inicia el hilo.

    Posteriormente, procede a conectarse al servidor. Luego, envía la operación, que incluye el comando, la fecha y hora actuales, el nombre de usuario, la dirección IP y el puerto, a través del socket al servidor. Una vez que recibe la respuesta, verifica cuál ha sido. Dependiendo de esto, imprime por pantalla si la conexión del usuario se completó con éxito, si el nombre no está registrado, si ya está conectado previamente o si no se pudo realizar por cualquier otro problema.

    La función *connection_client* se realiza en un bucle *while*, mientras se cumple la condición si el atributo connection se encuentra en true. En cuyo caso, acepta las peticiones que le lleguen. Posteriormente, crea otro hilo para tratar dicha petición (*tratar_peticion*), pasando como argumentos el socket devuelto por el accept.
    
    La función *tratar_peticion* funciona de forma similar a la del servidor, copia el socket en una variable local, protegiendo este proceso con un mutex. Una vez se ha copiado el socket, se recibe la petición y se procesa. En caso de que la operación sea *GET_FILE*, se recibe el resto de la operación. Posteriormente, se verifica que el fichero exista y se realiza una copia del contenido del fichero indicado en el fichero destino deseado. Se envía de vuelta el resultado, dependiendo de si todo se ha realizado correctamente o si se ha producido cualquier otro error (no exista el fichero u otro).

4.  Publicación de un fichero.
    
    asd

5. Eliminación de un fichero.
    
    asd

6. Listado de usuarios conectados.
    
    asd

7. Listado de publicaciones de un usuario.
    
    asd

8. Desconexión de un usuario.
    
    asd

9. Transferencia de un fichero.

    asd



### 5.2. Implementación del servicio web.



### 5.3. Implementación del servidor.



### 5.4. Implementación del sistema RPC.


## 6. Pruebas realizadas



## 7. Conclusiones