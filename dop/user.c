#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "user.h"

#define BBDD "data/users.dat"
#define DIR "data/"


void crear_usuario(char *nombre_usuario) {
    /* Crea un directorio con el nombre del usuario */
    char dir_path[256];
    sprintf(dir_path, "%s%s", DIR, nombre_usuario);
    if (mkdir(dir_path, 0777) == -1) {
        perror("Error al crear el directorio del usuario");
    }
}

void borrar_usuario(char *nombre_usuario) {
    /* Borra el directorio con el nombre del usuario */
    char command[256];
    sprintf(command, "rm -rf %s%s", DIR, nombre_usuario);
    if (system(command) == -1) {
        perror("Error al borrar el directorio del usuario");
    }
}

void agregar_contenido(char *nombre_usuario, char *nombre_archivo) {
    /* Crea un fichero en el directorio del usuario */
    char file_path[256];
    sprintf(file_path, "%s%s/%s", DIR, nombre_usuario, nombre_archivo);
    FILE *file = fopen(file_path, "w");
    if (file == NULL) {
        perror("Error al crear el archivo");
    }
    fclose(file);
}

void eliminar_contenido(char *nombre_usuario, char *nombre_archivo) {
    /* Borra el fichero en el directorio del usuario */
    char file_path[256];
    sprintf(file_path, "%s%s/%s", DIR, nombre_usuario, nombre_archivo);
    if (remove(file_path) == -1) {
        perror("Error al eliminar el archivo");
    }
}

int iniciar_base_datos() {
    /* Inicia la base de datos */
    // Crea el fichero de la base de datos
    FILE *file = fopen(BBDD, "wb");
    if (file == NULL) {
        perror("Error. Apertura de la base de datos.\n");
        return -1;
    }
    // Cierra el fichero
    if (fclose(file) != 0) {
        perror("Error. Cierre de la base de datos.\n");
        return -1;
    }
    return 0;
}

int agregar_usuario(struct user *usuario) {
    /* Agrega un usuario a la base de datos */
    // Abre el fichero
    FILE *file = fopen(BBDD, "a+b");
    if (file == NULL) {
        perror("Error. Apertura de la base de datos.\n");
        return -1;
    }
    // Escribe el usuario en el fichero
    if (fwrite(usuario, sizeof(struct user), 1, file) != 1) {
        perror("Error. Escritura en la base de datos.\n");
        fclose(file);
        return -1;
    }
    // Cierra el fichero
    if (fclose(file) != 0) {
        perror("Error. Cierre de la base de datos.\n");
        return -1;
    }
    // Crea el directorio del usuario
    crear_usuario(usuario->name);
    // Para imprimir los usuarios registrados, descomentar
    // print_usuarios();

    return 0;
}

int buscar_usuario(char *nombre) {
    /* Busca un usuario en la base de datos */
    // Abre el fichero
    FILE *file = fopen(BBDD, "rb");
    if (file == NULL) {
        perror("Error. Apertura de la base de datos.\n");
        return -1;
    }
    // Lee en usuario en usuario del fichero
    int encontrado = 0;
    struct user usuario;
    while (fread(&usuario, sizeof(struct user), 1, file)) {
        if (strcmp(usuario.name, nombre) == 0) {
            // Si encuentra el usuario
            encontrado = 1;
            break;
        }
    }
    // Cierra el fichero
    if (fclose(file) != 0) {
        perror("Error. Cierre de la base de datos.\n");
        return -1;
    }
    return encontrado;
}

struct user *obtener_usuario(char *nombre) {
    /* Busca y retorna el usuario en la base de datos */
    // Abre el fichero
    FILE *file = fopen(BBDD, "rb");
    if (file == NULL) {
        perror("Error. Apertura de la base de datos.\n");
        return NULL;
    }
    // Lee en usuario en usuario del fichero
    struct user *usuario = NULL;
    struct user usu;
    while (fread(&usu, sizeof(struct user), 1, file)) {
        if (strcmp(usu.name, nombre) == 0) {
            // Si encuentra el usuario
            usuario = malloc(sizeof(struct user));
            if (usuario == NULL) {
                perror("Error. Reserva de memoria para el usuario.\n");
                return NULL;
            }
            usuario = &usu;
            break;
        }
    }
    // Cierra el fichero
    if (fclose(file) != 0) {
        perror("Error. Cierre de la base de datos.\n");
        return NULL;
    }
    // Retorna el usuario
    return usuario;
}

