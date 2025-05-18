#include <stdio.h>
#include <string.h>
#include "funciones.h"

int main() {
    int opcion;
    do {
        printf("\n=== Menu Principal ===\n");
        printf("1) Ingresar productos\n");
        printf("2) Ingresar recursos\n");
        printf("3) Calcular produccion\n");
        printf("4) Reabastecer recurso\n");
        printf("5) Eliminar producto o recurso\n");
        printf("6) Editar producto o recurso\n");
        printf("7) Listar datos\n");
        printf("8) Eliminar todo\n");
        printf("9) Salir\n");

        opcion = leerEntero("Seleccione una opcion: ", 1, 7);

        switch (opcion) {
            case 1: ingresarProductos(); break;
            case 2: ingresarRecursos(); break;
            case 3: calcularProduccion(); break;
            case 4: reabastecerRecurso(); break;
            case 5: eliminarElemento(); break;
            case 6: editarElemento(); break;
            case 7: listarDatos(); break;
            case 8: eliminarTodo(); break;
            case 9: printf("Adios.\n"); break;

        }
    } while (opcion != 7);
    return 0;
}