#lang racket

(require racket/contract
         "tipos.rkt"
         "choques.rkt")

(provide (contract-out
          [compatible-con-elegidos? (-> Grupo? (listof (cons/c Curso? Grupo?)) boolean?)]
          [generar-combinaciones-crudas (-> (listof (cons/c Curso? (listof Grupo?)))
                                             (listof (cons/c Curso? Grupo?))
                                             stream?)]))

;;; Este módulo genera, de forma perezosa (con streams) e inmutable, las
;;; combinaciones de horario: probando incluir un grupo compatible por cada
;;; curso de la lista de pendientes o bien omitir ese curso para formar
;;; subconjuntos válidos sin que ningún par de grupos elegidos choque entre sí.

;;; eligió" se pasa como argumento (elegidos) en cada llamada recursiva,
;;; nunca se modifica.


;;; Función: compatible-con-elegidos?
;;; Determina si un grupo candidato choca con alguno de los grupos ya seleccionados.
;;; E: grupo (struct Grupo candidato) y elegidos (lista de pares Curso . Grupo ya elegidos).
;;; S: #t si grupo no choca con ninguno de los ya elegidos, #f si sí.
(define (compatible-con-elegidos? grupo elegidos)
  (not (ormap (lambda (par) (grupos-chocan? grupo (cdr par))) elegidos)))


;;; Función: mi-stream-append
;;; Concatena varios streams de forma perezosa: no evalúa los demás strea hasta que se acabe el primero, y no fuerza ningún
;;; elemento más allá del que se está pidiendo.
;;; E: cero o más streams del mismo tipo de elemento.
;;; S: Un único stream perezoso con los elementos de todos, en orden.
(define (mi-stream-append . streams)
  (cond
    [(null? streams) empty-stream]
    [(stream-empty? (car streams)) (apply mi-stream-append (cdr streams))]
    [else (stream-cons (stream-first (car streams))
                        (apply mi-stream-append (stream-rest (car streams)) (cdr streams)))]))


;;; Función: generar-combinaciones-crudas
;;; Genera un stream con todas las combinaciones válidas de un grupo por cada curso.
;;; cursos como subconjuntos de ellos, excepto la combinación vacía).
;;; Descarta los grupos que chocan de forma inmediata en cada llamada recursiva.
;;; E: pendientes (lista de pares Curso . Listof Grupo) y elegidos (lista de pares Curso . Grupo).
;;; S: Stream de combinaciones, donde cada combinación es una lista de pares Curso . Grupo.

(define (generar-combinaciones-crudas pendientes elegidos)
  (cond
    [(null? pendientes)
     (if (null? elegidos)
         empty-stream
         (stream (reverse elegidos)))]
    [else
     (define curso (car (car pendientes)))
     (define grupos-candidatos (cdr (car pendientes)))
     (define resto-pendientes (cdr pendientes))
     (define grupos-compatibles
       (filter (lambda (g) (compatible-con-elegidos? g elegidos)) grupos-candidatos))
     ;;  Ramas donde si se matricula el curso (probando cada grupo compatible)
     (define ramas-con-curso
       (map (lambda (g) (generar-combinaciones-crudas resto-pendientes (cons (cons curso g) elegidos)))
            grupos-compatibles))

     ;; Rama donde no se matricula el curso
     (define rama-sin-curso
       (generar-combinaciones-crudas resto-pendientes elegidos))
     (apply mi-stream-append (append ramas-con-curso (list rama-sin-curso)))]))


;;; Pruebas
(module+ main
  (define ce2103-grupo1 (Grupo 1 "CE2103" "AED II"
                                (list (Bloque-Horario "MAR" "07:30" "09:20" 450 560)
                                      (Bloque-Horario "JUE" "07:30" "09:20" 450 560))))
  (define pi3501-grupo1 (Grupo 1 "PI3501" "Costos I" (list (Bloque-Horario "MAR" "07:30" "10:20" 450 620))))
  (define pi3501-grupo2 (Grupo 2 "PI3501" "Costos I" (list (Bloque-Horario "MAR" "17:00" "19:50" 1020 1190))))
  (define pi3501-grupo3 (Grupo 3 "PI3501" "Costos I" (list (Bloque-Horario "MIE" "13:00" "15:50" 780 950))))

  (define curso-ce2103 (Curso "CE2103" "CE" "AED II" 4 12 3 "CURSO" "X" '() '() #f #t '() #t #f (list ce2103-grupo1)))
  (define curso-pi3501 (Curso "PI3501" "PI" "Costos I" 3 3 2 "CURSO" "X" '() '() #f #t '() #t #f (list pi3501-grupo1 pi3501-grupo2 pi3501-grupo3)))

  (define pendientes (list (cons curso-ce2103 (list ce2103-grupo1))
                            (cons curso-pi3501 (list pi3501-grupo1 pi3501-grupo2 pi3501-grupo3))))

  (define crudas (generar-combinaciones-crudas pendientes '()))

  (displayln "--- generar-combinaciones-crudas ---")
  (displayln "primera combinacion, sin calcular las demas (laziness):")
  (for/list ([par (stream-first crudas)]) (list (Curso-codigo (car par)) (Grupo-numero (cdr par))))

  (displayln "todas las combinaciones (incluyendo subconjuntos donde falta algun curso):")
  (for/list ([combo (stream->list crudas)])
    (for/list ([par combo]) (list (Curso-codigo (car par)) (Grupo-numero (cdr par))))))
