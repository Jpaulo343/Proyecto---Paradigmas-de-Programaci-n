//
// Created by jpaul on 9/18/2026.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "constantes_y_auxiliares.h"

/*
  Recorre la lista enlazada y libera la memoria reservada para el dato y
  la memoria de la estructura del nodo.
*/
void liberarLista(struct Nodo **inicio) {

    struct Nodo *actual = *inicio;

    while (actual != NULL) {
        struct Nodo *temp = actual;
        actual = actual->siguiente;

        free(temp->dato); // se libera la memoria del dato
        free(temp); // se libera la memoria del nodo
    }

    *inicio = NULL;
}

/*
  Reserva memoria en el Heap para un nuevo nodo y para su dato generico,
  realiza una copia exacta de los bytes del puntero dado usando memcpy
  y lo enlaza al inicio de la lista.
*/
void insertarInicio(struct Nodo **inicio, void *dato, size_t tamanoDato) {
    struct Nodo *nuevo = malloc(sizeof(Nodo));

    if (nuevo == NULL) {
        printf("Error: no se pudo reservar memoria.\n");
        return;
    }

    nuevo->dato = malloc(tamanoDato); //al ser punteros se debe reservar el espacio de memoria del dato por aparte
    if (!nuevo->dato) {
        free(nuevo);
        printf("Error: no se pudo reservar memoria para el dato.\n");
        return;
    }

    memcpy(nuevo->dato, dato, tamanoDato); //copia los datos del puntero en el nodo

    nuevo->siguiente = *inicio;
    *inicio = nuevo;
}

/*
  Igual que insertarInicio pero inserta al final de la lista enlazada.
*/
void insertarFinal(struct Nodo **inicio, void *dato, size_t tamanoDato) {
    struct Nodo *nuevo = malloc(sizeof(Nodo));

    if (nuevo == NULL) {
        printf("Error: no se pudo reservar memoria.\n");
        return;
    }

    nuevo->dato = malloc(tamanoDato);
    if (!nuevo->dato) {
        free(nuevo);
        printf("Error: no se pudo reservar memoria para el dato.\n");
        return;
    }

    memcpy(nuevo->dato, dato, tamanoDato);
    nuevo->siguiente = NULL; // al ser el el ultimo, no apunta a ningun nodo

    // si la lista esta vacia, el nuevo nodo es el inicio
    if (*inicio == NULL) {
        *inicio = nuevo;
        return;
    }

    struct Nodo *actual = *inicio;
    while (actual->siguiente != NULL) { // se avanza hasta el ultimo nodo
        actual = actual->siguiente;
    }
    actual->siguiente = nuevo;
}

/*
  Recorre el texto y cambia por '\0' el primer '\n' o '\r' que encuentre
  los archivos exportados desde excel terminan con un salto de linea, lo cual puede causar problemas al comparar
*/
void quitarSaltoLinea(char *texto) {
    int i = 0;
    while (*(texto + i) != '\0') {
        if (*(texto + i) == '\n' || *(texto + i) == '\r') {
            *(texto + i) = '\0';
            break;
        }
        i++;
    }
}

/*
  Funciona parecudo a strtok, pero no se salta los campos vacios, es decir, busca el siguiente ';', lo cambia por '\0' 
  y deja el cursor al inicio del siguiente campo.
*/
char *separarCampo(char **cursor) {
    char *campo = *cursor;
    if (campo == NULL) {
        return NULL;
    }

    char *separador = strchr(campo, SEPARADOR);
    if (separador != NULL) {
        *separador = '\0';
        *cursor = separador + 1;
    } else {
        *cursor = NULL; // era el ultimo campo
    }
    return campo;
}
 
/*
  Abre y procesa el archivo CSV con los datos del plan de estudios,
  separa cada columna mediante separarCampo con delimitador ';', llena los campos
  del struct Curso e inserta cada curso en la lista enlazada dada.
*/
void cargar_plan_estudios(struct Nodo **inicio, const char *ruta) {
    FILE *plan_estudios = fopen(ruta,"r");
    if (!plan_estudios) {
        printf("No se puede abrir el plan de estudios\n");
        return;
    }
    char buffer[TAM_LINEA]; //se define el tamaño de linea

    fgets(buffer, TAM_LINEA, plan_estudios); // se lee y se ignora la primera linea por ser el encabezado

    //while que recorre cada linea del archivo .csv
    while (fgets(buffer,TAM_LINEA,plan_estudios)) {// el fgets no retorna False hasta que se termine de recorrer el .csv
       if (buffer[0] == '\n' || buffer[0] == '\r') {
            continue; // se ignoran las lineas vacias
        }

        struct Curso c = {0}; // se inicializa con 0 para evitar basura en memoria

        char *cursor = buffer;
        char *columna = separarCampo(&cursor); //se separa el texto en columnas por cada ";"

        if (columna) {
            strncpy(c.codigo, columna, TAM_CODIGO - 1);
        }

        columna = separarCampo(&cursor);
        if (columna) {
            strncpy(c.nombre, columna, TAM_NOMBRE - 1);
        }

        columna = separarCampo(&cursor);
        if (columna) {
            c.creditos = atoi(columna); //se convierte el texto a int
        }

        columna = separarCampo(&cursor);
        if (columna) {
            c.horas = atoi(columna);
        }

        columna = separarCampo(&cursor);
        if (columna) {
            c.semestre = atoi(columna);
        }

        columna = separarCampo(&cursor);
        if (columna) {
            strncpy(c.requisitos, columna, TAM_REQUISITOS - 1);
        }

        columna = separarCampo(&cursor);
        if (columna) {
            strncpy(c.correquisitos, columna, TAM_CORREQUISITOS - 1);
        }

        columna = separarCampo(&cursor); //se utiliza la funcion separarCampo para no saltarse los campos vacios

        // Eliminar el salto de linea ('\n') del ultimo campo si existe
        if (columna) {
            strcpy(c.tipo, columna);
            quitarSaltoLinea(c.tipo);
        }

        insertarFinal(inicio, &c, sizeof(struct Curso)); //guardar en estructura de datos utilizando la referencia del objeto
    }
    fclose(plan_estudios); //se libera la memoria
}

