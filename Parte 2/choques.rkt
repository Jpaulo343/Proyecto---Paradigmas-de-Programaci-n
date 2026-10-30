#lang racket

(require racket/contract
         "tipos.rkt")

(provide (contract-out
          [bloques-chocan? (-> Bloque-Horario? Bloque-Horario? boolean?)]
          [grupos-chocan? (-> Grupo? Grupo? boolean?)]))

;;; Función: bloques-chocan?
;;; Determina si dos bloques de horario se traslapan, deben ser el mismo dia y sus rangos en minutos
;;; deben intersecarse. Si un bloque termina justo cuando empieza el otro entonces no es choque.
;;; E: Dos struct Bloque-Horario a comparar.
;;; S: #t si los bloques se traslapan, #f en caso contrario.

(define (bloques-chocan? b1 b2)
  (and (equal? (Bloque-Horario-dia b1) (Bloque-Horario-dia b2))
       (< (Bloque-Horario-inicio-min b1) (Bloque-Horario-fin-min b2))
       (< (Bloque-Horario-inicio-min b2) (Bloque-Horario-fin-min b1))))

;;; Función: grupos-chocan?
;;; Determina si dos grupos chocan entre sí, un par de sus
;;; bloques se traslape para que los grupos sean incompatibles.
;;; E: Dos struct Grupo a comparar.
;;; S: #t si algún bloque de g1 choca con algún bloque de g2, #f si no.
(define (grupos-chocan? g1 g2)
  (ormap (lambda (b1)
           (ormap (lambda (b2) (bloques-chocan? b1 b2))
                  (Grupo-horarios g2)))
         (Grupo-horarios g1)))


;;; Pruebas
(module+ main
  (displayln "--- bloques-chocan? ---")
  (bloques-chocan? (Bloque-Horario "MAR" "07:30" "09:20" 450 560) (Bloque-Horario "MAR" "08:00" "10:00" 480 600))
  (bloques-chocan? (Bloque-Horario "MAR" "07:30" "09:20" 450 560) (Bloque-Horario "MAR" "09:20" "11:00" 560 660))
  (bloques-chocan? (Bloque-Horario "MAR" "07:30" "09:20" 450 560) (Bloque-Horario "MAR" "10:00" "11:00" 600 660))
  (bloques-chocan? (Bloque-Horario "MAR" "07:30" "09:20" 450 560) (Bloque-Horario "JUE" "07:30" "09:20" 450 560))
  (bloques-chocan? (Bloque-Horario "MAR" "07:30" "11:00" 450 660) (Bloque-Horario "MAR" "08:00" "09:00" 480 540))

  ;; Datos para grupos
  (define ce2103-g1 (Grupo 1 "CE2103" "AED II" (list (Bloque-Horario "MAR" "07:30" "09:20" 450 560) (Bloque-Horario "JUE" "07:30" "09:20" 450 560))))
  (define pi3501-g1 (Grupo 1 "PI3501" "Costos I" (list (Bloque-Horario "MAR" "07:30" "10:20" 450 620))))
  (define pi3501-g2 (Grupo 2 "PI3501" "Costos I" (list (Bloque-Horario "MAR" "17:00" "19:50" 1020 1190))))

  (displayln "--- grupos-chocan? ---")
  (grupos-chocan? ce2103-g1 pi3501-g1)
  (grupos-chocan? ce2103-g1 pi3501-g2))
