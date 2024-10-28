/* Implementación del servidor */

#include "dop/lines.h"
#include "dop/server-impl.h"
#include "rpc/operacion.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <pthread.h>
#include <netdb.h>

#define MAX_OP 20
#define MAX_STR 256
#define MAX_LINE 1024

/* Variables globales */
char *dir_ip;
int sd;
int ocupado = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

char *get_ip() {
    char host[256];
    char *ip;
    struct hostent *host_entry;

    // Obtener el nombre del host
    if (gethostname(host, sizeof(host)) < 0) {
        perror("Error. Obtención del nombre del host.\n");
        return NULL;
    }

    // Obtener la dirección IP
    host_entry = gethostbyname(host);
    if (host_entry == NULL) {
        perror("Error. Obtención de la dirección IP.\n");
        return NULL;
    }

    ip = inet_ntoa(*((struct in_addr *) host_entry->h_addr_list[0]));

    return ip;
}

/* Llamada al procedimiento remoto */
int rpc_imprimir(char *username, char *op, char *datetime, char *filename) {
    // Crear el cliente para el RPC
    CLIENT *clnt;
    clnt = clnt_create(dir_ip, IMPRIMIR_OPERACION, IMPRIMIR_OPERACION_V1, "tcp");
    if (clnt == NULL) {
        clnt_pcreateerror(dir_ip);
        printf("ERROR: No se pudo crear el cliente.\n");
        return -1;
    }

    // Crear la operación
    struct Operacion operacion;
    operacion.username = username;
    operacion.op = op;
    operacion.datetime = datetime;
    operacion.filename = filename;

    // Llamar al procedimiento remoto
    int result;
    enum clnt_stat retval;
    retval = print_operacion_1(operacion, &result, clnt);
    if (retval != RPC_SUCCESS) {
        clnt_perror(clnt, "call failed");
        printf("ERROR: Llamada al procedimiento fallida.\n");
        clnt_destroy(clnt);
        return -1;
    }

    // Destruir el cliente
    clnt_destroy(clnt);

    return 0;
}

