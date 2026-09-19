#include <stdio.h>
#include "constantes_y_auxiliares.h"

/*
  Recorre la lista de cursos e imprime los datos de cada uno para 
  revisar que se cargaron bien
*/
void mostrar_plan(struct Nodo *inicio) {
    struct Nodo *actual = inicio;
    int cantidad = 0;

    while (actual != NULL) {
        struct Curso *c = (struct Curso *) actual->dato;

        printf("%s | %s | creditos: %d | semestre: %d | requisitos: [%s] | correquisitos: [%s] | tipo: %s\n",
               c->codigo, c->nombre, c->creditos, c->semestre, c->requisitos, c->correquisitos, c->tipo);

        cantidad++;
        actual = actual->siguiente;
    }
    printf("Total de cursos: %d\n", cantidad);
}

int main(void) {
    struct Nodo *planCE = NULL;
    struct Nodo *planPI = NULL;

    printf("\n=== Plan de Ingenieria en Computadores ===\n");
    cargar_plan_estudios(&planCE, RUTA_PLAN_CE);
    mostrar_plan(planCE);

    printf("\n=== Plan de Ingenieria en Informatica ===\n");
    cargar_plan_estudios(&planPI, RUTA_PLAN_PI);
    mostrar_plan(planPI);

    // Liberar memoria de las listas
    liberarLista(&planCE);
    liberarLista(&planPI);
    return 0;
}
    