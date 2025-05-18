#include <stdio.h>
#include <string.h>
#include "funciones.h"
#define MAX_PRODUCTOS 5
#define MAX_RECURSOS 5
#define MAX_NOMBRE 50


char nombresProductos[MAX_PRODUCTOS][MAX_NOMBRE];
int demandaProductos[MAX_PRODUCTOS];
int tiempoProductos[MAX_PRODUCTOS];
int numeroProductos = 0;

char nombresRecursos[MAX_RECURSOS][MAX_NOMBRE];
int stockRecursos[MAX_RECURSOS];
int requerimientos[MAX_PRODUCTOS][MAX_RECURSOS];
int numeroRecursos = 0;

int ingresoProductosBloqueado = 0;
int ingresoRecursosBloqueado = 0;

void limpiarBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int leerEntero(const char *msg, int min, int max) {
    int val;
    char term;
    while (1) {
        printf("%s", msg);
        if (scanf("%d%c", &val, &term) != 2 || term != '\n') {
            printf("  >> Entrada invalida. Ingrese un numero entero.\n");
            limpiarBuffer();
            continue;
        }
        if (val < min || val > max) {
            printf("  >> Debe estar entre %d y %d.\n", min, max);
            continue;
        }
        return val;
    }
}

void leerCadena(const char *msg, char *dest, int maxLen) {
    while (1) {
        printf("%s", msg);
        if (!fgets(dest, maxLen, stdin)) continue;
        size_t len = strlen(dest);
        if (len > 0 && dest[len - 1] == '\n') dest[len - 1] = '\0';
        if (strlen(dest) == 0) {
            printf("  >> No puede estar vacio.\n");
            continue;
        }
        return;
    }
}

int cadenasIguales(const char *a, const char *b) {
    while (*a && *b) {
        if (*a != *b) return 0;
        a++;
        b++;
    }
    return *a == '\0' && *b == '\0';
}

void ingresarProductos() {
    if (ingresoProductosBloqueado) {
        printf("Ya no es posible ingresar mas productos.\n");
        return;
    }
    int disponibles = MAX_PRODUCTOS - numeroProductos;
    if (disponibles == 0) {
        ingresoProductosBloqueado = 1;
        printf("Ya se ingreso el máximo de productos.\n");
        return;
    }
    printf("\n=== Ingreso de Productos (Tarjetas Graficas) ===\n");
    printf("Puede ingresar hasta %d productos mas.\n", disponibles);
    int cantidad = leerEntero("¿Cuantos desea ingresar? ", 1, disponibles);

    for (int i = 0; i < cantidad; i++) {
        printf("\nProducto #%d:\n", numeroProductos + 1);
        leerCadena("Nombre del producto: ", nombresProductos[numeroProductos], MAX_NOMBRE);
        demandaProductos[numeroProductos] = leerEntero("Demanda esperada: ", 1, 10000);
        tiempoProductos[numeroProductos] = leerEntero("Tiempo por unidad (horas): ", 1, 1000);
        numeroProductos++;
    }
    if (numeroProductos == MAX_PRODUCTOS) {
        ingresoProductosBloqueado = 1;
        printf("\nSe alcanzo el límite de productos.\n");
    }
}

void ingresarRecursos() {
    if (numeroProductos == 0) {
        printf("Primero debe ingresar al menos un producto.\n");
        return;
    }
    if (ingresoRecursosBloqueado) {
        printf("Ya no es posible ingresar mas recursos.\n");
        return;
    }
    int disponibles = MAX_RECURSOS - numeroRecursos;
    if (disponibles == 0) {
        ingresoRecursosBloqueado = 1;
        printf("Ya se ingreso el maximo de recursos.\n");
        return;
    }
    printf("\n=== Ingreso de Recursos ===\n");
    printf("Puede ingresar hasta %d recursos mas.\n", disponibles);
    int cantidad = leerEntero("¿Cuántos desea ingresar? ", 1, disponibles);

    for (int i = 0; i < cantidad; i++) {
        printf("\nRecurso #%d:\n", numeroRecursos + 1);
        leerCadena("Nombre del recurso: ", nombresRecursos[numeroRecursos], MAX_NOMBRE);
        for (int j = 0; j < numeroProductos; j++) {
            char mensaje[100];
            snprintf(mensaje, sizeof(mensaje), "¿Cuantos '%s' necesita '%s'? ", nombresRecursos[numeroRecursos], nombresProductos[j]);
            requerimientos[j][numeroRecursos] = leerEntero(mensaje, 0, 10000);
        }
        stockRecursos[numeroRecursos] = leerEntero("Stock disponible de este recurso: ", 0, 100000);
        numeroRecursos++;
    }
    if (numeroRecursos == MAX_RECURSOS) {
        ingresoRecursosBloqueado = 1;
        printf("\nSe alcanzo el limite de recursos.\n");
    }
}

