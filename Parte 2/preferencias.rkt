#lang racket

(require json
         racket/contract
         "tipos.rkt"
         "constantes.rkt")

(provide (contract-out
          [json->curso-interes (-> hash? Curso-Interes?)]
          [json->preferencias (-> hash? Preferencias?)]
          [cargar-preferencias (-> path-string? Preferencias?)]))

;;; Función: json->curso-interes
;;; Toma una tabla hash de un curso de interés y construye un struct Curso-Interes.
;;; Entrada:
;;;   - c: Tabla hash con las llaves 'codigo y 'numeros_grupos.
;;; Salida:
;;;   - Instancia del struct Curso-Interes.
(define (json->curso-interes c)
  (Curso-Interes (hash-ref c 'codigo)
                 (hash-ref c 'numeros_grupos)))


;;; Función: json->preferencias
;;; Toma la tabla hash raíz del archivo de preferencias y construye un struct Preferencias.
;;; Entrada:
;;;   - p: Tabla hash con 'creditos_min, 'creditos_max y 'cursos_interes.
;;; Salida:
;;;   - Instancia del struct Preferencias.
(define (json->preferencias p)
  (Preferencias (hash-ref p 'creditos_min)
                (hash-ref p 'creditos_max)
                (map json->curso-interes (hash-ref p 'cursos_interes))))


;;; Función: cargar-preferencias
;;; Lee el archivo JSON de preferencias desde la ruta especificada y retorna el struct Preferencias.
;;; Entrada:
;;;   - ruta: Ruta del archivo JSON de preferencias (ej. RUTA-PREFERENCIAS-CE).
;;; Salida:
;;;   - Instancia del struct Preferencias.
(define (cargar-preferencias ruta)
 (json->preferencias (call-with-input-file ruta read-json)))


;;; Prueba
(displayln " Preferencias CE cargadas ")
(define prefs-ce (cargar-preferencias RUTA-PREFERENCIAS-CE))
(println prefs-ce)

(displayln "\n Preferencias PI cargadas ")
(define prefs-pi (cargar-preferencias RUTA-PREFERENCIAS-PI))
(println prefs-pi)

