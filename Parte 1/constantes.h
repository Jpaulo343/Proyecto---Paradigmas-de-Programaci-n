#ifndef PARTE_1_CONSTANTES_H
#define PARTE_1_CONSTANTES_H

// tamaños de los textos de los structs
#define TAM_CODIGO 15
#define TAM_NOMBRE 100
#define TAM_REQUISITOS 30
#define TAM_CORREQUISITOS 30
#define TAM_TIPO 10
#define TAM_LINEA 256
#define TAM_DIA 5
#define TAM_HORA 10

// limites de la oferta de horarios
#define MAX_BLOQUES 5
#define MAX_GRUPOS 40

#define SEPARADOR ';' // separador de columnas en los archivos CSV

// archivos de entrada
#define RUTA_PLAN_CE "data/plan_CE.csv" // archivo con el plan de estudios de Computadores
#define RUTA_PLAN_PI "data/plan_PI.csv"
#define RUTA_HISTORIAL_CE "data/historial_CE.csv" // archivo con el historial del estudiante de Computadores
#define RUTA_HISTORIAL_PI "data/historial_PI.csv"
#define RUTA_OFERTA_CE "data/oferta_CE.csv" // archivo con la oferta de cursos y horarios del plan de Computadores
#define RUTA_OFERTA_PI "data/oferta_PI.csv"

// valores de la columna "aprobado" del historial
#define APROBADO_SI "SI" // Valor que indica que el curso fue aprobado en el historial
#define APROBADO_NO "NO" // Valor que indica que el curso aún no ha sido aprobado en el historial

// archivos de salida
#define RUTA_SALIDA_CE "catalogo_CE.json" // archivo de salida con el catalogo completo en formato JSON
#define RUTA_SALIDA_PI "catalogo_PI.json"
#define CARRERA_CE "CE" // codigo de la carrera que se exporta en el JSON
#define CARRERA_PI "PI" 
#define PERIODO "2026-2" // periodo academico que se exporta en el JSON

// codigos de salida del programa
#define EXITO 0 // todo bien
#define ERROR_ARCHIVO 1 // no se pudo abrir o crear un archivo
#define ERROR_DATOS 2 // los archivos se leyeron pero los datos no son consistentes

#endif // PARTE_1_CONSTANTES_H
