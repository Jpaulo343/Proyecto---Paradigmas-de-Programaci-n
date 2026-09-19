//
// Created by jpaul on 9/18/2026.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
  Abre y procesa el archivo CSV con los datos del plan de estudios,
  separa cada columna mediante strtok con delimitador ';', llena los campos
  del struct Curso e inserta cada curso en la lista enlazada dada.
*/
void cargar_plan_estudios(struct Nodo **inicio) {
    FILE *plan_estudios = fopen("data/plan_CE.csv","r");
    if (!plan_estudios) {
        printf("No se puede abrir el plan de estudios\n");
        return;
    }
    char buffer[150]; //se define el tamaño de linea

    //while que recorre cada linea del archivo .csv
    while (fgets(buffer,150,plan_estudios)) {// el fgets no retorna False hasta que se termine de recorrer el .csv
        struct Curso c;

        char *columna = strtok(buffer, ";"); //se separa el texto po en columnas por cada ";"

        if (columna) {
            strcpy(c.codigo, columna);
        }

        columna = strtok(NULL, ";");
        if (columna) {
            strcpy(c.nombre, columna);
        }

        columna = strtok(NULL, ";");
        if (columna) {
            c.creditos = atoi(columna); //se convierte el texto a int
        }

        columna = strtok(NULL, ";");
        if (columna) {
            c.horas = atoi(columna);
        }

        columna = strtok(NULL, ";");
        if (columna) {
            c.semestre = atoi(columna);
        }

        columna = strtok(NULL, ";");
        if (columna) {
            strcpy(c.requisitos, columna);
        }

        columna = strtok(NULL, ";");
        if (columna) {
            strcpy(c.correquisitos, columna);
        }

        columna = strtok(NULL, ";");

        // Eliminar el salto de linea ('\n') del ultimo campo si existe
        if (columna) {
            strcpy(c.tipo, columna);
            int i = 0;
            while (*(c.tipo + i) != '\0') {
                if (*(c.tipo + i) == '\n') {
                    *(c.tipo + i) = '\0';
                    break;
                }
                i++;
            }
        }

        insertarInicio(inicio, &c, sizeof(struct Curso)); //guardar en estructura de datos utilizando la referencia del objeto
    }
    fclose(plan_estudios); //se libera la memoria
}

