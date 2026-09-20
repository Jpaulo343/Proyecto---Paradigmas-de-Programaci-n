//
// Created by jpaul on 9/18/2026.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "constantes_y_auxiliares.h"

void liberarLista(struct Nodo **inicio) {
    struct Nodo *actual = *inicio;

    while (actual != NULL) {
        struct Nodo *temp = actual;
        actual = actual->siguiente;

        free(temp->dato);
        free(temp);
    }

    *inicio = NULL;
}

void insertarInicio(struct Nodo **inicio, void *dato, size_t tamanoDato) {
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

    nuevo->siguiente = *inicio;
    *inicio = nuevo;
}

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
    nuevo->siguiente = NULL;

    if (*inicio == NULL) {
        *inicio = nuevo;
        return;
    }

    struct Nodo *actual = *inicio;
    while (actual->siguiente != NULL) {
        actual = actual->siguiente;
    }
    actual->siguiente = nuevo;
}

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
        *cursor = NULL;
    }
    return campo;
}

int horaAMinutos(const char *horaStr) {
    int h = 0, m = 0;
    if (sscanf(horaStr, "%d:%d", &h, &m) == 2) {
        return h * 60 + m;
    }
    return 0;
}

void cargar_plan_estudios(struct Nodo **inicio, const char *ruta) {
    FILE *plan_estudios = fopen(ruta,"r");
    if (!plan_estudios) {
        printf("No se puede abrir el plan de estudios %s\n", ruta);
        return;
    }
    char buffer[TAM_LINEA];

    fgets(buffer, TAM_LINEA, plan_estudios);

    while (fgets(buffer, TAM_LINEA, plan_estudios)) {
        if (buffer[0] == '\n' || buffer[0] == '\r') {
            continue;
        }

        struct Curso c = {0};

        char *cursor = buffer;
        char *columna = separarCampo(&cursor);

        if (columna) strncpy(c.codigo, columna, TAM_CODIGO - 1);

        columna = separarCampo(&cursor);
        if (columna) strncpy(c.nombre, columna, TAM_NOMBRE - 1);

        columna = separarCampo(&cursor);
        if (columna) c.creditos = atoi(columna);

        columna = separarCampo(&cursor);
        if (columna) c.horas = atoi(columna);

        columna = separarCampo(&cursor);
        if (columna) c.semestre = atoi(columna);

        columna = separarCampo(&cursor);
        if (columna) strncpy(c.requisitos, columna, TAM_REQUISITOS - 1);

        columna = separarCampo(&cursor);
        if (columna) strncpy(c.correquisitos, columna, TAM_CORREQUISITOS - 1);

        columna = separarCampo(&cursor);
        if (columna) {
            strcpy(c.tipo, columna);
            quitarSaltoLinea(c.tipo);
        }

		c.matriculable=0; // Se define como no matriculable por defecto

        insertarFinal(inicio, &c, sizeof(struct Curso)); //guardar en estructura de datos utilizando la referencia del objeto
    }
    fclose(plan_estudios);
}

void cargar_historial(struct Nodo **inicio, const char *ruta) {
    FILE *archivo = fopen(ruta,"r");
    if (!archivo) {
        printf("No se puede abrir el historial %s\n", ruta);
        return;
    }
    char buffer[TAM_LINEA]; 
    int numeroLinea = 0;

    fgets(buffer, TAM_LINEA, archivo);

    while (fgets(buffer, TAM_LINEA, archivo)) {
        numeroLinea++;
        quitarSaltoLinea(buffer);
        if (buffer[0] == '\0') continue;

        struct Historial h = {0};

        char *cursor = buffer;
        char *columna = separarCampo(&cursor);
        if (columna) strncpy(h.codigo, columna, TAM_CODIGO - 1);

        columna = separarCampo(&cursor);
        if (columna && strcmp(columna, APROBADO_SI) == 0) {
            h.aprobado = 1;
        } else if (columna && strcmp(columna, APROBADO_NO) == 0) {
            h.aprobado = 0;
        } else {
            printf("Error en %s linea %d (%s): la columna aprobado debe ser SI o NO\n", ruta, numeroLinea, h.codigo);
            continue;
        }
        
        columna = separarCampo(&cursor);
        if (columna) strncpy(h.opcion, columna, TAM_CODIGO - 1);
        
        insertarFinal(inicio, &h, sizeof(struct Historial));
    }
    fclose(archivo);
}

