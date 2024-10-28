const MAX_OP = 20;
const MAX_STR = 256;

struct Operacion {
    string username<MAX_STR>;
    string op<MAX_OP>;
    string datetime<MAX_STR>;
    string filename<MAX_STR>;
};

program IMPRIMIR_OPERACION {
    version IMPRIMIR_OPERACION_V1 {
        void print_operacion(struct Operacion) = 1;
    } = 1;
} = 500;
