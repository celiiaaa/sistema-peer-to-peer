#!/bin/bash

make

# Definir el servidor y el puerto por defecto
DEFAULT_PORT=8080

gnome-terminal -- bash -c "./server -p $DEFAULT_PORT; exec bash"
gnome-terminal -- bash -c "./server_rpc; exec bash"