int eliminar_usuario(char *nombre) {
    /* Elimina un usuario de la base de datos */
    // Abre el fichero
    FILE *file = fopen(BBDD, "rb");
    if (file == NULL) {
        perror("Error. Apertura de la base de datos.\n");
        return -1;
    }
    // Abre el fichero temporal
    FILE *temp = fopen("temp.dat", "wb");
    if (temp == NULL) {
        perror("Error. Apertura del fichero temporal.\n");
        fclose(file);
        return -1;
    }
    // Lee en usuario en usuario del fichero
    struct user usuario;
    while (fread(&usuario, sizeof(struct user), 1, file)) {
        // Si no es el usuario a eliminar
        if (strcmp(usuario.name, nombre) != 0) {
            // Escribe el usuario en el fichero temporal
            if (fwrite(&usuario, sizeof(struct user), 1, temp) != 1) {
                perror("Error. Escritura en el fichero temporal.\n");
                fclose(file);
                fclose(temp);
                return -1;
            }
        }
    }
    // Cierra el fichero
    if (fclose(file) != 0) {
        perror("Error. Cierre de la base de datos.\n");
        fclose(temp);
        return -1;
    }
    // Cierra el fichero temporal
    if (fclose(temp) != 0) {
        perror("Error. Cierre del fichero temporal.\n");
        return -1;
    }
    // Elimina el fichero original
    if (remove(BBDD) != 0) {
        perror("Error. Eliminación de la base de datos.\n");
        return -1;
    }
    // Renombra el fichero temporal para que sea el original
    if (rename("temp.dat", BBDD) != 0) {
        perror("Error. Renombrado del fichero temporal.\n");
        return -1;
    }
    // Borra el directorio del usuario
    borrar_usuario(nombre);
    // Para imprimir los usuarios registrados, descomentar
    // print_usuarios();
    return 0;
}

int conectar_usuario(char *nombre, char *ip, int puerto) {
    /* Modifica el estado de conectado de un usuario y agrega la ip y port */
    // Abre el fichero
    FILE *file = fopen(BBDD, "rb");
    if (file == NULL) {
        perror("Error. Apertura de la base de datos.\n");
        return -1;
    }
    // Abre el fichero temporal
    FILE *temp = fopen("data/temp.dat", "wb");
    if (temp == NULL) {
        perror("Error. Apertura del fichero temporal.\n");
        fclose(file);
        return -1;
    }
    // Lee en usuario en usuario del fichero
    struct user usuario;
    while (fread(&usuario, sizeof(struct user), 1, file)) {
        // Si es el usuario a conectar, modifica el usuario antes de escribirlo
        if (strcmp(usuario.name, nombre) == 0) {
            // Modifica el estado de conectado y agrega la ip y el puerto
            strcpy(usuario.ip, ip);
            usuario.port = puerto;
            usuario.conectado = 1;
        }
        // Escribe el usuario en el fichero temporal
        if (fwrite(&usuario, sizeof(struct user), 1, temp) != 1) {
            perror("Error. Escritura en el fichero temporal.\n");
            fclose(file);
            fclose(temp);
            return -1;
        }
    }
    // Cierra el fichero
    if (fclose(file) != 0) {
        perror("Error. Cierre de la base de datos.\n");
        fclose(temp);
        return -1;
    }
    // Cierra el fichero temporal
    if (fclose(temp) != 0) {
        perror("Error. Cierre del fichero temporal.\n");
        return -1;
    }
    // Elimina el fichero original
    if (remove(BBDD) != 0) {
        perror("Error. Eliminación de la base de datos.\n");
        return -1;
    }
    // Renombra el fichero temporal para que sea el original
    if (rename("data/temp.dat", BBDD) != 0) {
        perror("Error. Renombrado del fichero temporal.\n");
        return -1;
    }
    // Para imprimir los usuarios registrados, descomentar
    // print_usuarios();
    return 0;
}

