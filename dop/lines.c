#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include "lines.h"

int sendMessage(int socket, char * buffer, int len) {
	int r;
	int l = len;

	do {
		r = write(socket, buffer, l);
		l = l -r;
		buffer = buffer + r;
	} while ((l>0) && (r>=0));
	
	if (r < 0) {
		return (-1);   /* fail */
	} else {
		return(0);	/* full length has been sent */
	}
}

int recvMessage(int socket, char *buffer, int len) {
	int r;
	int l = len;
	char *p = buffer;

	do {	
		r = read(socket, p, 1);
        l = l - r ;
        p = p + r;
	} while ((l > 0) && (r >= 0) && (*(p-1) != '\0'));  
	
	if (r < 0) {
		return (-1);   /* fallo */
	} else {
		return(0);	/* full length has been receive */
	}
}

