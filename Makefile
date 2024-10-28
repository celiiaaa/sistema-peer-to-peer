BIN_FILES = server server_rpc
CC = gcc
CFLAGS = -Wall -Werror -fPIC -g -I/usr/include/tirpc
DEPS = dop/lines.h dop/server-impl.h dop/user.h
OBJ = server.o dop/lines.o dop/server-impl.o dop/user.o rpc/operacion_xdr.o rpc/operacion_clnt.o
OBJ_RPC_SERVER = rpc/operacion_svc.o rpc/operacion_server.o rpc/operacion_xdr.o


# Opciones del preprocesador
CPPFLAGS = -I$(INSTALL_PATH)/include

# Opciones del enlazador
LDFLAGS = -L$(INSTALL_PATH)/lib/

# Bibliotecas a enlazar
LIBS = -lnsl -lpthread -ldl -ltirpc -lm

# Regla para generar los ejecutables
all: $(BIN_FILES)
	@echo "***************************"
	@echo "Compilation successfully!"
	@echo "***************************"

# Reglas de construcción de los ejecutables
server: $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^ $(CFLAGS) $(LIBS)

server_rpc: $(OBJ_RPC_SERVER)
	$(CC) $(LDFLAGS) -o $@ $^ $(CFLAGS) $(LIBS)

# Reglas de construcción de los objetos
%.o: %.c $(DEPS)
	$(CC) $(CPPFLAGS) -c -o $@ $< $(CFLAGS)

rpc/%.o: rpc/%.c
	$(CC) $(CPPFLAGS) -c -o $@ $< $(CFLAGS)

# Regla para borrar todos los archivos generados
clean:
	rm -f $(BIN_FILES) *.o dop/*.o rpc/*.o
	@echo "***************************"
	@echo "Files deleted successfully!"
	@echo "***************************"

.PHONY: all clean
