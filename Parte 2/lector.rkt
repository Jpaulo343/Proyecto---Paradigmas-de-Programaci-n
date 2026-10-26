#lang racket

(require json
         racket/contract
         "tipos.rkt"
         "constantes.rkt")

(provide (contract-out
          [json->bloque (-> hash? Bloque-Horario?)]
          [json->grupo (-> hash? Grupo?)]
          [json->curso (-> hash? Curso?)]))

;;; Función: json->bloque
;;; Toma una tabla hash de un bloque horario (proveniente del JSON)
;;; y construye un struct Bloque-Horario.
;;; Entrada:
;;;   - b: Tabla hash con las llaves 'dia, 'inicio, 'fin, 'inicio_min y 'fin_min.
;;; Salida:
;;;   - Instancia del struct Bloque-Horario.
(define (json->bloque b)
  (Bloque-Horario (hash-ref b 'dia)
                  (hash-ref b 'inicio)
                  (hash-ref b 'fin)
                  (hash-ref b 'inicio_min)
                  (hash-ref b 'fin_min)))


;;; Función: json->grupo
;;; Toma una tabla hash de un grupo (proveniente del JSON)
;;; y construye un struct Grupo convirtiendo su lista interna de horarios.
;;; Entrada:
;;;   - g: Tabla hash con las llaves 'numero, 'codigo_opcion, 'nombre_opcion y 'horarios.
;;; Salida:
;;;   - Instancia del struct Grupo (con su lista de Bloque-Horario adentro).
(define (json->grupo g)
  (Grupo (hash-ref g 'numero)
         (hash-ref g 'codigo_opcion)
         (hash-ref g 'nombre_opcion)
         (map json->bloque (hash-ref g 'horarios))))

;;; Función: json->curso
;;; Toma una tabla hash de un curso (proveniente del JSON) y construye un struct Curso.
;;; Entrada:
;;;   - c: Tabla hash con las llaves 'codigo, 'nombre, 'carrera, 'creditos, 'horas,
;;;        'semestre, 'tipo, 'matriculable, 'correquisitos_pendientes y 'grupos.
;;; Salida:
;;;   - Instancia del struct Curso.
(define (json->curso c)
  (Curso (hash-ref c 'codigo)
         (hash-ref c 'nombre)
         (hash-ref c 'carrera)
         (hash-ref c 'creditos)
         (hash-ref c 'horas)
         (hash-ref c 'semestre)
         (hash-ref c 'tipo)
         (hash-ref c 'matriculable)
         (hash-ref c 'correquisitos_pendientes)
         (map json->grupo (hash-ref c 'grupos))))



;;; Prueba
(define datos-prueba (call-with-input-file RUTA-CATALOGO-CE read-json))
(define primer-curso (first (hash-ref datos-prueba 'cursos)))


(displayln "Primer curso convertido (con sus grupos y horarios adentro):")
(println (first (json->curso primer-curso)))