int desconectar_usuario(char *nombre) {
    /* Modifica el estado de conectado de un usuario y elimina la ip y port */
    // Abre el fichero
    FILE *file = fopen(BBDD, "rb");
    if (file == NULL) {
        perror("Error. Apertura de la base de datos.\n");
        return -1;
    }
    // Abre el fichero temporal
    FILE *temp = fopen("data/temp.dat", "wb");
    if (temp == NULL) {
        perror("Error. Apertura del fichero temporal.\n");
        fclose(file);
        return -1;
    }
    // Lee en usuario en usuario del fichero
    struct user usuario;
    while (fread(&usuario, sizeof(struct user), 1, file)) {
        // Si es el usuario a desconectar, modifica el usuario antes de escribirlo
        if (strcmp(usuario.name, nombre) == 0) {
            // Cambia el estado de conectado y elimina la ip y el puerto
            strcpy(usuario.ip, "");
            usuario.port = 0;
            usuario.conectado = 0;
        }
        // Escribe el usuario en el fichero temporal
        if (fwrite(&usuario, sizeof(struct user), 1, temp) != 1) {
            perror("Error. Escritura en el fichero temporal.\n");
            fclose(file);
            fclose(temp);
            return -1;
        }
    }
    // Cierra el fichero
    if (fclose(file) != 0) {
        perror("Error. Cierre de la base de datos.\n");
        fclose(temp);
        return -1;
    }
    // Cierra el fichero temporal
    if (fclose(temp) != 0) {
        perror("Error. Cierre del fichero temporal.\n");
        return -1;
    }
    // Elimina el fichero original
    if (remove(BBDD) != 0) {
        perror("Error. Eliminación de la base de datos.\n");
        return -1;
    }
    // Renombra el fichero temporal para que sea el original
    if (rename("data/temp.dat", BBDD) != 0) {
        perror("Error. Renombrado del fichero temporal.\n");
        return -1;
    }
    // Para imprimir los usuarios registrados, descomentar
    // print_usuarios();
    return 0;
}

int publicar_archivo(char *nombre, char *archivo, char *descripcion) {
    /* Añadir el archivo al array de ficheros publicados del usuario */
    // Abre el fichero
    FILE *file = fopen(BBDD, "rb");
    if (file == NULL) {
        perror("Error. Apertura de la base de datos.\n");
        return -1;
    }
    // Abre el fichero temporal
    FILE *temp = fopen("data/temp.dat", "wb");
    if (temp == NULL) {
        perror("Error. Apertura del fichero temporal.\n");
        fclose(file);
        return -1;
    }
    // Lee en usuario en usuario del fichero
    struct user usuario;
    while (fread(&usuario, sizeof(struct user), 1, file)) {
        // Si es el usuario que publica, agregar el fichero al array
        if (strcmp(usuario.name, nombre) == 0) {
            int index = usuario.n_files*2;
            usuario.files[index] = strdup(archivo);
            usuario.files[index+1] = strdup(descripcion);
            usuario.n_files++;
        }
        // Escribe el usuario en el fichero temporal
        if (fwrite(&usuario, sizeof(struct user), 1, temp) != 1) {
            perror("Error. Escritura en el fichero temporal.\n");
            fclose(file);
            fclose(temp);
            return -1;
        }
    }
    // Cierra el fichero
    if (fclose(file) != 0) {
        perror("Error. Cierre de la base de datos.\n");
        fclose(temp);
        return -1;
    }
    // Cierra el fichero temporal
    if (fclose(temp) != 0) {
        perror("Error. Cierre del fichero temporal.\n");
        return -1;
    }
    // Elimina el fichero original
    if (remove(BBDD) != 0) {
        perror("Error. Eliminación de la base de datos.\n");
        return -1;
    }
    // Renombra el fichero temporal para que sea el original
    if (rename("data/temp.dat", BBDD) != 0) {
        perror("Error. Renombrado del fichero temporal.\n");
        return -1;
    }
    // Crear el archivo al directorio del usuario
    agregar_contenido(nombre, archivo);
    // Para imprimir los usuarios registrados, descomentar
    // print_usuarios();
    return 0;
}

