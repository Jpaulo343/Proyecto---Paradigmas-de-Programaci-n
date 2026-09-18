//
// Created by jpaul on 9/18/2026.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void liberarLista(struct Nodo **inicio) {

    struct Nodo *actual = *inicio;

    while (actual != NULL) {
        Nodo *temp = actual;
        actual = actual->siguiente;
        free(temp);
    }

    *inicio = NULL;
}

void insertarInicio(struct Nodo **inicio,struct Curso curso) {

    struct Nodo *nuevo = malloc(sizeof(Nodo));

    if (nuevo == NULL) {
        printf("Error: no se pudo reservar memoria.\n");
        return;
    }

    nuevo->curso = curso;
    nuevo->siguiente = *inicio;
    *inicio = nuevo;
}


void cargar_plan_estudios(struct Nodo **inicio) {
// Esta función convierte los datos de "data/plan_CE.csv" en objetos de curso
// y los guarda en la estructura de datos dada
    FILE *plan_estudios = fopen("data/plan_CE.csv","r")
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
        if (columna) {
            strcpy(c.tipo, columna);
            int i = 0;
            while (*(c.tipo + i) != '\0') {
                if (*(c.tipo + i) == '\n') { //se elimina el salto de linea
                    *(c.tipo + i) = '\0';
                    break;
                }
                i++;
            }
        }

        insertarInicio(inicio, c); //guardar en estructura de datos
    }
    fclose(plan_estudios); //se libera la memoria
}

