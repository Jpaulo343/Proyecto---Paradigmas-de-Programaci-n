#lang typed/racket

(provide RUTA-CATALOGO-CE
         RUTA-CATALOGO-PI
         RUTA-PREFERENCIAS-CE
         RUTA-PREFERENCIAS-PI
         RUTA-SALIDA-CE
         RUTA-SALIDA-PI)

;;; Rutas de los catálogos exportados por la Etapa 1 (C)
(: RUTA-CATALOGO-CE String)
(define RUTA-CATALOGO-CE "../Parte 1/catalogo_CE.json")

(: RUTA-CATALOGO-PI String)
(define RUTA-CATALOGO-PI "../Parte 1/catalogo_PI.json")

;;; Rutas de los archivos de preferencias del estudiante por carrera
(: RUTA-PREFERENCIAS-CE String)
(define RUTA-PREFERENCIAS-CE "data/preferencias_CE.json")

(: RUTA-PREFERENCIAS-PI String)
(define RUTA-PREFERENCIAS-PI "data/preferencias_PI.json")

(: RUTA-SALIDA-CE String)
(define RUTA-SALIDA-CE "data/salida_CE.json")

(: RUTA-SALIDA-PI String)
(define RUTA-SALIDA-PI "data/salida_PI.json")