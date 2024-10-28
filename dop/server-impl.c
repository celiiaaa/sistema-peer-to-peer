#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "user.h"

int register_user(char *name) {
    /* Registrar al usuario */
    int res;

    // Crear el usuario
    struct user usuario;
    strcpy(usuario.name, name);
    usuario.conectado = 0;
    strcpy(usuario.ip, "");
    usuario.port = 0;
    usuario.n_files = 0;
    usuario.files[0] = NULL;

    // Comprobar si el usuario ya está registrado
    if ((res = (buscar_usuario(name))) == 1) {
        // Usuario ya registrado
        return 1;
    } else if (res < 0) {
        // Error en la búsqueda
        return 2;
    } else {
        // Usuario no registrado
        if (agregar_usuario(&usuario) < 0) {
            // Error en la inserción
            return 2;
        }
    }

    return 0;
}

int unregister_user(char *name) {
    /* Desregistrar al usuario */
    int res;

    // Comprobar si el usuario ya está registrado
    if ((res = (buscar_usuario(name))) == 0) {
        // Usuario no registrado
        return 1;
    } else if (res < 0) {
        // Error en la búsqueda
        return 2;
    } else {
        // Usuario no registrado
        if (eliminar_usuario(name) < 0) {
            // Error en la eliminación
            return 2;
        }
    }

    return 0;
}

int connect_user(char *name, char *ip, int port) {
    /* Conectar al usuario */
    
    // Comprobar si el usuario está registrado   
    struct user *usuario;
    usuario = obtener_usuario(name);
    if (usuario == NULL) {
        // Usuario no registrado
        return 1;
    }

    // Comprobar si el usuario está conectado
    if (usuario->conectado == 1) {
        // Usuario ya conectado
        return 2;
    }
    
    // Conectar al usuario
    if (conectar_usuario(name, ip, port) < 0) {
        // Error en la conexión
        return 3;
    }

    return 0;
}

int disconnect_user(char *name) {
    /* Desconectar al usuario */

    // Comprobar si el usuario está registrado
    struct user *usuario;
    usuario = obtener_usuario(name);
    if (usuario == NULL) {
        // Usuario no registrado
        return 1;
    }

    // Comprobar si el usuario está desconectado
    if (usuario->conectado == 0) {
        // Usuario no conectado
        return 2;
    }

    // Desconectar al usuario
    if (desconectar_usuario(name) < 0) {
        // Error en la desconexión
        return 3;
    }
    return 0;
}

int publish_file(char *name, char *file, char *description) {
    /* Publicar un archivo */

    // Comprobar si el usuario está registrado   
    struct user *usuario;
    usuario = obtener_usuario(name);
    if (usuario == NULL) {
        // Usuario no registrado
        return 1;
    }

    // Comprobar si el usuario está conectado
    if (usuario->conectado == 0) {
        // Usuario no conectado
        return 2;
    }

    // Comprobar si el archivo ya está publicado
    for (int i = 0; i < usuario->n_files; i++) {
        if (strcmp(usuario->files[i*2], file) == 0) {
            return 3;
        }
    }

    // Publicar el archivo
    if (publicar_archivo(name, file, description) < 0) {
        // Error en la publicación
        return 4;
    }

    return 0;
}

int delete_file(char *name, char *file) {
    /* Eliminar un archivo */

    // Comprobar si el usuario está registrado   
    struct user *usuario;
    usuario = obtener_usuario(name);
    if (usuario == NULL) {
        // Usuario no registrado
        return 1;
    }

    // Comprobar si el usuario está conectado
    if (usuario->conectado == 0) {
        // Usuario no conectado
        return 2;
    }

    // Comprobar si el archivo está publicado
    int existe = 0;
    for (int i = 0; i < usuario->n_files; i++) {
        if (strcmp(usuario->files[i*2], file) == 0) {
            existe = 1;
            break;
        }
    }
    if (!existe) {
        return 3;
    }

    // Eliminar el archivo
    if (eliminar_archivo(name, file) < 0) {
        // Error en la eliminación
        return 4;
    }

    return 0;
}

int list_users(char *name, char **usuarios_conectados) {
    /* Listar los usuarios */
    struct user *usuario;
    usuario = obtener_usuario(name);
    if (usuario == NULL) {
        // Usuario no registrado
        return -1;
    }

    // Verificar si el usuario está conectado
    if (usuario->conectado == 0) {
        // Usuario no conectado
        return -2;
    }

    int num_usuarios = numero_usuarios_conectados();
    // printf("Número de usuarios conectados: %d\n", num_usuarios);
    if (num_usuarios > 0) {
        *usuarios_conectados = listar_usuarios_conectados(num_usuarios);
        //printf("USUARIOS CONECTADOS: %s\n", *usuarios_conectados);
        if (usuarios_conectados == NULL) {
            // Error en la obtención de los usuarios
            return -3;
        }
    }

    return num_usuarios;
}

int list_content(char *name, char *r_name, char **files) {
    /* Listar los archivos */
    struct user *usuario;
    usuario = obtener_usuario(name);
    if (usuario == NULL) {
        // Usuario no registrado
        return -1;
    }

    // Verificar si el usuario está conectado
    if (usuario->conectado == 0) {
        // Usuario no conectado
        return -2;
    }

    // Verificar si el usuario a listar está registrado
    struct user *usuario2;
    usuario2 = obtener_usuario(r_name);
    if (usuario2 == NULL) {
        // Usuario no registrado
        return -3;
    }

    // Obtener los archivos publicados del usuario
    int num_archivos = usuario2->n_files;
    if (num_archivos > 0) {
        // Reservar memoria para la cadena con todos los archivos
        *files = (char *) calloc(1000 * num_archivos, sizeof(char));
        if (files == NULL) {
            // Error en reservar memoria
            return -4;
        }
        // Concatenar los archivos
        for (int i = 0; i < num_archivos; i++) {
            char linea[550];
            sprintf(linea, "%s: %s | ", usuario2->files[i*2], usuario2->files[i*2+1]);
            strcat(*files, linea);
        }
    }

    return num_archivos;
}

void iniciar_servicio() {
    /* Iniciar el servicio */
    iniciar_base_datos();
}

void finalizar_servicio() {
    /* Finalizar el servicio */
    eliminar_base_datos();
}