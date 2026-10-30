#lang racket

(require racket/contract
         racket/stream
         "tipos.rkt")

(provide (contract-out
          [par->curso-seleccionado (-> (cons/c Curso? Grupo?) Curso-Seleccionado?)]
          [pares->combinacion (-> (listof (cons/c Curso? Grupo?)) Combinacion?)]
          [cumple-criterios? (parametric->/c [A] (-> (listof (-> A boolean?)) A boolean?))]
          [filtrar-stream-por-criterios (-> (listof (-> any/c boolean?)) stream? stream?)]
          [stream-pares->combinaciones (-> stream? stream?)]
          [filtrar-por-creditos (-> stream? integer? integer? stream?)]))

;;; Cómo se usa este módulo y qué llama a qué por dentro:
;;; Desde afuera (por ejemplo en main.rkt) solo se llaman 2 funciones en este orden:
;;;
;;; 1. Se llama a (stream-pares->combinaciones stream-pares):
;;;    - Recibe el stream de combinaciones que sale de generador.rkt.
;;;    - Por dentro, esta función llama automáticamente a pares->combinacion por cada
;;;      horario, y pares->combinacion a su vez llama a par->curso-seleccionado para
;;;      convertir cada par (Curso . Grupo) en un Curso-Seleccionado y armar el struct
;;;      Combinacion con el total de créditos, horas y carreras ya calculados.
;;;
;;; 2. Se llama a (filtrar-por-creditos stream-combinaciones min-cred max-cred):
;;;    - Recibe el stream de Combinacion del paso 1 y los límites de créditos.
;;;    - Por dentro, arma las 2 condiciones (>= min-cred y <= max-cred) y llama a
;;;      filtrar-stream-por-criterios, la cual usa la función polimórfica
;;;      cumple-criterios? para revisar cada combinación y devolver solo las válidas.


;;; Función: par->curso-seleccionado
;;; Convierte un par (Curso . Grupo) en un struct Curso-Seleccionado con el grupo elegido.
;;; E: par (cons de un struct Curso y un struct Grupo).
;;; S: Instancia del struct Curso-Seleccionado.
(define (par->curso-seleccionado par)
  (define c (car par))
  (define g (cdr par))
  (Curso-Seleccionado (Curso-codigo c)
                      (Curso-carrera c)
                      (Curso-nombre c)
                      (Curso-creditos c)
                      (Curso-horas c)
                      (Curso-semestre c)
                      (Curso-tipo c)
                      (Curso-requisitos c)
                      (Curso-correquisitos c)
                      (Curso-aprobado c)
                      (Curso-cumple-requisitos c)
                      (Curso-correquisitos-pendientes c)
                      (Curso-matriculable c)
                      (Curso-tiene-choque c)
                      g))


;;; Función: pares->combinacion
;;; Construye un struct Combinacion a partir de una lista de pares (Curso . Grupo),
;;; calculando el total de créditos, total de horas y la lista de carreras de origen.
;;; E: pares (lista de pares Curso . Grupo que conforman un horario sin choques).
;;; S: Instancia del struct Combinacion.
(define (pares->combinacion pares)
  (define seleccionados (map par->curso-seleccionado pares))
  (define creditos (foldl + 0 (map Curso-Seleccionado-creditos seleccionados)))
  (define horas (foldl + 0 (map Curso-Seleccionado-horas seleccionados)))
  (define carreras (map Curso-Seleccionado-carrera seleccionados))
  (Combinacion seleccionados creditos horas carreras))


