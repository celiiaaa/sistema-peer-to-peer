#!/bin/bash

# Definir el servidor y el puerto por defecto
DEFAULT_SERVER=localhost
DEFAULT_PORT=8080

gnome-terminal -- bash -c "python3 client.py -s $DEFAULT_SERVER -p $DEFAULT_PORT; exec bash"
gnome-terminal -- bash -c "python3 ws-client.py; exec bash"