int eliminar_archivo(char *nombre, char *archivo) {
    /* Eliminar el archivo del array de ficheros publicados del usuario */
    // Abre el fichero
    FILE *file = fopen(BBDD, "rb");
    if (file == NULL) {
        perror("Error. Apertura de la base de datos.\n");
        return -1;
    }
    // Abre el fichero temporal
    FILE *temp = fopen("data/temp.dat", "wb");
    if (temp == NULL) {
        perror("Error. Apertura del fichero temporal.\n");
        fclose(file);
        return -1;
    }
    // Lee en usuario en usuario del fichero
    struct user usuario;
    while (fread(&usuario, sizeof(struct user), 1, file)) {
        // Si es el usuario que publica, eliminar el fichero del array
        if (strcmp(usuario.name, nombre) == 0) {
            // Buscar el archivo y eliminarlo
            for (int i = 0; i < usuario.n_files; i++) {
                if (strcmp(usuario.files[i*2], archivo) == 0) {
                    free(usuario.files[i*2]);
                    free(usuario.files[i*2+1]);
                    for (int j = i; j < usuario.n_files-2; j++) {
                        usuario.files[j*2] = usuario.files[j*2+2];
                        usuario.files[j*2+1] = usuario.files[j*2+3];
                    }
                    break;
                }
            }
            usuario.n_files -= 1;
        }
        // Escribe el usuario en el fichero temporal
        if (fwrite(&usuario, sizeof(struct user), 1, temp) != 1) {
            perror("Error. Escritura en el fichero temporal.\n");
            fclose(file);
            fclose(temp);
            return -1;
        }
    }
    // Cierra el fichero
    if (fclose(file) != 0) {
        perror("Error. Cierre de la base de datos.\n");
        fclose(temp);
        return -1;
    }
    // Cierra el fichero temporal
    if (fclose(temp) != 0) {
        perror("Error. Cierre del fichero temporal.\n");
        return -1;
    }
    // Elimina el fichero original
    if (remove(BBDD) != 0) {
        perror("Error. Eliminación de la base de datos.\n");
        return -1;
    }
    // Renombra el fichero temporal para que sea el original
    if (rename("data/temp.dat", BBDD) != 0) {
        perror("Error. Renombrado del fichero temporal.\n");
        return -1;
    }
    // Eliminar el archivo del directorio del usuario
    eliminar_contenido(nombre, archivo);
    // Para imprimir los usuarios registrados, descomentar
    // print_usuarios();
    return 0;
}

int numero_usuarios_conectados() {
    /* Retorna el numero de usuarios conectados */
    // Abre el fichero
    FILE *file = fopen(BBDD, "rb");
    if (file == NULL) {
        perror("Error. Apertura de la base de datos.\n");
        return -1;
    }
    // Lee en usuario en usuario del fichero
    struct user usuario;
    int i = 0;
    while (fread(&usuario, sizeof(struct user), 1, file)) {
        // Si el usuario esta conectado, incrementa el contador
        if (usuario.conectado == 1) {
            i++;
        }
    }
    // Cierra el fichero
    if (fclose(file) != 0) {
        perror("Error. Cierre de la base de datos.\n");
        return -1;
    }
    // Retorna el numero de usuarios conectados
    return i;
}

char* listar_usuarios_conectados(int numero) {
    /* Lista a todos los usuarios conectados */
    // Abre el fichero
    FILE *file = fopen(BBDD, "rb");
    if (file == NULL) {
        perror("Error. Apertura de la base de datos.\n");
        return NULL;
    }
    // Reserva memoria para la lista con los usuarios conectados
    char *resultado = calloc(1000 * numero, sizeof(char));
    if (resultado == NULL) {
        perror("Error. Reserva de memoria para el resultado.\n");
        return NULL;
    }
    // Lee en usuario en usuario del fichero
    struct user usuario;
    while (fread(&usuario, sizeof(struct user), 1, file)) {
        // Si el usuario esta conectado, lo agrega a la lista
        if (usuario.conectado == 1) {
            // Escribe el usuario con el nombre, la ip y el puerto
            char linea[550];
            sprintf(linea, "%s %s %d ", usuario.name, usuario.ip, usuario.port);
            // Agrega el usuario a la lista
            strcat(resultado, linea);
        }
    }
    // Cierra el fichero
    if (fclose(file) != 0) {
        perror("Error. Cierre de la base de datos.\n");
        free(resultado);
        return NULL;
    }
    // Retorna la lista con los usuarios conectados
    return resultado;
}

int eliminar_base_datos() {
    /* Elimina la base de datos */
    char command[256];
    sprintf(command, "rm -rf %s", DIR);
    if (system(command) == -1) {
        perror("Error al eliminar el directorio data");
        return -1;
    }
    if (mkdir(DIR, 0777) == -1) {
        perror("Error al crear el directorio data");
        return -1;
    }
    return 0;
}

void print_usuarios() {
    /* Imprime los usuarios registrados */
    FILE *file = fopen(BBDD, "rb");
    if (file == NULL) {
        perror("Error. Apertura de la base de datos.\n");
        return;
    }
    struct user usuario;
    while (fread(&usuario, sizeof(struct user), 1, file)) {
        printf("Usuario: %s\n", usuario.name);
        printf("Conectado: %d\n", usuario.conectado);
        printf("IP: %s\n", usuario.ip);
        printf("Puerto: %d\n", usuario.port);
        printf("Archivos publicados:\n");
        for (int i = 0; i < usuario.n_files; i++) {
            printf("  %s: %s\n", usuario.files[i*2], usuario.files[i*2+1]);
        }
    }
    if (fclose(file) != 0) {
        perror("Error. Cierre de la base de datos.\n");
        return;
    }
}