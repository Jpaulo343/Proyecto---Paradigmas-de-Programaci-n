#ifndef CONSTANTES_H
#define CONSTANTES_H

// Tamaño de campos de texto
#define TAM_CODIGO 15
#define TAM_NOMBRE 100
#define TAM_REQUISITOS 30
#define TAM_CORREQUISITOS 30
#define TAM_TIPO 10
#define TAM_LINEA 256
#define TAM_DIA 5
#define TAM_HORA 10
#define TAM_CATEGORIA 20

// Límites de oferta
#define MAX_BLOQUES 5
#define MAX_GRUPOS 70 
#define MAX_OPCIONES 100

// Comodin que se cumple con cualquier actividad cultural o deportiva
#define COMODIN_CULTURAL_DEPORTIVO "SE1400"

#define SEPARADOR ';' // Separador CSV

// Rutas por defecto
#define RUTA_PLAN_CE "data/plan_CE.csv"
#define RUTA_PLAN_PI "data/plan_PI.csv"
#define RUTA_HISTORIAL_CE "data/historial_CE.csv"
#define RUTA_HISTORIAL_PI "data/historial_PI.csv"
#define RUTA_OFERTA_CE "data/oferta_CE.csv"
#define RUTA_OFERTA_PI "data/oferta_PI.csv"
#define RUTA_OPCIONES "data/opciones.csv"

// Historial
#define APROBADO_SI "SI"
#define APROBADO_NO "NO"

// Salida JSON
#define RUTA_SALIDA_CE "catalogo_CE.json"
#define RUTA_SALIDA_PI "catalogo_PI.json"
#define CARRERA_CE "CE"
#define CARRERA_PI "PI" 
#define PERIODO "2026-2"

// Códigos de retorno
#define EXITO 0
#define ERROR_ARCHIVO 1
#define ERROR_DATOS 2

#endif // CONSTANTES_H