;;; Función: cumple-criterios?
;;; Función con polimorfismo paramétrico que verifica si un elemento de cualquier
;;; tipo A satisface todos los predicados de una lista de criterios.
;;; E: criterios (lista de funciones predicado de A -> boolean) y elemento (valor de tipo A).
;;; S: #t si el elemento cumple todos los criterios, #f en caso contrario.
(define (cumple-criterios? criterios elemento)
  (cond
    [(null? criterios) #t]
    [((car criterios) elemento)
     (cumple-criterios? (cdr criterios) elemento)]
    [else #f]))


;;; Función: filtrar-stream-por-criterios
;;; Filtra de manera perezosa un stream conservando únicamente los elementos
;;; que cumplen todos los criterios recibidos.
;;; E: criterios (lista de predicados) y flujo (stream de elementos).
;;; S: Stream filtrado de forma perezosa.
(define (filtrar-stream-por-criterios criterios flujo)
  (stream-filter (lambda (elem) (cumple-criterios? criterios elem))
                 flujo))


;;; Función: stream-pares->combinaciones
;;; Transforma de forma perezosa un stream de listas de pares en un stream de struct Combinacion.
;;; E: stream-pares (stream donde cada elemento es una lista de pares Curso . Grupo).
;;; S: Stream de instancias del struct Combinacion.
(define (stream-pares->combinaciones stream-pares)
  (stream-map pares->combinacion stream-pares))


;;; Función: filtrar-por-creditos
;;; Filtra un stream de struct Combinacion según el rango de créditos [min-cred, max-cred].
;;; E: stream-combinaciones (stream de Combinacion), min-cred (entero) y max-cred (entero).
;;; S: Stream de Combinacion cuyos créditos totales están dentro del rango indicado.
(define (filtrar-por-creditos stream-combinaciones min-cred max-cred)
  (define criterios-creditos
    (list (lambda (comb) (>= (Combinacion-creditos-totales comb) min-cred))
          (lambda (comb) (<= (Combinacion-creditos-totales comb) max-cred))))
  (filtrar-stream-por-criterios criterios-creditos stream-combinaciones))


;;; Pruebas
(module+ main
  ;; 1. Grupos de prueba con sus bloques de horario (sin choques entre sí)
  (define g1-ce2103 (Grupo 1 "CE2103" "AED II"
                           (list (Bloque-Horario "MAR" "07:30" "09:20" 450 560)
                                 (Bloque-Horario "JUE" "07:30" "09:20" 450 560))))
  (define g1-cs2101 (Grupo 1 "CS2101" "Ambiente humano"
                           (list (Bloque-Horario "VIE" "09:30" "12:20" 570 740))))
  (define g4-ma2104 (Grupo 4 "MA2104" "Calculo superior"
                           (list (Bloque-Horario "MAR" "09:30" "11:20" 570 680)
                                 (Bloque-Horario "JUE" "09:30" "11:20" 570 680))))

  ;; 2. Cursos de prueba (CE2103: 4 créditos, CS2101: 2 créditos, MA2104: 4 créditos -> Total: 10 créditos y 22 horas)
  (define c-ce2103 (Curso "CE2103" "CE" "AED II" 4 12 3 "CURSO" '() '() #f #t '() #t #f (list g1-ce2103)))
  (define c-cs2101 (Curso "CS2101" "CE" "Ambiente humano" 2 6 3 "CURSO" '() '() #f #t '() #t #f (list g1-cs2101)))
  (define c-ma2104 (Curso "MA2104" "CE" "Calculo superior" 4 4 3 "CURSO" '() '() #f #t '() #t #f (list g4-ma2104)))

  ;; 3. Se simula un stream con una combinación de pares (Curso . Grupo) como las que entrega generador.rkt
  (define stream-pares-prueba
    (stream (list (cons c-ce2103 g1-ce2103)
                  (cons c-cs2101 g1-cs2101)
                  (cons c-ma2104 g4-ma2104))))

  ;; 4. Se transforma el stream de pares a un stream de structs Combinacion
  (define stream-combinaciones-prueba (stream-pares->combinaciones stream-pares-prueba))

  (displayln "--- filtrar-por-creditos ---")

  ;; Caso 1: La combinación suma 10 créditos y el rango es [8, 12], por lo que sí pasa el filtro
  (displayln "caso dentro de rango [8, 12] (la combinacion suma 10 creditos):")
  (define validas-rango (filtrar-por-creditos stream-combinaciones-prueba 8 12))
  (stream->list validas-rango)

  ;; Caso 2: La combinación suma 10 créditos y el rango es [11, 15], por lo que se descarta y queda vacío
  (displayln "caso fuera de rango [11, 15] (debe dar lista vacia):")
  (define invalidas-rango (filtrar-por-creditos stream-combinaciones-prueba 11 15))
  (stream->list invalidas-rango)

  ;; Caso 3: Se reutiliza cumple-criterios? con otro tipo de dato (struct Curso) para validar que sea matriculable y de semestre 3
  (displayln "--- cumple-criterios? con struct Curso ---")
  (define criterios-curso
    (list Curso-matriculable
          (lambda (c) (= (Curso-semestre c) 3))))
  (cumple-criterios? criterios-curso c-ce2103))
