//
// Created by jpaul on 9/17/2026.
//

#ifndef PARTE_1_CONSTANTES_Y_AUXILIARES_H
#define PARTE_1_CONSTANTES_Y_AUXILIARES_H

#include <stddef.h>

// Constantes
#define TAM_CODIGO 15
#define TAM_NOMBRE 100
#define TAM_REQUISITOS 30
#define TAM_CORREQUISITOS 30
#define TAM_TIPO 10
#define TAM_LINEA 256

#define SEPARADOR ';'

#define RUTA_PLAN_CE "data/plan_CE.csv"
#define RUTA_PLAN_PI "data/plan_PI.csv"
#define RUTA_HISTORIAL_CE "data/historial_CE.csv"
#define RUTA_HISTORIAL_PI "data/historial_PI.csv"

#define APROBADO_SI "SI"
#define APROBADO_NO "NO"

/**
 * @struct struct Curso
 * @brief Representa un curso individual del plan de estudios.
 * Almacena información académica como código, nombre, créditos, semestre
 * y sus dependencias (requisitos y correquisitos).
 */
typedef struct Curso {
    char codigo[TAM_CODIGO];
    char nombre[TAM_NOMBRE];
    int creditos;
    int horas;
    int semestre;
    char requisitos[TAM_REQUISITOS];
    char correquisitos[TAM_CORREQUISITOS];
    char tipo[TAM_TIPO];
} Curso;

/**
 * @struct struct Historial
 * @brief Representa una fila del historial del estudiante.
 * Indica si el curso del plan ya fue aprobado y cual de las actividades
 * culturales/deportivas fue elegida como opción.
 */
typedef struct Historial {
    char codigo[TAM_CODIGO];
    int aprobado; // 1 = SI, 0 = NO
    char opcion[TAM_CODIGO]; // codigo de la actividad cultural/deportiva elegida
} Historial;


/**
 * @struct Nodo
 * @brief Estructura de nodo genérico para una lista simplemente enlazada.
 * Utiliza un puntero void* para almacenar cualquier tipo de dato (Curso, código de historial u otro).
 */
typedef struct Nodo  {
    void *dato;
    struct Nodo *siguiente;
} Nodo;


/**
 * @brief Libera la memoria dinamica de todos los nodos y sus datos en la lista.
 * @param inicio Puntero al puntero de inicio de la lista enlazada.
 */
void liberarLista(struct Nodo **inicio);

/**
 * @brief Inserta un nuevo nodo con una copia del dato al inicio de la lista enlazada.
 * @param inicio Puntero al puntero de inicio de la lista enlazada.
 * @param dato Puntero al dato que se desea almacenar.
 * @param tamanoDato Tamaño en bytes del tipo de dato a copiar (sizeof).
 */
void insertarInicio(struct Nodo **inicio, void *dato, size_t tamanoDato);

/**
 * @brief Inserta un nuevo nodo con una copia del dato al final de la lista enlazada.
 * @param inicio Puntero al puntero de inicio de la lista enlazada.
 * @param dato Puntero al dato que se desea almacenar.
 * @param tamanoDato Tamaño en bytes del tipo de dato a copiar (sizeof).
 */
void insertarFinal(struct Nodo **inicio, void *dato, size_t tamanoDato);

/**
 * @brief Elimina el salto de línea ('\n' o '\r) del final de un texto si existe.
 * @param texto Texto que se desea limpiar
 */
void quitarSaltoLinea(char *texto);

/**
 * @brief Devuelve el siguiente campo de la linea (hasta el ';') y mueve el cursor al siguiente campo.
 * A diferencia de strtok, no se salta los campos vacios (";;").
 * @param cursor Puntero a la posicion actual en la linea. Queda en NULL al llegar al final.
 * @return El texto del campo, o NULL si ya no hay mas campos.
 */
char *separarCampo(char **cursor);

/**
 * @brief Lee el archivo CSV del plan de estudios y carga los cursos en la lista enlazada.
 * @param inicio Puntero al puntero de inicio de la lista donde se guardaran los cursos.
 * @param ruta Ruta del archivo CSV del plan.
 */
void cargar_plan_estudios(struct Nodo **inicio, const char *ruta);

/**
 * @brief Lee el archivo CSV del historial del estudiante y lo carga en la lista enlazada.
 * @param inicio Puntero al puntero de inicio de la lista donde se guardara el historial.
 * @param ruta Ruta del archivo CSV del historial
 */
void cargar_historial(struct Nodo **inicio, const char *ruta);

/**
 * @brief Busca un curso en la lista del plan de estudios por su codigo.
 * @param plan Lista con los cursos del plan.
 * @param codigo Codigo del curso que se busca
 * @return Puntero al curso encontrado, o NULL si no existe.
 */
struct Curso *buscarCurso(struct Nodo *plan, const char *codigo);

/**
 * @brief Revisa que el historial tenga exactamente los mismos cursos que el plan.
 * @param plan Lista con los cursos del plan.
 * @param historial Lista con el historial del estudiante.
 * @return Cantidad de errores encontrados (0 si todo esta bien).
 */
int validar_historial(struct Nodo *plan, struct Nodo *historial);

/**
 * @brief Consulta si un código de curso específico tiene estado de aprobado (1 = SI) en el historial.
 * @param historial Lista enlazada con el historial del estudiante.
 * @param codigo Código del curso a consultar (ej: "CE1101").
 * @return 1 si el curso está aprobado, 0 si no está aprobado o no existe en el historial.
 */
int esCursoAprobado(struct Nodo *historial, const char *codigo);

/**
 * @brief Valida que para cada curso aprobado por el estudiante en su historial,
 *        se hayan aprobado previamente todos los prerrequisitos exigidos por el plan de estudios.
 * @param plan Lista enlazada con los cursos del plan de estudios.
 * @param historial Lista enlazada con el historial del estudiante.
 * @return Cantidad de errores de prerrequisitos no cumplidos detectados (0 si todo es correcto).
 */
int validar_prerrequisitos(struct Nodo *plan, struct Nodo *historial);

#endif //PARTE_1_CONSTANTES_Y_AUXILIARES_H