#lang typed/racket

(provide RUTA-CATALOGO-CE
         RUTA-CATALOGO-PI
         RUTA-PREFERENCIAS
         RUTA-SALIDA)

;;; Rutas de los archivos de entrada y salida
(: RUTA-CATALOGO-CE String)
(define RUTA-CATALOGO-CE "../Parte 1/catalogo_CE.json")

(: RUTA-CATALOGO-PI String)
(define RUTA-CATALOGO-PI "../Parte 1/catalogo_PI.json")

(: RUTA-PREFERENCIAS String)
(define RUTA-PREFERENCIAS "data/preferencias_estudiante.json")

(: RUTA-SALIDA String)
(define RUTA-SALIDA "data/combinaciones_salida.json")