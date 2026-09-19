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

#endif //PARTE_1_CONSTANTES_Y_AUXILIARES_H