/* Tratar petición */
void tratar_peticion(int *s) {
    int sd_local;

    pthread_mutex_lock(&mutex);
    sd_local = (* (int *)s);
    ocupado = 0;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);

    /* tratar la peticion con el descriptor local */
    char op[MAX_OP];
    char datetime[MAX_STR];

    // Respuesta
    char res[MAX_LINE];

    // Recibir la operación
    if (recvMessage(sd_local, op, MAX_OP) < 0) {
        perror("Error. Recepción de la operación.\n");
        return;
    }

    // Recibir la fecha y hora
    if (recvMessage(sd_local, datetime, MAX_STR) < 0) {
        perror("Error. Recepción de la fecha y hora.\n");
        return;
    }

    if (strcmp(op, "REGISTER") == 0) {
        // Recibir el nombre del usuario
        char user[MAX_STR];
        if (recvMessage(sd_local, user, MAX_STR) < 0) {
            perror("Error. Recepción del nombre del usuario.\n");
            return;
        }

        // Imprimir mensaje
        printf("s> REGISTER FROM %s\n", user);

        // Llamar al procedimiento remoto para imprimir la operación
        rpc_imprimir(user, op, datetime, "");

        int result = register_user(user);

        // Resultado
        if (result == 0) {
            strcpy(res, "0\0");
        } else if (result == 1) {
            strcpy(res, "1\0");
        } else {
            strcpy(res, "2\0");
        }

        // Enviar el resultado
        if (sendMessage(sd_local, res, MAX_LINE) < 0) {
            perror("Error. Envío del resultado.\n");
            return;
        }
    }

    else if (strcmp(op, "UNREGISTER") == 0) {
        // Recibir el nombre del usuario
        char user[MAX_STR];
        if (recvMessage(sd_local, user, MAX_STR) < 0) {
            perror("Error. Recepción del nombre del usuario.\n");
            return;
        }

        // Imprimir mensaje
        printf("s> UNREGISTER FROM %s\n", user);

        // Llamar al procedimiento remoto para imprimir la operación
        rpc_imprimir(user, op, datetime, "");

        int result = unregister_user(user);

        // Resultado
        if (result == 0) {
            strcpy(res, "0\0");
        } else if (result == 1) {
            strcpy(res, "1\0");
        } else {
            strcpy(res, "2\0");
        }

        // Enviar el resultado
        if (sendMessage(sd_local, res, MAX_LINE) < 0) {
            perror("Error. Envío del resultado.\n");
            return;
        }
    }

    else if (strcmp(op, "CONNECT") == 0) {
        // Recibir el nombre del usuario
        char user[MAX_STR];
        if (recvMessage(sd_local, user, MAX_STR) < 0) {
            perror("Error. Recepción del nombre del usuario.\n");
            return;
        }

        // Recibir el puerto
        char port[MAX_STR];
        if (recvMessage(sd_local, port, MAX_STR) < 0) {
            perror("Error. Recepción del puerto.\n");
            return;
        }

        // Recibir la IP
        char ip[MAX_STR];
        if (recvMessage(sd_local, ip, MAX_STR) < 0) {
            perror("Error. Recepción de la IP.\n");
            return;
        }

        // Imprimir mensaje
        printf("s> CONNECT FROM %s\n", user);

        // Llamar al procedimiento remoto para imprimir la operación
        rpc_imprimir(user, op, datetime, "");

        int result = connect_user(user, ip, atoi(port));

        // Resultado
        if (result == 0) {
            // sprintf(res, "0");
            strcpy(res, "0\0");
        } else if (result == 1) {
            strcpy(res, "1\0");
        } else if (result == 2) {
            strcpy(res, "2\0");
        } else {
            strcpy(res, "3\0");
        }

        // Enviar el resultado
        if (sendMessage(sd_local, res, MAX_LINE) < 0) {
            perror("Error. Envío del resultado.\n");
            return;
        }
    }

    else if (strcmp(op, "DISCONNECT") == 0) {
        // Recibir el nombre del usuario
        char user[MAX_STR];
        if (recvMessage(sd_local, user, MAX_STR) < 0) {
            perror("Error. Recepción del nombre del usuario.\n");
            return;
        }

        // Imprimir mensaje
        printf("s> DISCONNECT FROM %s\n", user);

        // Llamar al procedimiento remoto para imprimir la operación
        rpc_imprimir(user, op, datetime, "");

        int result = disconnect_user(user);

        // Resultado
        if (result == 0) {
            strcpy(res, "0\0");
        } else if (result == 1) {
            strcpy(res, "1\0");
        } else if (result == 2) {
            strcpy(res, "2\0");
        } else {
            strcpy(res, "3\0");
        }

        // Enviar el resultado
        if (sendMessage(sd_local, res, MAX_LINE) < 0) {
            perror("Error. Envío del resultado.\n");
            return;
        }
    }

    else if (strcmp(op, "PUBLISH") == 0) {
        // Recibir el nombre del usuario
        char user[MAX_STR];
        if (recvMessage(sd_local, user, MAX_STR) < 0) {
            perror("Error. Recepción del nombre del usuario.\n");
            return;
        }

        // Recibir el nombre del fichero
        char file[MAX_STR];
        if (recvMessage(sd_local, file, MAX_STR) < 0) {
            perror("Error. Recepción del nombre del usuario.\n");
            return;
        }

        // Recibir la descrpción del fichero
        char description[MAX_STR];
        if (recvMessage(sd_local, description, MAX_STR) < 0) {
            perror("Error. Recepción del nombre del usuario.\n");
            return;
        }

        // Imprimir mensaje
        printf("s> PUBLISH FROM %s\n", user);

        // Llamar al procedimiento remoto para imprimir la operación
        rpc_imprimir(user, op, datetime, file);

        int result = publish_file(user, file, description);

        // Resultado
        if (result == 0) {
            strcpy(res, "0\0");
        } else if (result == 1) {
            strcpy(res, "1\0");
        } else if (result == 2) {
            strcpy(res, "2\0");
        } else if (result == 3) {
            strcpy(res, "3\0");
        } else {
            strcpy(res, "4\0");
        }

        // Enviar el resultado
        if (sendMessage(sd_local, res, MAX_LINE) < 0) {
            perror("Error. Envío del resultado.\n");
            return;
        }
    }

    else if (strcmp(op, "DELETE") == 0) {
        // Recibir el nombre del usuario
        char user[MAX_STR];
        if (recvMessage(sd_local, user, MAX_STR) < 0) {
            perror("Error. Recepción del nombre del usuario.\n");
            return;
        }

        // Recibir el nombre del fichero
        char file[MAX_STR];
        if (recvMessage(sd_local, file, MAX_STR) < 0) {
            perror("Error. Recepción del nombre del usuario.\n");
            return;
        }

        // Imprimir mensaje
        printf("s> DELETE FROM %s\n", user);

        // Llamar al procedimiento remoto para imprimir la operación
        rpc_imprimir(user, op, datetime, file);

        int result = delete_file(user, file);

        // Resultado
        if (result == 0) {
            strcpy(res, "0\0");
        } else if (result == 1) {
            strcpy(res, "1\0");
        } else if (result == 2) {
            strcpy(res, "2\0");
        } else {
            strcpy(res, "3\0");
        }

        // Enviar el resultado
        if (sendMessage(sd_local, res, MAX_LINE) < 0) {
            perror("Error. Envío del resultado.\n");
            return;
        }
    }

    else if (strcmp(op, "LIST_USERS") == 0) {
        // Recibir el nombre del usuario
        char user[MAX_STR];
        if (recvMessage(sd_local, user, MAX_STR) < 0) {
            perror("Error. Recepción del nombre del usuario.\n");
            return;
        }

        // Imprimir mensaje
        printf("s> LIST_USERS FROM %s\n", user);

        // Llamar al procedimiento remoto para imprimir la operación
        rpc_imprimir(user, op, datetime, "");

        char *usuarios_conectados;
        int result = list_users(user, &usuarios_conectados);

        // Resultado
        if (result == -1) {
            strcpy(res, "1\0");
        } else if (result == -2) {
            strcpy(res, "2\0");
        } else if (result == -3) {
            strcpy(res, "3\0");
        } else if (result == 0) {
            strcpy(res, "0\0");
        } else {
            sprintf(res, "0 %d %s", result, usuarios_conectados);
        }

        // Enviar el resultado
        if (sendMessage(sd_local, res, MAX_LINE) < 0) {
            perror("Error. Envío del resultado.\n");
            return;
        }
    }

    else if (strcmp(op, "LIST_CONTENT") == 0) {
        // Recibir el nombre del usuario
        char user[MAX_STR];
        if (recvMessage(sd_local, user, MAX_STR) < 0) {
            perror("Error. Recepción del nombre del usuario.\n");
            return;
        }

        // Recibir el nombre del usuario remoto
        char user_remote[MAX_STR];
        if (recvMessage(sd_local, user_remote, MAX_STR) < 0) {
            perror("Error. Recepción del nombre del usuario remoto.\n");
            return;
        }

        // Imprimir mensaje
        printf("s> LIST_CONTENT FROM %s\n", user);

        // Llamar al procedimiento remoto para imprimir la operación
        rpc_imprimir(user, op, datetime, "");

        char *ficheros;
        int result = list_content(user, user_remote, &ficheros);

        // Resultado
        if (result == -1) {
            strcpy(res, "1\0");
        } else if (result == -2) {
            strcpy(res, "2\0");
        } else if (result == -3) {
            strcpy(res, "3\0");
        } else {
            sprintf(res, "0 %d | %s", result, ficheros);
        }

        // Enviar el resultado
        if (sendMessage(sd_local, res, MAX_LINE) < 0) {
            perror("Error. Envío del resultado.\n");
            return;
        }
    }

    else {
        printf("Operación no válida, %s\n", op);
    }
    
    // Cerrar el socket local
    if (close(sd_local) < 0) {
        perror("Error. Cierre del socket.\n");
    }

    pthread_exit(NULL);
}

