#include <stdio.h>
#include "constantes_y_auxiliares.h"

void mostrar_plan(struct Nodo *inicio) {
    struct Nodo *actual = inicio;
    int cantidad = 0;

    while (actual != NULL) {
        struct Curso *c = (struct Curso *) actual->dato;

        printf("%s | %s | creditos: %d | semestre: %d | grupos: %d | choques: %s | requisitos: [%s] | correquisitos: [%s] | tipo: %s\n",
               c->codigo, c->nombre, c->creditos, c->semestre, c->cantidadGrupos, 
               c->tieneChoque ? "SI" : "NO", c->requisitos, c->correquisitos, c->tipo);

        cantidad++;
        actual = actual->siguiente;
    }
    printf("Total de cursos: %d\n", cantidad);
}

void mostrar_historial(struct Nodo *historial, struct Nodo *plan) {
    struct Nodo *actual = historial;
    int aprobados = 0;

    while (actual != NULL) {
        struct Historial *h = (struct Historial *) actual->dato;
        struct Curso *c = buscarCurso(plan, h->codigo);

        printf("%s | %s | %s", h->codigo, c ? c->nombre : "(no esta en el plan)", h->aprobado ? "APROBADO" : "pendiente");
        if (h->opcion[0] != '\0') {
            printf(" | opcion: %s", h->opcion);
        }
        printf("\n");

        if (h->aprobado) {
            aprobados++;
        }

        actual = actual->siguiente;
    }
    printf("Cursos aprobados: %d\n", aprobados);
}

int main(void) {
    struct Nodo *planCE = NULL;
    struct Nodo *planPI = NULL;
    struct Nodo *historialCE = NULL;
    struct Nodo *historialPI = NULL;

    printf("\n=== Plan de Ingenieria en Computadores ===\n");
    cargar_plan_estudios(&planCE, RUTA_PLAN_CE);
    cargar_oferta(planCE, RUTA_OFERTA_CE);
    calcular_choques_catalogo(planCE);
    mostrar_plan(planCE);

    printf("\n=== Historial del estudiante de Computadores ===\n");
    cargar_historial(&historialCE, RUTA_HISTORIAL_CE);
    if (validar_historial(planCE, historialCE) == 0 && validar_prerrequisitos(planCE, historialCE) == 0) {
        actualizar_matriculables(planCE, historialCE);
        mostrar_historial(historialCE, planCE);
    }

    printf("\n=== Plan de Ingenieria en Produccion Industrial ===\n");
    cargar_plan_estudios(&planPI, RUTA_PLAN_PI);
    cargar_oferta(planPI, RUTA_OFERTA_PI);
    calcular_choques_catalogo(planPI);
    mostrar_plan(planPI);

    printf("\n=== Historial del estudiante de Produccion Industrial ===\n");
    cargar_historial(&historialPI, RUTA_HISTORIAL_PI);
    if (validar_historial(planPI, historialPI) == 0  && validar_prerrequisitos(planPI, historialPI) == 0) {
        actualizar_matriculables(planPI, historialPI);
        mostrar_historial(historialPI, planPI);
    }

    // Liberar memoria
    liberarLista(&planCE);
    liberarLista(&planPI);
    liberarLista(&historialCE);
    liberarLista(&historialPI);
    
    return 0;
}