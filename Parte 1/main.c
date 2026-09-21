#include <stdio.h>
#include "constantes_y_auxiliares.h"
#include "constantes.h"
/*
  Muestra en pantalla todos los cursos del plan de estudios, con sus datos y
  la cantidad de grupos y si tiene choques de horario con otros cursos.
*/
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

/*
  Muestra en pantalla el historial de un estudiante, con los datos de cada curso y si fue aprobado o no.
*/
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

/*
  Programa principal que carga los planes de estudios y los historiales de los estudiantes,
  valida los datos y exporta el catalogo en formato JSON.
*/
int main(void) {
    struct Nodo *planCE = NULL;
    struct Nodo *planPI = NULL;
    struct Nodo *historialCE = NULL;
    struct Nodo *historialPI = NULL;

    int errores = 0; // se cuenta lo que no se pudo procesar correctamente, para retornar un codigo de salida distinto de cero

    printf("\n=== Plan de Ingenieria en Computadores ===\n");
    cargar_plan_estudios(&planCE, RUTA_PLAN_CE);
    if (planCE == NULL) {
        printf("Error: no se cargo ningun curso del plan de Computadores\n");
        errores++;
    }
    cargar_oferta(planCE, RUTA_OFERTA_CE);
    calcular_choques_catalogo(planCE);
    mostrar_plan(planCE);

    printf("\n=== Historial del estudiante de Computadores ===\n");
    cargar_historial(&historialCE, RUTA_HISTORIAL_CE);
    if (validar_historial(planCE, historialCE) == 0 && validar_prerrequisitos(planCE, historialCE) == 0) {
        actualizar_matriculables(planCE, historialCE);
        mostrar_historial(historialCE, planCE);
        if (exportar_json(planCE, historialCE, RUTA_SALIDA_CE, CARRERA_CE) != EXITO) {
            errores++;
        }
    } else {
        // si el historial no es consistente no se exporta, porque el resultado seria incorrecto
        printf("Error: el historial de Computadores tiene errores, no se exporta el catalogo\n");
        errores++;
    }

    printf("\n=== Plan de Ingenieria en Produccion Industrial ===\n");
    cargar_plan_estudios(&planPI, RUTA_PLAN_PI);
    if (planPI == NULL) {
        printf("Error: no se cargo ningun curso del plan de Produccion Industrial\n");
        errores++;
    }
    cargar_oferta(planPI, RUTA_OFERTA_PI);
    calcular_choques_catalogo(planPI);
    mostrar_plan(planPI);

    printf("\n=== Historial del estudiante de Produccion Industrial ===\n");
    cargar_historial(&historialPI, RUTA_HISTORIAL_PI);
    if (validar_historial(planPI, historialPI) == 0  && validar_prerrequisitos(planPI, historialPI) == 0) {
        actualizar_matriculables(planPI, historialPI);
        mostrar_historial(historialPI, planPI);
        if (exportar_json(planPI, historialPI, RUTA_SALIDA_PI, CARRERA_PI) != EXITO) {
            errores++;
        }
    } else {
        printf("Error: el historial de Produccion Industrial tiene errores, no se exporta el catalogo\n");
        errores++;
    }

    // Liberar memoria
    liberarLista(&planCE);
    liberarLista(&planPI);
    liberarLista(&historialCE);
    liberarLista(&historialPI);

    // el codigo de salida permite saber desde la terminal si el programa termino bien
    if (errores > 0) {
        printf("\nEl programa termino con %d error(es). Codigo de salida: %d\n", errores, ERROR_DATOS);
        return ERROR_DATOS;
    }

    printf("\nTodo se proceso correctamente. Codigo de salida: %d\n", EXITO);
    return EXITO;
}