void cierre(int sig) {
    printf("\nFin del servicio.\n");
    printf("******************\n");
    // Vaciar la base de datos
    finalizar_servicio();
    // Destruir los mutex y las variables de condición
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
    // Cerrar el socket
    close(sd);
    exit(0);
}

int main(int argc, char *argv[]) {
    // Comprobar los parámetros.
    if (argc != 3) {
        perror("Error. Uso: ./server -p <puerto>.\n");
        return -1;
    }
    // Comprobar el valor del puerto.
    int port = atoi(argv[2]);
    if (port < 1024 || port > 49151) {
        perror("Error. Valor del puerto no válido.\n");
        return -1;
    }

    // Inicializar la base de datos
    iniciar_servicio();

    // Crear el hilo
    pthread_t thid;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    // Variables para el socket
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t size;
    
    // Crear el socket
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Error. Creación del socket.\n");
        return -1;
    }

    int val = 1;
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *) &val, sizeof(val));

    // Configurar la dirección del servidor
    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    // Bind
    if (bind(sd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error. Socket bind.\n");
        return -1;
    }

    // Listen
    if (listen(sd, SOMAXCONN)) {
        perror("Error. Socket listen.\n");
        return -1;
    }

    // Obtener la dirección IP
    dir_ip = inet_ntoa(server_addr.sin_addr);

    // Imprimir mensaje
    printf("s> init server %s : %d\n", dir_ip, port);

    // Capturar la señal
    signal(SIGINT, cierre);

    size = sizeof(client_addr);

    while(1) {
        // Aceptar la conexión
        int new_sd = accept(sd, (struct sockaddr *) &client_addr, (socklen_t *) &size);
        if (new_sd < 0) {
            perror("Error. Socket accept.\n");
            return -1;
        }

        // Crear un hilo para tratar la petición
        if (pthread_create(&thid, &attr, (void *)tratar_peticion, (void *)&new_sd) != 0) {
            perror("Error. Creación de hilo.\n");
            return -1;
        } else {
            pthread_mutex_lock(&mutex);
            while (ocupado == 1) {
                pthread_cond_wait(&cond, &mutex);
            }
            ocupado = 1;
            pthread_mutex_unlock(&mutex);
        }

    }
    // Cerrar el socket
    if (close(sd) < 0) {
        perror("Error. Cierre del socket.\n");
        return -1;
    }

    return 0;
}
