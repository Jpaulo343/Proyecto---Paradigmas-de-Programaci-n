//
// Created by jpaul on 9/17/2026.
//

#ifndef PARTE_1_CONSTANTES_Y_AUXILIARES_H
#define PARTE_1_CONSTANTES_Y_AUXILIARES_H

/**
 * @struct struct Curso
 * @brief Representa un curso individual del plan de estudios.
 * Almacena información académica como código, nombre, créditos, semestre
 * y sus dependencias (requisitos y correquisitos).
 */
typedef struct Curso {
    char codigo[15];
    char nombre[100];
    int creditos;
    int horas;
    int semestre;
    char requisitos[30];
    char correquisitos[10];
    char tipo[10];
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
 * @brief Lee el archivo CSV del plan de estudios y carga los cursos en la lista enlazada.
 * @param inicio Puntero al puntero de inicio de la lista donde se guardaran los cursos.
 */
void cargar_plan_estudios(struct Nodo **inicio);

#endif //PARTE_1_CONSTANTES_Y_AUXILIARES_H