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
#define TAM_DIA 5
#define TAM_HORA 10
#define MAX_BLOQUES 5
#define MAX_GRUPOS 10

#define SEPARADOR ';'

#define RUTA_PLAN_CE "data/plan_CE.csv"
#define RUTA_PLAN_PI "data/plan_PI.csv"
#define RUTA_HISTORIAL_CE "data/historial_CE.csv"
#define RUTA_HISTORIAL_PI "data/historial_PI.csv"
#define RUTA_OFERTA_CE "data/oferta_CE.csv"
#define RUTA_OFERTA_PI "data/oferta_PI.csv"

#define APROBADO_SI "SI"
#define APROBADO_NO "NO"

/**
 * @struct BloqueHorario
 * @brief Representa un bloque de clase con dia y rango horario.
 */
typedef struct BloqueHorario {
    char dia[TAM_DIA];
    int horaInicio; // Minutos transcurridos desde 00:00 (ej: 07:30 = 450)
    int horaFin;    // Minutos transcurridos desde 00:00 (ej: 09:20 = 560)
} BloqueHorario;

/**
 * @struct Grupo
 * @brief Representa un grupo ofertado con sus bloques de horario.
 */
typedef struct Grupo {
    int numeroGrupo;
    BloqueHorario bloques[MAX_BLOQUES];
    int cantidadBloques;
} Grupo;

/**
 * @struct  Curso
 * @brief Representa un curso individual del plan de estudios.
 * Almacena información académica como código, nombre, créditos, semestre,
 * sus dependencias (requisitos y correquisitos) y si es matriculable o no por el estudiante.
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
	int matriculable; // 1 = SI, 0 = NO
    
    // Oferta y choques
    Grupo grupos[MAX_GRUPOS];
    int cantidadGrupos;
    int tieneChoque; // 1 si choca con algun curso/grupo del catalogo
} Curso;

/**
 * @struct Historial
 * @brief Representa una fila del historial del estudiante.
 */
typedef struct Historial {
    char codigo[TAM_CODIGO];
    int aprobado; // 1 = SI, 0 = NO
    char opcion[TAM_CODIGO]; // codigo de la actividad elegida
} Historial;

/**
 * @struct Nodo
 * @brief Estructura de nodo generico para una lista simplemente enlazada.
 */
typedef struct Nodo {
    void *dato;
    struct Nodo *siguiente;
} Nodo;

// Funciones de Listas y Auxiliares
void liberarLista(struct Nodo **inicio);
void insertarInicio(struct Nodo **inicio, void *dato, size_t tamanoDato);
void insertarFinal(struct Nodo **inicio, void *dato, size_t tamanoDato);
void quitarSaltoLinea(char *texto);
char *separarCampo(char **cursor);
int horaAMinutos(const char *horaStr);

// Carga de datos
void cargar_plan_estudios(struct Nodo **inicio, const char *ruta);
void cargar_historial(struct Nodo **inicio, const char *ruta);
void cargar_oferta(struct Nodo *plan, const char *ruta);

// Busqueda y Validaciones
struct Curso *buscarCurso(struct Nodo *plan, const char *codigo);
int validar_historial(struct Nodo *plan, struct Nodo *historial);
int esCursoAprobado(struct Nodo *historial, const char *codigo);
int validar_prerrequisitos(struct Nodo *plan, struct Nodo *historial);
void calcular_choques_catalogo(struct Nodo *plan);


/**
 * @brief Evalúa cada curso del plan de estudios y actualiza su campo 'matriculable' a 1
 *        si el estudiante no lo ha aprobado aún y cumple con todos sus prerrequisitos.
 * @param plan Lista enlazada con los cursos del plan de estudios.
 * @param historial Lista enlazada con el historial del estudiante.
 */
void actualizar_matriculables(struct Nodo *plan, struct Nodo *historial);
#endif //PARTE_1_CONSTANTES_Y_AUXILIARES_H