#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "constantes_y_auxiliares.h"

static Opcion listaOpciones[MAX_OPCIONES];
static int cantidadOpciones = 0;
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
        free(temp);  // se libera la memoria del nodo
    }
    *inicio = NULL;
}
/*
  Reserva memoria en el Heap para un nuevo nodo y para su dato generico,
  realiza una copia exacta de los bytes del puntero dado usando memcpy
  y lo enlaza al inicio de la lista.
*/
void insertarInicio(struct Nodo **inicio, void *dato, size_t tamanoDato) {
    struct Nodo *nuevo = malloc(sizeof(struct Nodo));
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
/*
  Igual que insertarInicio pero recorre la lista hasta el ultimo nodo y
  enlaza el nuevo al final, asi los datos quedan en el orden del archivo.
*/
void insertarFinal(struct Nodo **inicio, void *dato, size_t tamanoDato) {
    struct Nodo *nuevo = malloc(sizeof(struct Nodo));
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
    if (campo == NULL) return NULL;

    char *separador = strchr(campo, SEPARADOR);
    if (separador != NULL) {
        *separador = '\0';
        *cursor = separador + 1;
    } else {
        *cursor = NULL;
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

void minutosAHora(int minutos, char *destino) {
    sprintf(destino, "%02d:%02d", minutos / 60, minutos % 60);
}

void cargar_opciones(const char *ruta) {
    FILE *archivo = fopen(ruta, "r");
    if (!archivo) {
        printf("No se pudo abrir el archivo de opciones: %s\n", ruta);
        return;
    }
    char buffer[TAM_LINEA];
    fgets(buffer, TAM_LINEA, archivo); // Omitir encabezado
    cantidadOpciones = 0;
    while (fgets(buffer, TAM_LINEA, archivo) && cantidadOpciones < MAX_OPCIONES) {
        quitarSaltoLinea(buffer);
        if (buffer[0] == '\0') continue;
        
        char *cursor = buffer;
        char *col_plan = separarCampo(&cursor);
        char *col_opcion = separarCampo(&cursor);
        char *col_nombre = separarCampo(&cursor);
        char *col_categoria = separarCampo(&cursor);
        
        if (col_plan && col_opcion) {
            strncpy(listaOpciones[cantidadOpciones].codigoPlan, col_plan, TAM_CODIGO - 1);
            strncpy(listaOpciones[cantidadOpciones].codigoOpcion, col_opcion, TAM_CODIGO - 1);
            if (col_nombre) {
                strncpy(listaOpciones[cantidadOpciones].nombreOpcion, col_nombre, TAM_NOMBRE - 1);
            }
            if (col_categoria) {
                strncpy(listaOpciones[cantidadOpciones].categoria, col_categoria, TAM_CATEGORIA - 1);
            }
            cantidadOpciones++;
        }
    }
    fclose(archivo);
}

Opcion *buscarOpcion(const char *codigoOferta) {
    for (int i = 0; i < cantidadOpciones; i++) {
        if (strcmp(listaOpciones[i].codigoOpcion, codigoOferta) == 0) {
            return &listaOpciones[i];
        }
    }
    return NULL;
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

struct Curso *buscarCursoConOpciones(struct Nodo *plan, const char *codigoOferta) {
    struct Curso *c = buscarCurso(plan, codigoOferta);
    if (c != NULL) return c;

    Opcion *opc = buscarOpcion(codigoOferta);
    if (opc != NULL) {
        c = buscarCurso(plan, opc->codigoPlan);
        if (c != NULL) return c;
        c = buscarCurso(plan, COMODIN_CULTURAL_DEPORTIVO);
        if (c != NULL) return c;
    }
    return NULL;
}

void cargar_plan_estudios(struct Nodo **inicio, const char *ruta) {
    FILE *plan_estudios = fopen(ruta, "r");
    if (!plan_estudios) {
        printf("No se puede abrir el plan de estudios %s\n", ruta);
        return;
    }
    char buffer[TAM_LINEA];
    fgets(buffer, TAM_LINEA, plan_estudios);

    while (fgets(buffer, TAM_LINEA, plan_estudios)) {
        if (buffer[0] == '\n' || buffer[0] == '\r') continue;

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
            strncpy(c.tipo, columna, TAM_TIPO - 1);
            quitarSaltoLinea(c.tipo);
        }

        c.matriculable = 0;
        insertarFinal(inicio, &c, sizeof(struct Curso));
    }
    fclose(plan_estudios);
}
/*
  Abre y procesa el archivo CSV con el historial del estudiante,
  cada linea tiene el codigo;aprobado;opcion. La columna aprobado se convierte a 1 (SI) o 0 (NO)
*/
void cargar_historial(struct Nodo **inicio, const char *ruta) {
    FILE *archivo = fopen(ruta, "r");
    if (!archivo) {
        printf("No se puede abrir el historial %s\n", ruta);
        return;
    }
    char buffer[TAM_LINEA];
    int numeroLinea = 1;

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
  sesion (codigo;grupo;dia;inicio;fin), asi que se busca el curso en el plan
  y se le agrega la sesion a su grupo. Las actividades culturales y deportivas
  se agregan a su comodin (SE1100 o SE1200) y tambien a SE1400, porque
  SE1400 se puede cumplir con cualquiera de ellas.
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

        // destinos[0] es el curso del plan (o el comodin SE1100/SE1200 si es una actividad).
        // destinos[1] es SE1400, solo si la linea es una actividad cultural o deportiva,
        // porque SE1400 se puede cumplir con cualquiera de ellas.
        struct Curso *destinos[2];
        destinos[0] = buscarCursoConOpciones(plan, col_codigo);
        destinos[1] = NULL;
        if (buscarOpcion(col_codigo) != NULL) {
            destinos[1] = buscarCurso(plan, COMODIN_CULTURAL_DEPORTIVO);
            if (destinos[1] == destinos[0]) {
                destinos[1] = NULL; // no se agrega dos veces al mismo curso
            }
        }

        for (int d = 0; d < 2; d++) {
            struct Curso *c = destinos[d];
            if (c == NULL) {
                continue;
            }

            int numGrupo = atoi(col_grupo);
            int idxGrupo = -1;

            for (int i = 0; i < c->cantidadGrupos; i++) {
                if (c->grupos[i].numeroGrupo == numGrupo && strcmp(c->grupos[i].codigoOpcion, col_codigo) == 0) {
                    idxGrupo = i;
                    break;
                }
            }

            if (idxGrupo == -1) {
                if (c->cantidadGrupos < MAX_GRUPOS) {
                    idxGrupo = c->cantidadGrupos;
                    c->grupos[idxGrupo].numeroGrupo = numGrupo;
                    strncpy(c->grupos[idxGrupo].codigoOpcion, col_codigo, TAM_CODIGO - 1);

                    Opcion *opc = buscarOpcion(col_codigo);
                    if (opc) {
                        strncpy(c->grupos[idxGrupo].nombreOpcion, opc->nombreOpcion, TAM_NOMBRE - 1);
                    } else {
                        strncpy(c->grupos[idxGrupo].nombreOpcion, c->nombre, TAM_NOMBRE - 1);
                    }

                    c->grupos[idxGrupo].cantidadBloques = 0;
                    c->cantidadGrupos++;
                } else {
                    printf("Error en %s: el curso %s tiene mas de %d grupos\n", ruta, c->codigo, MAX_GRUPOS);
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
                    printf("Error en %s: el grupo %d de %s tiene mas de %d bloques\n", ruta, numGrupo, c->codigo, MAX_BLOQUES);
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
int validar_historial(struct Nodo *plan, struct Nodo *historial) {
    int errores = 0;
    struct Nodo *actual = historial;
    while (actual != NULL) {
        struct Historial *h = (struct Historial *) actual->dato;
        if (buscarCursoConOpciones(plan, h->codigo) == NULL) {
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
            } else {
                Opcion *opc = buscarOpcion(h->codigo);
                if (opc && (strcmp(opc->codigoPlan, c->codigo) == 0 || strcmp(c->codigo, COMODIN_CULTURAL_DEPORTIVO) == 0)) {
                    encontrado = 1;
                }
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
        if (h != NULL) {
            if (strcmp(h->codigo, codigo) == 0) return h->aprobado;
            Opcion *opc = buscarOpcion(h->codigo);
            if (opc && (strcmp(opc->codigoPlan, codigo) == 0 || strcmp(codigo, COMODIN_CULTURAL_DEPORTIVO) == 0)) {
                return h->aprobado;
            }
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
            if (strlen(c->requisitos) > 0 && strcmp(c->requisitos, "NaN") != 0) {
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
  Revisa si todos los requisitos de un curso estan aprobados en el historial.
  Retorna 1 si los cumple todos (o si el curso no tiene requisitos) y 0 si le falta alguno.
*/
int cumple_requisitos(struct Nodo *historial, struct Curso *c) {
    if (strlen(c->requisitos) == 0 || strcmp(c->requisitos, "NaN") == 0) {
        return 1;
    }
    char copiaReq[TAM_REQUISITOS];
    strcpy(copiaReq, c->requisitos);
    char *req = strtok(copiaReq, ",");
    while (req != NULL) {
        if (esCursoAprobado(historial, req) == 0) {
            return 0;
        }
        req = strtok(NULL, ",");
    }
    return 1;
}
/*
  Revisa si todos los requisitos de un curso estan aprobados en el historial.
  Retorna 1 si los cumple todos (o si el curso no tiene requisitos) y 0 si le falta alguno.
*/
int es_Correquisito_Posible(struct Nodo *plan, struct Nodo *historial, const char *codigo_Correq) {
    if (esCursoAprobado(historial, codigo_Correq) == 1) {
        return 1;
    }
    struct Curso *cCorreq = buscarCurso(plan, codigo_Correq);
    if (cCorreq == NULL) {
        printf("Error: no se pudo cargar el curso en 'es_Correquisito_Posible'\n");
        return 0;
    }
    return cumple_requisitos(historial, cCorreq);
}

/*
  Recorre los cursos del plan de estudios y marca cuales puede matricular el estudiante.
  Un curso es matriculable si no esta aprobado, si se ofrece en el periodo,
  si cumple sus requisitos y si cada correquisito esta aprobado o se puede llevar a la vez.
*/
void actualizar_matriculables(struct Nodo *plan, struct Nodo *historial) {
    struct Nodo *actual = plan;
    while (actual != NULL) {
        struct Curso *c = (struct Curso *) actual->dato;
        if (c != NULL) {
            if (esCursoAprobado(historial, c->codigo) == 1) {
                c->matriculable = 0;
            } else if (c->cantidadGrupos == 0) {
                c->matriculable = 0; // no se ofrece en este periodo
            } else {
                int requisitosCumplidos = cumple_requisitos(historial, c);

                if (requisitosCumplidos && strlen(c->correquisitos) > 0 && strcmp(c->correquisitos, "NaN") != 0) {
                    char copiaCorreq[TAM_CORREQUISITOS];
                    strncpy(copiaCorreq, c->correquisitos, TAM_CORREQUISITOS - 1);
                    copiaCorreq[TAM_CORREQUISITOS - 1] = '\0';

                    char *correq = copiaCorreq;
                    while (correq != NULL && *correq != '\0') {
                        char *coma = strchr(correq, ',');
                        if (coma != NULL) {
                            *coma = '\0'; // se corta el codigo actual
                        }

                        if (!es_Correquisito_Posible(plan, historial, correq)) {
                            requisitosCumplidos = 0;
                            break;
                        }

                        correq = (coma != NULL) ? coma + 1 : NULL; // se pasa al siguiente codigo
                    }
                }
                c->matriculable = requisitosCumplidos;
            }
        }
        actual = actual->siguiente;
    }
}

/*
  Quita de SE1400 los grupos de las actividades que el estudiante ya llevo,
  porque no puede repetir una actividad cultural o deportiva. Las actividades
  ya llevadas se leen de la columna "opcion" del historial.
*/
void quitar_actividades_llevadas(struct Nodo *plan, struct Nodo *historial) {
    struct Curso *mixto = buscarCurso(plan, COMODIN_CULTURAL_DEPORTIVO);
    if (mixto == NULL) {
        return;
    }

    struct Nodo *actual = historial;
    while (actual != NULL) {
        struct Historial *h = (struct Historial *) actual->dato;

        if (h->opcion[0] != '\0') {
            int i = 0;
            while (i < mixto->cantidadGrupos) {
                if (strcmp(mixto->grupos[i].codigoOpcion, h->opcion) == 0) {
                    // se corren los grupos siguientes una posicion hacia atras
                    for (int j = i; j < mixto->cantidadGrupos - 1; j++) {
                        mixto->grupos[j] = mixto->grupos[j + 1];
                    }
                    mixto->cantidadGrupos--;
                } else {
                    i++;
                }
            }
        }
        actual = actual->siguiente;
    }
}

static int hayChoqueBloques(BloqueHorario b1, BloqueHorario b2) {
    if (strcmp(b1.dia, b2.dia) != 0) return 0;
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
                        // SE1400 comparte grupos con SE1100 y SE1200: el mismo grupo real no choca consigo mismo
                        if (c1->grupos[i].numeroGrupo == c2->grupos[j].numeroGrupo &&
                            strcmp(c1->grupos[i].codigoOpcion, c2->grupos[j].codigoOpcion) == 0) {
                            continue;
                        }
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
/*
  Convierte los minutos guardados en el bloque (450) al texto de la hora
  ("07:30")
*/
static void escribir_texto_json(FILE *archivo, const char *texto) {
    fprintf(archivo, "\"");
    int i = 0;
    while (texto[i] != '\0') {
        if (texto[i] == '"' || texto[i] == '\\') {
            fprintf(archivo, "\\");
        }
        fprintf(archivo, "%c", texto[i]);
        i++;
    }
    fprintf(archivo, "\"");
}
/*
  Escribe una lista de codigos como arreglo JSON
  Si el texto viene vacio escribe un arreglo vacio.
*/
static void escribir_lista_json(FILE *archivo, const char *texto) {
    if (strcmp(texto, "NaN") == 0) {
        fprintf(archivo, "[]");
        return;
    }
    char copia[TAM_REQUISITOS];
    strncpy(copia, texto, TAM_REQUISITOS - 1);
    copia[TAM_REQUISITOS - 1] = '\0';
    fprintf(archivo, "[");
    char *cursor = copia;
    int primero = 1;
    while (cursor != NULL && *cursor != '\0') {
        char *codigo = cursor;
        char *coma = strchr(cursor, ',');
        if (coma != NULL) {
            *coma = '\0';
            cursor = coma + 1;
        } else {
            cursor = NULL;
        }
        if (!primero) fprintf(archivo, ", ");
        escribir_texto_json(archivo, codigo);
        primero = 0;
    }
    fprintf(archivo, "]");
}
/*
  Escribe los grupos de un curso con sus bloques de horario. Cada bloque
  lleva el dia, la hora en texto y la hora en minutos (para que sea mas facil de comparar)
*/
static void escribir_grupos_json(FILE *archivo, struct Curso *c) {
    fprintf(archivo, "      \"grupos\": [\n");
    for (int i = 0; i < c->cantidadGrupos; i++) {
        Grupo *g = &c->grupos[i];
        fprintf(archivo, "        { \"numero\": %d, ", g->numeroGrupo);
        fprintf(archivo, "\"codigo_opcion\": ");
        escribir_texto_json(archivo, g->codigoOpcion);
        fprintf(archivo, ", \"nombre_opcion\": ");
        escribir_texto_json(archivo, g->nombreOpcion);
        fprintf(archivo, ", \"horarios\": [");
        
        for (int j = 0; j < g->cantidadBloques; j++) {
            BloqueHorario *b = &g->bloques[j];
            char horaInicio[TAM_HORA], horaFin[TAM_HORA];
            minutosAHora(b->horaInicio, horaInicio);
            minutosAHora(b->horaFin, horaFin);
            
            if (j > 0) fprintf(archivo, ", ");
            fprintf(archivo, "{ \"dia\": ");
            escribir_texto_json(archivo, b->dia);
            fprintf(archivo, ", \"inicio\": ");
            escribir_texto_json(archivo, horaInicio);
            fprintf(archivo, ", \"fin\": ");
            escribir_texto_json(archivo, horaFin);
            fprintf(archivo, ", \"inicio_min\": %d, \"fin_min\": %d }", b->horaInicio, b->horaFin);
        }
        fprintf(archivo, "] }");
        if (i < c->cantidadGrupos - 1) fprintf(archivo, ",");
        fprintf(archivo, "\n");
    }
    fprintf(archivo, "      ]\n");
}
/*
  Escribe como arreglo JSON los correquisitos que el estudiante todavia no ha aprobado
*/
static void escribir_correquisitos_pendientes_json(FILE *archivo, struct Nodo *historial, struct Curso *c) {
    if (strcmp(c->correquisitos, "NaN") == 0) {
        fprintf(archivo, "[]");
        return;
    }
    char copia[TAM_CORREQUISITOS];
    strncpy(copia, c->correquisitos, TAM_CORREQUISITOS - 1);
    copia[TAM_CORREQUISITOS - 1] = '\0';
    fprintf(archivo, "[");
    char *cursor = copia;
    int primero = 1;
    while (cursor != NULL && *cursor != '\0') {
        char *codigo = cursor;
        char *coma = strchr(cursor, ',');
        if (coma != NULL) {
            *coma = '\0';
            cursor = coma + 1;
        } else {
            cursor = NULL;
        }
        if (esCursoAprobado(historial, codigo) == 0) {
            if (!primero) fprintf(archivo, ", ");
            escribir_texto_json(archivo, codigo);
            primero = 0;
        }
    }
    fprintf(archivo, "]");
}
/*
  Recorre todo el plan de estudios y escribe el JSON con todos los cursos.
*/
int exportar_json(struct Nodo *plan, struct Nodo *historial, const char *ruta, const char *carrera) {
    FILE *salida = fopen(ruta, "w");
    if (!salida) {
        printf("No se puede crear el archivo de salida %s\n", ruta);
        return ERROR_ARCHIVO;
    }
    fprintf(salida, "{\n");
    fprintf(salida, "  \"carrera\": \"%s\",\n", carrera);
    fprintf(salida, "  \"periodo\": \"%s\",\n", PERIODO);
    fprintf(salida, "  \"cursos\": [\n");
    
    struct Nodo *actual = plan;
    while (actual != NULL) {
        struct Curso *c = (struct Curso *) actual->dato;
        fprintf(salida, "    {\n");
        fprintf(salida, "      \"codigo\": ");
        escribir_texto_json(salida, c->codigo);
        fprintf(salida, ",\n      \"nombre\": ");
        escribir_texto_json(salida, c->nombre);
        fprintf(salida, ",\n      \"creditos\": %d,\n", c->creditos);
        fprintf(salida, "      \"horas\": %d,\n", c->horas);
        fprintf(salida, "      \"semestre\": %d,\n", c->semestre);
        fprintf(salida, "      \"tipo\": ");
        escribir_texto_json(salida, c->tipo);
        fprintf(salida, ",\n      \"requisitos\": ");
        escribir_lista_json(salida, c->requisitos);
        fprintf(salida, ",\n      \"correquisitos\": ");
        escribir_lista_json(salida, c->correquisitos);
        fprintf(salida, ",\n      \"aprobado\": %s,\n", esCursoAprobado(historial, c->codigo) ? "true" : "false");
        fprintf(salida, "      \"cumple_requisitos\": %s,\n", cumple_requisitos(historial, c) ? "true" : "false");
        fprintf(salida, "      \"correquisitos_pendientes\": ");
        escribir_correquisitos_pendientes_json(salida, historial, c);
        fprintf(salida, ",\n      \"matriculable\": %s,\n", c->matriculable ? "true" : "false");
        fprintf(salida, "      \"tiene_choque\": %s,\n", c->tieneChoque ? "true" : "false");
        escribir_grupos_json(salida, c);
        fprintf(salida, "    }");
        if (actual->siguiente != NULL) fprintf(salida, ",");
        fprintf(salida, "\n");
        actual = actual->siguiente;
    }
    fprintf(salida, "  ]\n");
    fprintf(salida, "}\n");
    fclose(salida);
    printf("Catalogo exportado en %s\n", ruta);
    return EXITO;
}