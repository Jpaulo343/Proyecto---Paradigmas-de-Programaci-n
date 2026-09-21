#ifndef PARTE_1_CONSTANTES_Y_AUXILIARES_H
#define PARTE_1_CONSTANTES_Y_AUXILIARES_H

#include <stddef.h>
#include "constantes.h"

/**
 * @struct BloqueHorario
 * @brief Representa un bloque de clase con día y rango horario.
 */
typedef struct BloqueHorario {
    char dia[TAM_DIA];
    int horaInicio; // Minutos transcurridos desde 00:00 (ej: 07:30 = 450)
    int horaFin;    // Minutos transcurridos desde 00:00 (ej: 09:20 = 560)
} BloqueHorario;

/**
 * @struct Grupo
 * @brief Representa un grupo ofertado con sus bloques de horario y datos opcionales de la actividad.
 */
typedef struct Grupo {
    int numeroGrupo;
    char codigoOpcion[TAM_CODIGO];  // Código específico de la oferta (ej: SE1101)
    char nombreOpcion[TAM_NOMBRE];  // Nombre específico (ej: Teatro)
    BloqueHorario bloques[MAX_BLOQUES];
    int cantidadBloques;
} Grupo;

/**
 * @struct Curso
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
    int tieneChoque; // 1 si choca con algún curso/grupo del catálogo
} Curso;

/**
 * @struct Historial
 * @brief Representa una fila del historial del estudiante.
 */
typedef struct Historial {
    char codigo[TAM_CODIGO];
    int aprobado; // 1 = SI, 0 = NO
    char opcion[TAM_CODIGO]; // código de la actividad elegida
} Historial;

/**
 * @struct Opcion
 * @brief Mapea las actividades específicas (ej: SE1101) con el comodín del plan (ej: SE1100).
 */
typedef struct Opcion {
    char codigoPlan[TAM_CODIGO];   // ej: SE1100
    char codigoOpcion[TAM_CODIGO]; // ej: SE1101
    char nombreOpcion[TAM_NOMBRE]; // ej: Artes dramáticas
    char categoria[TAM_CATEGORIA]; // ej: CULTURAL
} Opcion;

/**
 * @struct Nodo
 * @brief Estructura de nodo genérico para una lista simplemente enlazada.
 */
typedef struct Nodo {
    void *dato;
    struct Nodo *siguiente;
} Nodo;

// ---------- Funciones de listas y auxiliares ----------

/**
 * @brief Libera la memoria dinámica de todos los nodos y sus datos en la lista.
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
 * Se usa al cargar los archivos para que la lista quede en el mismo orden del archivo.
 * @param inicio Puntero al puntero de inicio de la lista enlazada.
 * @param dato Puntero al dato que se desea almacenar.
 * @param tamanoDato Tamaño en bytes del tipo de dato a copiar (sizeof).
 */
void insertarFinal(struct Nodo **inicio, void *dato, size_t tamanoDato);

/**
 * @brief Elimina el salto de línea del final de un texto si existe.
 * @param texto Texto que se desea limpiar.
 */
void quitarSaltoLinea(char *texto);

/**
 * @brief Devuelve el siguiente campo de la línea (hasta el ';') y mueve el cursor al siguiente campo.
 * A diferencia de strtok, no se salta los campos vacíos (";;").
 * @param cursor Puntero a la posición actual en la línea. Queda en NULL al llegar al final.
 * @return El texto del campo, o NULL si ya no hay más campos.
 */
char *separarCampo(char **cursor);

/**
 * @brief Convierte una hora del archivo ("07:30") a minutos desde las 00:00 (450).
 * @param horaStr Hora en formato HH:MM.
 * @return La cantidad de minutos, o 0 si el texto no tiene ese formato.
 */
int horaAMinutos(const char *horaStr);

// ---------- Carga de datos ----------

/**
 * @brief Lee el archivo CSV de opciones y carga el arreglo global de mapeos.
 * @param ruta Ruta del archivo CSV de opciones (RUTA_OPCIONES).
 */
void cargar_opciones(const char *ruta);

/**
 * @brief Lee el archivo CSV del plan de estudios y carga los cursos en la lista enlazada.
 * @param inicio Puntero al puntero de inicio de la lista donde se guardarán los cursos.
 * @param ruta Ruta del archivo CSV del plan (RUTA_PLAN_CE o RUTA_PLAN_PI).
 */
void cargar_plan_estudios(struct Nodo **inicio, const char *ruta);

/**
 * @brief Lee el archivo CSV del historial del estudiante y lo carga en la lista enlazada.
 * @param inicio Puntero al puntero de inicio de la lista donde se guardará el historial.
 * @param ruta Ruta del archivo CSV del historial (RUTA_HISTORIAL_CE o RUTA_HISTORIAL_PI).
 */
void cargar_historial(struct Nodo **inicio, const char *ruta);

/**
 * @brief Lee el archivo CSV de la oferta y le agrega a cada curso del plan sus grupos y horarios.
 * Cada línea del archivo es una sesión, por lo que un grupo con clases el martes
 * y el jueves ocupa dos líneas con el mismo código y número de grupo.
 * @param plan Lista con los cursos del plan, ya cargada.
 * @param ruta Ruta del archivo CSV de la oferta (RUTA_OFERTA_CE o RUTA_OFERTA_PI).
 */
void cargar_oferta(struct Nodo *plan, const char *ruta);

// ---------- Búsqueda y validaciones ----------

