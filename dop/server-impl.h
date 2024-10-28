#ifndef SERVERIMPL_H
#define SERVERIMPL_H


int register_user(char *name);
int unregister_user(char *name);
int connect_user(char *name, char *ip, int port);
int disconnect_user(char *name);
int publish_file(char *name, char *file, char *description);
int delete_file(char *name, char *file);
int list_users(char *name, char **usuarios_conectados);
int list_content(char *name, char *r_name, char **archivos);
void iniciar_servicio();
void finalizar_servicio();

#endif

