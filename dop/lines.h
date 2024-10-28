#include <unistd.h>

int sendMessage(int socket, char *buffer, int len);
int sendMessage2(int socket, char *buffer, int len);
int recvMessage(int socket, char *buffer, int len);
