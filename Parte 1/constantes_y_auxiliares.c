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

    nuevo->dato = malloc(tamanoDato); //se reserva por aparte el espacio del dato
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
  Igual que insertarInicio pero recorre la lista hasta el ultimo nodo y
  enlaza el nuevo al final, asi los datos quedan en el orden del archivo.
*/
void insertarFinal(struct Nodo **inicio, void *dato, size_t tamanoDato) {
    struct Nodo *nuevo = malloc(sizeof(Nodo));

    if (nuevo == NULL) {
        printf("Error: no se pudo reservar memoria.\n");
        return;
    }

    nuevo->dato = malloc(tamanoDato); //se reserva por aparte el espacio del dato
    if (!nuevo->dato) {
        free(nuevo);
        printf("Error: no se pudo reservar memoria para el dato.\n");
        return;
    }

    memcpy(nuevo->dato, dato, tamanoDato); //copia los datos del puntero en el nodo
    nuevo->siguiente = NULL; // al ser el ultimo, no apunta a ningun nodo

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
  Recorre el texto y cambia por fin de cadena el primer salto de linea que
  encuentre. Los archivos exportados desde Excel terminan cada linea con un
  salto de linea, lo cual puede causar problemas al comparar.
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
  Funciona parecido a strtok, pero no se salta los campos vacios, es decir,
  busca el siguiente ';', lo cambia por un fin de cadena y deja el cursor
  al inicio del siguiente campo.
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
        *cursor = NULL; // era el ultimo campo de la linea
    }
    return campo;
}

/*
  Convierte una hora del archivo ("07:30") a minutos desde las 00:00 (450),
  asi comparar dos horarios es solo comparar numeros enteros.
*/
int horaAMinutos(const char *horaStr) {
    int h = 0, m = 0;
    if (sscanf(horaStr, "%d:%d", &h, &m) == 2) {
        return h * 60 + m;
    }
    return 0;
}

/*
  Abre y procesa el archivo CSV con los datos del plan de estudios,
  separa cada columna mediante separarCampo con delimitador ';', llena los campos
  del struct Curso e inserta cada curso en la lista enlazada dada.
*/
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

        struct Curso c = {0}; // se inicializa con 0 para evitar basura en memoria

        char *cursor = buffer;
        char *columna = separarCampo(&cursor);

        if (columna) strncpy(c.codigo, columna, TAM_CODIGO - 1);

        columna = separarCampo(&cursor);
        if (columna) strncpy(c.nombre, columna, TAM_NOMBRE - 1);

        columna = separarCampo(&cursor);
        if (columna) c.creditos = atoi(columna); //se convierte el texto a int

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
            strncpy(c.tipo, columna, TAM_TIPO - 1);
            quitarSaltoLinea(c.tipo);
        }

		c.matriculable=0; // Se define como no matriculable por defecto

        insertarFinal(inicio, &c, sizeof(struct Curso)); //guardar en estructura de datos utilizando la referencia del objeto
    }
    fclose(plan_estudios);
}

/*
  Abre y procesa el archivo CSV con el historial del estudiante,
  cada linea tiene el codigo;aprobado;opcion. La columna aprobado se convierte a 1 (SI) o 0 (NO)
*/
void cargar_historial(struct Nodo **inicio, const char *ruta) {
    FILE *archivo = fopen(ruta,"r");
    if (!archivo) {
        printf("No se puede abrir el historial %s\n", ruta);
        return;
    }
    char buffer[TAM_LINEA];
    int numeroLinea = 1; // la linea 1 es el encabezado

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

/*
  Abre y procesa el archivo CSV con la oferta de horarios. Cada linea es una
  sesion (codigo;grupo;dia;inicio;fin), asi que se busca el curso en el plan,
  se busca o se crea su grupo y se le agrega el bloque de horario.
*/
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

            if (idxGrupo == -1) {
                if (c->cantidadGrupos < MAX_GRUPOS) {
                    idxGrupo = c->cantidadGrupos;
                    c->grupos[idxGrupo].numeroGrupo = numGrupo;
                    c->grupos[idxGrupo].cantidadBloques = 0;
                    c->cantidadGrupos++;
                } else {
                    // si no se avisa, el curso queda con menos grupos de los que ofrece el TEC
                    printf("Error en %s: el curso %s tiene mas de %d grupos, el grupo %d no se cargo\n",
                           ruta, c->codigo, MAX_GRUPOS, numGrupo);
                }
            }

            if (idxGrupo != -1) {
                Grupo *g = &c->grupos[idxGrupo];
                if (g->cantidadBloques < MAX_BLOQUES) {
                    BloqueHorario *b = &g->bloques[g->cantidadBloques];
                    strncpy(b->dia, col_dia, TAM_DIA - 1);
                    b->horaInicio = horaAMinutos(col_inicio);
                    b->horaFin = horaAMinutos(col_fin);
                    g->cantidadBloques++;
                } else {
                    printf("Error en %s: el grupo %d de %s tiene mas de %d bloques\n",
                           ruta, numGrupo, c->codigo, MAX_BLOQUES);
                }
            }
        }
    }
    fclose(archivo);
}

/*
  Recorre la lista de cursos y compara cada codigo con strcmp
  Retorna el puntero al curso encontrado o NULL si no esta
*/
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

/*
  Revisa que el historial sea consistente, es decir, cada codigo del historial debe existir en el plan
  y cada curso del plan debe aparecer en el historial
  Retorna la cantidad de errores encontrados
*/
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

/*
  Busca en la lista del historial el codigo de un curso dado.
  Retorna 1 si el curso fue aprobado (SI) o 0 en caso contrario.
*/
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

/*
  Recorre todos los cursos del plan de estudios. Para cada curso que el estudiante
  haya aprobado en su historial, separa sus requisitos obligatorios por comas
  y verifica que cada uno de ellos tambien tenga estado aprobado == 1.
  Retorna la cantidad total de inconsistencias de prerrequisitos encontradas.
*/
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

        actual = actual->siguiente;
    }
}

/*
  Dos bloques chocan si son el mismo dia y sus horas se traslapan.
  Si uno termina justo cuando empieza el otro NO se considera choque.
*/
static int hayChoqueBloques(BloqueHorario b1, BloqueHorario b2) {
    if (strcmp(b1.dia, b2.dia) != 0) {
        return 0;
    }
    return (b1.horaInicio < b2.horaFin && b2.horaInicio < b1.horaFin);
}

/*
  Compara todos los bloques de un grupo contra los del otro.
  Basta con que un par de bloques choque para que los grupos choquen.
*/
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

/*
  Compara cada curso del catalogo contra todos los demas, grupo por grupo,
  y marca con tieneChoque = 1 a los que chocan con al menos otro curso.
  No se comparan los grupos de un mismo curso entre si, porque el
  estudiante solo lleva uno de ellos.
*/
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