void cargar_oferta(struct Nodo *plan, const char *ruta) {
    FILE *archivo = fopen(ruta, "r");
    if (!archivo) {
        printf("No se puede abrir la oferta %s\n", ruta);
        return;
    }

    char buffer[TAM_LINEA];
    fgets(buffer, TAM_LINEA, archivo);

    while (fgets(buffer, TAM_LINEA, archivo)) {
        quitarSaltoLinea(buffer);
        if (buffer[0] == '\0') continue;

        char *cursor = buffer;
        char *col_codigo = separarCampo(&cursor);
        char *col_grupo = separarCampo(&cursor);
        char *col_dia = separarCampo(&cursor);
        char *col_inicio = separarCampo(&cursor);
        char *col_fin = separarCampo(&cursor);

        if (!col_codigo || !col_grupo || !col_dia || !col_inicio || !col_fin) {
            continue;
        }

        struct Curso *c = buscarCurso(plan, col_codigo);
        if (c != NULL) {
            int numGrupo = atoi(col_grupo);
            
            int idxGrupo = -1;
            for (int i = 0; i < c->cantidadGrupos; i++) {
                if (c->grupos[i].numeroGrupo == numGrupo) {
                    idxGrupo = i;
                    break;
                }
            }

            if (idxGrupo == -1 && c->cantidadGrupos < MAX_GRUPOS) {
                idxGrupo = c->cantidadGrupos;
                c->grupos[idxGrupo].numeroGrupo = numGrupo;
                c->grupos[idxGrupo].cantidadBloques = 0;
                c->cantidadGrupos++;
            }

            if (idxGrupo != -1) {
                Grupo *g = &c->grupos[idxGrupo];
                if (g->cantidadBloques < MAX_BLOQUES) {
                    BloqueHorario *b = &g->bloques[g->cantidadBloques];
                    strncpy(b->dia, col_dia, TAM_DIA - 1);
                    b->horaInicio = horaAMinutos(col_inicio);
                    b->horaFin = horaAMinutos(col_fin);
                    g->cantidadBloques++;
                }
            }
        }
    }
    fclose(archivo);
}

struct Curso *buscarCurso(struct Nodo *plan, const char *codigo) {
    struct Nodo *actual = plan;
    while (actual != NULL) {
        struct Curso *c = (struct Curso *) actual->dato;
        if (strcmp(c->codigo, codigo) == 0) {
            return c;
        }
        actual = actual->siguiente;
    }
    return NULL;
}

int validar_historial(struct Nodo *plan, struct Nodo *historial) {
    int errores = 0;
    
    struct Nodo *actual = historial;
    while (actual != NULL) {
        struct Historial *h = (struct Historial *) actual->dato;
        if (buscarCurso(plan, h->codigo) == NULL) {
            printf("Error: el curso %s del historial no esta en el plan de estudios\n", h->codigo);
            errores++;
        }
        actual = actual->siguiente;
    }

    actual = plan;
    while (actual != NULL) {
        struct Curso *c = (struct Curso *) actual->dato;
        int encontrado = 0;

        struct Nodo *otro = historial;
        while (otro != NULL && !encontrado) {
            struct Historial *h = (struct Historial *) otro->dato;
            if (strcmp(h->codigo, c->codigo) == 0) {
                encontrado = 1;
            }
            otro = otro->siguiente;
        }

        if (!encontrado) {
            printf("Error: el curso %s del plan no aparece en el historial\n", c->codigo);
            errores++;
        }
        actual = actual->siguiente;
    }
    return errores;
}

