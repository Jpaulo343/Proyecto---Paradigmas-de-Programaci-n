#lang racket

(require racket/contract
         "tipos.rkt")

(provide (contract-out
          [candidatos-de-interes (-> (listof Curso?) Curso-Interes? (or/c (cons/c Curso? (listof Grupo?)) #f))]))

;;; Función: candidatos-de-interes
;;; Busca en el catálogo el curso de un Curso-Interes y filtra sus grupos a
;;; los números que el estudiante pidió (si no pidió ninguno en especifico,
;;; se consideran todos los grupos ofertados). Si el curso no existe en el
;;; catálogo, no es matriculable, o no tiene ningún grupo con esos números,
;;; se avisa por pantalla y se excluye para que no tire error el programa
;;; E: catalogo (lista de struct Curso) e interes (struct Curso-Interes).
;;; S: Un par (Curso . Grupos-candidatos) si el curso quedó habilitado, o #f si se excluye.
(define (candidatos-de-interes catalogo interes)
  (define codigo (Curso-Interes-codigo interes))
  (define curso (findf (lambda (c) (equal? (Curso-codigo c) codigo)) catalogo))
  (cond
    [(not curso)
     (displayln (format "~a no esta en el catalogo, se excluye de las combinaciones" codigo))
     #f]
    [(not (Curso-matriculable curso))
     (displayln (format "~a no es matriculable, se excluye de las combinaciones" codigo))
     #f]
    [else
     (define numeros (Curso-Interes-numeros-grupos interes))
     (define todos-los-grupos (Curso-grupos curso))
     (define candidatos
       (if (null? numeros)
           todos-los-grupos
           (filter (lambda (g) (and (member (Grupo-numero g) numeros) #t)) todos-los-grupos)))
     (cond
       [(null? candidatos)
        (displayln (format "~a no tiene grupos ofertados para los numeros solicitados" codigo))
        #f]
       [else (cons curso candidatos)])]))


;;; Pruebas
(module+ main
  (define grupo1 (Grupo 1 "CE2103" "AED II" '()))
  (define grupo2 (Grupo 2 "CE2103" "AED II" '()))
  (define grupo3 (Grupo 3 "CE2103" "AED II" '()))

  (define ce2103 (Curso "CE2103" "CE" "AED II" 4 12 3 "CURSO" '() '() #f #t '() #t #f (list grupo1 grupo2 grupo3)))
  (define ma2104 (Curso "MA2104" "CE" "Calculo superior" 4 4 3 "CURSO" '() '() #f #t '() #f #f (list grupo1)))
  (define catalogo-prueba (list ce2103 ma2104))

  (displayln "--- candidatos-de-interes ---")
  (displayln "caso normal, pide los grupos 1 y 3:")
  (candidatos-de-interes catalogo-prueba (Curso-Interes "CE2103" '(1 3)))

  (displayln "caso sin numeros de grupo, debe traer los 3:")
  (candidatos-de-interes catalogo-prueba (Curso-Interes "CE2103" '()))

  (displayln "caso curso no matriculable, debe avisar y dar #f:")
  (candidatos-de-interes catalogo-prueba (Curso-Interes "MA2104" '(1)))

  (displayln "caso curso que no existe en el catalogo, debe avisar y dar #f:")
  (candidatos-de-interes catalogo-prueba (Curso-Interes "FI1101" '(1)))

  (displayln "caso pide un grupo que no se oferta, debe avisar y dar #f:")
  (candidatos-de-interes catalogo-prueba (Curso-Interes "CE2103" '(9))))
