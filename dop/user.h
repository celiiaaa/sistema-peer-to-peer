#ifndef USER_H
#define USER_H


struct user {
    int id;
    char name[256];
    int conectado;
    char ip[256];
    int port;
    int n_files;
    char *files[256];
};

void crear_usuario(char *nombre_usuario);
void borrar_usuario(char *nombre_usuario);
void agregar_contenido(char *nombre_usuario, char *nombre_archivo);
void eliminar_contenido(char *nombre_usuario, char *nombre_archivo);
int iniciar_base_datos();
int agregar_usuario(struct user *u);
int buscar_usuario(char *nombre);
struct user *obtener_usuario(char *nombre);
int conectar_usuario(char *nombre, char *ip, int port);
int desconectar_usuario(char *nombre);
int publicar_archivo(char *nombre, char *archivo, char *descripcion);
int eliminar_archivo(char *nombre, char *archivo);
int eliminar_usuario(char *nombre);
int numero_usuarios_conectados();
char *listar_usuarios_conectados(int numero);
int eliminar_base_datos();
void print_usuarios();

#endif 