int esCursoAprobado(struct Nodo *historial, const char *codigo) {
    struct Nodo *actual = historial;
    while (actual != NULL) {
        struct Historial *h = (struct Historial *) actual->dato;

        if (h != NULL && strcmp(h->codigo, codigo) == 0) {
            return h->aprobado;
        }

        actual = actual->siguiente;
    }
    return 0;
}

int validar_prerrequisitos(struct Nodo *plan, struct Nodo *historial) {
    int errores = 0;
    struct Nodo *actual = plan;

    while (actual != NULL) {
        struct Curso *c = (struct Curso *) actual->dato;

        if (c != NULL && esCursoAprobado(historial, c->codigo) == 1) {
            if (strlen(c->requisitos) > 0) {
                char copiaReq[TAM_REQUISITOS];
                strcpy(copiaReq, c->requisitos);

                char *req = strtok(copiaReq, ",");
                while (req != NULL) {
                    if (esCursoAprobado(historial, req) == 0) {
                        printf("Error : El estudiante aprobo %s pero no ha aprobado su requisito %s.\n", c->codigo, req);
                        errores++;
                    }
                    req = strtok(NULL, ",");
                }
            }
        }
        actual = actual->siguiente;
    }
    return errores;
}

/*
  Recorre los cursos del plan de estudios. Para cada curso no aprobado,
  verifica si todos sus requisitos obligatorios estan aprobados en el historial.
  Asigna c->matriculable = 1 si se puede matricular o lo deja en su estado orignal (0).
*/
void actualizar_matriculables(struct Nodo *plan, struct Nodo *historial) {
    struct Nodo *actual = plan;

    while (actual != NULL) {
        struct Curso *c = (struct Curso *) actual->dato;

        //Si el curso no ha sido aprobado se verifica que cumple los requisitos
        if (esCursoAprobado(historial, c->codigo) == 0) {
            int requisitosCumplidos = 1;

            if (strlen(c->requisitos) > 0) {
                char copiaReq[TAM_REQUISITOS];
                strcpy(copiaReq, c->requisitos);

                char *req = strtok(copiaReq, ",");
                while (req != NULL) {
                    // Si  un requisito np esta aprobado
                    if (esCursoAprobado(historial, req) == 0) {
                        requisitosCumplidos = 0; // Le falta un requisito
                        break;
                    }
                    req = strtok(NULL, ",");
                }
            }

            c->matriculable = requisitosCumplidos; // 1 si cumple todos, 0 si no
        }
	}
}

        actual = actual->siguiente;
static int hayChoqueBloques(BloqueHorario b1, BloqueHorario b2) {
    if (strcmp(b1.dia, b2.dia) != 0) {
        return 0;
    }
    return (b1.horaInicio < b2.horaFin && b2.horaInicio < b1.horaFin);
}

static int hayChoqueGrupos(Grupo g1, Grupo g2) {
    for (int i = 0; i < g1.cantidadBloques; i++) {
        for (int j = 0; j < g2.cantidadBloques; j++) {
            if (hayChoqueBloques(g1.bloques[i], g2.bloques[j])) {
                return 1;
            }
        }
    }
    return 0;
}

void calcular_choques_catalogo(struct Nodo *plan) {
    struct Nodo *n1 = plan;
    while (n1 != NULL) {
        struct Curso *c1 = (struct Curso *) n1->dato;

        struct Nodo *n2 = plan;
        while (n2 != NULL) {
            struct Curso *c2 = (struct Curso *) n2->dato;

            if (strcmp(c1->codigo, c2->codigo) != 0) {
                for (int i = 0; i < c1->cantidadGrupos; i++) {
                    for (int j = 0; j < c2->cantidadGrupos; j++) {
                        if (hayChoqueGrupos(c1->grupos[i], c2->grupos[j])) {
                            c1->tieneChoque = 1;
                            c2->tieneChoque = 1;
                        }
                    }
                }
            }
            n2 = n2->siguiente;
        }
        n1 = n1->siguiente;
    }
}