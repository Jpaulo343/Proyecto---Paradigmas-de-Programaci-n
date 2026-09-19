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
    int numeroLinea = 0;

    fgets(buffer, TAM_LINEA, archivo); // se lee y se ignora la primera linea por ser el encabezado

    while (fgets(buffer,TAM_LINEA,archivo)) {
        numeroLinea++;
        quitarSaltoLinea(buffer);
        if (buffer[0] == '\0') {
            continue; // se ignoran las lineas vacias
        }

        struct Historial h = {0}; // evitar basura en memoria

        char *cursor = buffer;
        char *columna = separarCampo(&cursor);
        if (columna) {
            strncpy(h.codigo, columna, TAM_CODIGO - 1);
        }

        columna = separarCampo(&cursor);
        if (columna && strcmp(columna, APROBADO_SI) == 0) {
            h.aprobado = 1;
        } else if (columna && strcmp(columna, APROBADO_NO) == 0) {
            h.aprobado = 0;
        } else {
            printf("Error en %s linea %d (%s): la columna aprobado debe ser SI o NO\n", ruta, numeroLinea, h.codigo);
            continue; // se ignora la linea con error
        }
        
        columna = separarCampo(&cursor);
        if (columna) {
            strncpy(h.opcion, columna, TAM_CODIGO - 1);
        }
        
        insertarFinal(inicio, &h, sizeof(struct Historial));
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
    Revisa que el historial sea consistente, es decir, cada codigo del hisotorial debe existir en el plan
    y cada curso del plan debe aparecer en el historial
    Retorna la cantidad de errores encontrados
*/
int validar_historial(struct Nodo *plan, struct Nodo *historial) {
    int errores = 0;
    
    // cada curso del historial debe existir en el plan
    struct Nodo *actual = historial;
    while (actual != NULL) {
        struct Historial *h = (struct Historial *) actual->dato;
        if (buscarCurso(plan, h->codigo) == NULL) {
            printf("Error: el curso %s del historial no esta en el plan de estudios\n", h->codigo);
            errores++;
        }
        actual = actual->siguiente;
    }

    // cada curso del plan debe aparecer en el historial
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
            return h->aprobado; // Retorna 1 si es SI, 0 si es NO
        }

        actual = actual->siguiente;
    }
    return 0; // Si no esta en el historial o no se encontro
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

        // Si el estudiante ya aprobo este curso en su historial
        if (c != NULL && esCursoAprobado(historial, c->codigo) == 1) {

            // Si el curso tiene requisitos obligatorios
            if (strlen(c->requisitos) > 0) {
                char copiaReq[TAM_REQUISITOS];
                strcpy(copiaReq, c->requisitos);

                char *req = strtok(copiaReq, ",");
                while (req != NULL) {
                    // Verificar si el requisito individual esta aprobado en el historial
                    if (esCursoAprobado(historial, req) == 0) {
                        printf("Error : El estudiante aprobo %s pero no ha aprobado su requisito %s.\n", c->codigo, req);
                        errores++;
                    }
                    req = strtok(NULL, ","); // Avanzar al siguiente requisito
                }
            }
        }
        actual = actual->siguiente;
    }
    return errores;
}