/**
 * @brief Busca un curso en la lista del plan de estudios por su código.
 * @param plan Lista con los cursos del plan.
 * @param codigo Código del curso que se busca (ej. CE1101).
 * @return Puntero al curso encontrado, o NULL si no existe.
 */
struct Curso *buscarCurso(struct Nodo *plan, const char *codigo);

/**
 * @brief Busca un curso en la lista del plan por su código directo o por su mapeo de opción.
 * @param plan Lista con los cursos del plan.
 * @param codigoOferta Código proveniente de la oferta (ej. SE1101).
 * @return Puntero al curso comodín (ej. SE1100, SE1400) o NULL si no existe.
 */
struct Curso *buscarCursoConOpciones(struct Nodo *plan, const char *codigoOferta);

/**
 * @brief Busca la información detallada de una opción dada su código de oferta.
 * @param codigoOferta Código específico de la oferta (ej. SE1101).
 * @return Puntero a la opción dentro del arreglo global, o NULL si no existe.
 */
Opcion *buscarOpcion(const char *codigoOferta);

/**
 * @brief Revisa que el historial tenga exactamente los mismos cursos que el plan.
 * @param plan Lista con los cursos del plan.
 * @param historial Lista con el historial del estudiante.
 * @return Cantidad de errores encontrados (0 si todo está bien).
 */
int validar_historial(struct Nodo *plan, struct Nodo *historial);

/**
 * @brief Busca en la lista del historial el código de un curso dado.
 * @param historial Lista con el historial del estudiante.
 * @param codigo Código del curso que se busca.
 * @return 1 si el curso fue aprobado (SI), 0 en caso contrario.
 */
int esCursoAprobado(struct Nodo *historial, const char *codigo);

/**
 * @brief Revisa si el estudiante tiene aprobados todos los requisitos de un curso.
 * @param historial Lista con el historial del estudiante.
 * @param c Curso que se desea revisar.
 * @return 1 si cumple todos los requisitos (o si el curso no tiene), 0 si le falta alguno.
 */
int cumple_requisitos(struct Nodo *historial, struct Curso *c);

/**
 * @brief Revisa que cada curso aprobado tenga también aprobados sus requisitos.
 * @param plan Lista con los cursos del plan.
 * @param historial Lista con el historial del estudiante.
 * @return Cantidad de inconsistencias encontradas (0 si todo está bien).
 */
int validar_prerrequisitos(struct Nodo *plan, struct Nodo *historial);

/**
 * @brief Compara los grupos de todos los cursos del catálogo y marca con
 * tieneChoque = 1 los cursos que chocan de horario con al menos otro curso.
 * @param plan Lista con los cursos del plan, con la oferta ya cargada.
 */
void calcular_choques_catalogo(struct Nodo *plan);

/**
 * @brief Evalúa si un correquisito específico está aprobado o si el estudiante cumple
 *        con los requisitos para matricularlo simultáneamente con un curso.
 * @param plan Lista enlazada con los cursos del plan de estudios.
 * @param historial Lista enlazada con el historial del estudiante.
 * @param codigo_Correq Código del correquisito que se desea evaluar.
 * @return 1 si el correquisito está aprobado o se puede matricular simultáneamente, 0 en caso contrario.
 */
int es_Correquisito_Posible(struct Nodo *plan, struct Nodo *historial, const char *codigo_Correq);

/**
 * @brief Evalúa cada curso del plan de estudios y actualiza su campo 'matriculable' a 1
 *        si el estudiante no lo ha aprobado aún, si cumple con todos sus prerrequisitos
 *        y si es posible cumplir los correquisitos.
 * @param plan Lista enlazada con los cursos del plan de estudios.
 * @param historial Lista enlazada con el historial del estudiante.
 */
void actualizar_matriculables(struct Nodo *plan, struct Nodo *historial);

/**
 * @brief Quita de SE1400 los grupos de las actividades que el estudiante ya llevo,
 * porque una actividad cultural o deportiva no se puede repetir.
 * @param plan Lista con los cursos del plan, con la oferta ya cargada.
 * @param historial Lista con el historial del estudiante (columna opcion).
 */
void quitar_actividades_llevadas(struct Nodo *plan, struct Nodo *historial);

// ---------- Exportación ----------

/**
 * @brief Convierte minutos desde las 00:00 (450) al texto de la hora ("07:30").
 * @param minutos Cantidad de minutos desde las 00:00.
 * @param destino Arreglo de al menos TAM_HORA caracteres donde se escribe la hora.
 */
void minutosAHora(int minutos, char *destino);

/**
 * @brief Escribe el catálogo completo en un archivo JSON.
 * Incluye por cada curso sus datos, requisitos, correquisitos, grupos con sus
 * horarios, si choca con otro curso y si el estudiante lo puede matricular.
 * @param plan Lista con los cursos del plan, con la oferta y los choques ya calculados.
 * @param historial Lista con el historial del estudiante.
 * @param ruta Ruta del archivo de salida (RUTA_SALIDA_CE o RUTA_SALIDA_PI).
 * @param carrera Código de la carrera que se exporta (CARRERA_CE o CARRERA_PI).
 * @return EXITO si se escribió el archivo, ERROR_ARCHIVO si no se pudo crear.
 */
int exportar_json(struct Nodo *plan, struct Nodo *historial, const char *ruta, const char *carrera);

#endif // PARTE_1_CONSTANTES_Y_AUXILIARES_H