void fabricarProducto(int indice) {
    for (int j = 0; j < numeroRecursos; j++) {
        stockRecursos[j] -= requerimientos[indice][j] * demandaProductos[indice];
    }
    printf("  => Produccion de '%s' realizada. Stock actualizado.\n", nombresProductos[indice]);
}

void calcularProduccion() {
    if (numeroProductos == 0 || numeroRecursos == 0) {
        printf("Debe haber al menos un producto y un recurso ingresado.\n");
        return;
    }

    printf("\n=== Calculo de Produccion ===\n");
    int horasDisponibles = leerEntero("Ingrese las horas disponibles de produccion: ", 1, 100000);
    int algunViable = 0;

    for (int i = 0; i < numeroProductos; i++) {
        printf("\nProducto: %s\n", nombresProductos[i]);

        int tiempoTotal = tiempoProductos[i] * demandaProductos[i];
        int tiempoOK = tiempoTotal <= horasDisponibles;
        printf("  Tiempo requerido: %d horas (Disponible: %d) => %s\n",
               tiempoTotal, horasDisponibles,
               tiempoOK ? "OK" : "NO");

        int puede = tiempoOK;
        for (int j = 0; j < numeroRecursos; j++) {
            int totalNecesario = requerimientos[i][j] * demandaProductos[i];
            printf("  Recurso '%s': necesita %d, stock %d => %s\n",
                   nombresRecursos[j], totalNecesario,
                   stockRecursos[j], totalNecesario <= stockRecursos[j] ? "OK" : "NO");
            if (totalNecesario > stockRecursos[j]) puede = 0;
        }

        printf("  => %s produccion\n", puede ? "VIABLE" : "NO VIABLE");

        if (puede) {
            algunViable = 1;
            char respuesta;
            char linea[10];
            printf("¿Desea fabricar '%s'? (s/n): ", nombresProductos[i]);
            fgets(linea, sizeof(linea), stdin);
            respuesta = linea[0];

            if (respuesta == 's' || respuesta == 'S') {
                fabricarProducto(i);
            } else {
                printf("  => Produccion no realizada.\n");
            }
        }
    }

    if (!algunViable) {
        printf("\nNo hay productos viables para fabricar con los recursos y tiempo disponibles.\n");
    }
}

void reabastecerRecurso() {
    if (numeroRecursos == 0) {
        printf("No hay recursos para reabastecer.\n");
        return;
    }
    char nombre[MAX_NOMBRE];
    leerCadena("Ingrese el nombre del recurso a reabastecer: ", nombre, MAX_NOMBRE);
    for (int i = 0; i < numeroRecursos; i++) {
        if (cadenasIguales(nombre, nombresRecursos[i])) {
            int cantidad = leerEntero("¿Cuánto desea añadir al stock? ", 1, 100000);
            stockRecursos[i] += cantidad;
            printf("Stock actualizado. Nuevo stock de '%s': %d\n", nombre, stockRecursos[i]);
            return;
        }
    }
    printf("Recurso no encontrado.\n");
}

void eliminarElemento() {
    if (numeroProductos == 0 && numeroRecursos == 0) {
        printf("No hay productos ni recursos para eliminar.\n");
        return;
    }
    int tipo = leerEntero("¿Desea eliminar producto (1) o recurso (0)? ", 0, 1);

    if (tipo == 1) {
        printf("\nProductos:\n");
        for (int i = 0; i < numeroProductos; i++) {
            printf("  - %s\n", nombresProductos[i]);
        }
    } else {
        printf("\nRecursos:\n");
        for (int i = 0; i < numeroRecursos; i++) {
            printf("  - %s\n", nombresRecursos[i]);
        }
    }

    char nombre[MAX_NOMBRE];
    leerCadena("\nIngrese el nombre a eliminar: ", nombre, MAX_NOMBRE);

    if (tipo == 1) {
        for (int i = 0; i < numeroProductos; i++) {
            if (cadenasIguales(nombre, nombresProductos[i])) {
                for (int j = i; j < numeroProductos - 1; j++) {
                    strcpy(nombresProductos[j], nombresProductos[j + 1]);
                    demandaProductos[j] = demandaProductos[j + 1];
                    tiempoProductos[j] = tiempoProductos[j + 1];
                    for (int k = 0; k < numeroRecursos; k++) {
                        requerimientos[j][k] = requerimientos[j + 1][k];
                    }
                }
                numeroProductos--;
                ingresoProductosBloqueado = 0;
                printf("Producto eliminado.\n");
                return;
            }
        }
        printf("Producto no encontrado.\n");
    } else {
        for (int i = 0; i < numeroRecursos; i++) {
            if (cadenasIguales(nombre, nombresRecursos[i])) {
                for (int j = i; j < numeroRecursos - 1; j++) {
                    strcpy(nombresRecursos[j], nombresRecursos[j + 1]);
                    stockRecursos[j] = stockRecursos[j + 1];
                    for (int k = 0; k < numeroProductos; k++) {
                        requerimientos[k][j] = requerimientos[k][j + 1];
                    }
                }
                numeroRecursos--;
                ingresoRecursosBloqueado = 0;
                printf("Recurso eliminado.\n");
                return;
            }
        }
        printf("Recurso no encontrado.\n");
    }
}

void editarElemento() {
    int tipo = leerEntero("¿Desea editar producto (1) o recurso (0)? ", 0, 1);

    if (tipo == 1 && numeroProductos > 0) {
        printf("\nProductos:\n");
        for (int i = 0; i < numeroProductos; i++) {
            printf("  - %s\n", nombresProductos[i]);
        }

        char nombre[MAX_NOMBRE];
        leerCadena("\nIngrese el nombre del producto a editar: ", nombre, MAX_NOMBRE);
        for (int i = 0; i < numeroProductos; i++) {
            if (cadenasIguales(nombre, nombresProductos[i])) {
                printf("Editando '%s'\n", nombresProductos[i]);
                leerCadena("Nuevo nombre: ", nombresProductos[i], MAX_NOMBRE);
                demandaProductos[i] = leerEntero("Nueva demanda esperada: ", 1, 10000);
                tiempoProductos[i] = leerEntero("Nuevo tiempo por unidad (horas): ", 1, 1000);
                printf("Producto editado correctamente.\n");
                return;
            }
        }
        printf("Producto no encontrado.\n");
    } else if (tipo == 0 && numeroRecursos > 0) {
        printf("\nRecursos:\n");
        for (int i = 0; i < numeroRecursos; i++) {
            printf("  - %s\n", nombresRecursos[i]);
        }

        char nombre[MAX_NOMBRE];
        leerCadena("\nIngrese el nombre del recurso a editar: ", nombre, MAX_NOMBRE);
        for (int i = 0; i < numeroRecursos; i++) {
            if (cadenasIguales(nombre, nombresRecursos[i])) {
                printf("Editando '%s'\n", nombresRecursos[i]);
                leerCadena("Nuevo nombre: ", nombresRecursos[i], MAX_NOMBRE);
                stockRecursos[i] = leerEntero("Nuevo stock: ", 0, 100000);
                for (int j = 0; j < numeroProductos; j++) {
                    char mensaje[100];
                    snprintf(mensaje, sizeof(mensaje), "¿Cuántos '%s' necesita '%s'? ", nombresRecursos[i], nombresProductos[j]);
                    requerimientos[j][i] = leerEntero(mensaje, 0, 10000);
                }
                printf("Recurso editado correctamente.\n");
                return;
            }
        }
        printf("Recurso no encontrado.\n");
    } else {
        printf("No hay elementos para editar.\n");
    }
}

void listarDatos() {
    if (numeroProductos == 0 && numeroRecursos == 0) {
        printf("No hay productos ni recursos ingresados para mostrar.\n");
        return;
    }

    printf("\n=== Listado de Productos ===\n");
    if (numeroProductos == 0) {
        printf("No hay productos ingresados.\n");
    } else {
        for (int i = 0; i < numeroProductos; i++) {
            printf("Producto #%d: %s\n", i + 1, nombresProductos[i]);
            printf("  Demanda esperada: %d\n", demandaProductos[i]);
            printf("  Tiempo por unidad (horas): %d\n", tiempoProductos[i]);
            printf("  Requerimientos de recursos:\n");
            for (int j = 0; j < numeroRecursos; j++) {
                printf("    - %s: %d\n", nombresRecursos[j], requerimientos[i][j]);
            }
            printf("\n");
        }
    }

    printf("\n=== Listado de Recursos ===\n");
    if (numeroRecursos == 0) {
        printf("No hay recursos ingresados.\n");
    } else {
        for (int i = 0; i < numeroRecursos; i++) {
            printf("Recurso #%d: %s\n", i + 1, nombresRecursos[i]);
            printf("  Stock disponible: %d\n", stockRecursos[i]);
            printf("\n");
        }
    }
}

void eliminarTodo() {
    numeroProductos = 0;
    numeroRecursos = 0;
    ingresoProductosBloqueado = 0;
    ingresoRecursosBloqueado = 0;

    // Limpiar los nombres de productos y recursos
    for (int i = 0; i < MAX_PRODUCTOS; i++) {
        nombresProductos[i][0] = '\0';
        demandaProductos[i] = 0;
        tiempoProductos[i] = 0;
        for (int j = 0; j < MAX_RECURSOS; j++) {
            requerimientos[i][j] = 0;
        }
    }
    for (int i = 0; i < MAX_RECURSOS; i++) {
        nombresRecursos[i][0] = '\0';
        stockRecursos[i] = 0;
    }

    printf("Todos los datos fueron eliminados. El programa ha sido reiniciado.\n");
}
