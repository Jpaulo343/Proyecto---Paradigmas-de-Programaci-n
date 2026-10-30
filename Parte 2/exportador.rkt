#lang racket

(require racket/contract
         "tipos.rkt")

(provide (contract-out
          [exportar-combinaciones-json (->* (path-string? stream?) ((or/c string? #f)) void?)]))

;;; Este módulo convierte las combinaciones a un formato JSON propio y lo
;;; escribe con indentación legible


;;; struct objeto-json: representa un objeto JSON. pares es una lista
;;; ordenada de (cons string valor-json), en el orden en que se deben escribir.
(struct objeto-json (pares) #:transparent)

;;; struct arreglo-json: representa un arreglo JSON. elementos es una lista
;;; de valores (cada uno un objeto-json, arreglo-json, string, numero o boolean).
(struct arreglo-json (elementos) #:transparent)


;;; Función: indentar
;;; Construye la sangría (2 espacios por nivel) para un nivel de anidamiento dado.
;;; E: nivel (entero, 0 = sin sangría).
;;; S: String con los espacios de esa sangría.
(define (indentar nivel)
  (make-string (* nivel 2) #\space))


;;; Función: escribir-json
;;; Escribe un valor-json en el puerto, con la sangría del nivel indicado.
;;; Es la función central: decide qué hacer según el tipo de valor y delega
;;; en escribir-objeto o escribir-arreglo cuando corresponde.
;;; E: valor (objeto-json, arreglo-json, string, numero o boolean), puerto
;;;    de salida, y nivel de anidamiento actual.
;;; S: (ninguna; escribe directamente en el puerto).
(define (escribir-json valor puerto nivel)
  (cond
    [(objeto-json? valor) (escribir-objeto (objeto-json-pares valor) puerto nivel)]
    [(arreglo-json? valor) (escribir-arreglo (arreglo-json-elementos valor) puerto nivel)]
    [(string? valor) (write valor puerto)]
    [(boolean? valor) (display (if valor "true" "false") puerto)]
    [(number? valor) (display valor puerto)]
    [else (error 'escribir-json "tipo de valor no soportado: ~a" valor)]))


;;; Función: escribir-objeto
;;; Escribe una lista de pares (clave . valor) como un objeto JSON indentado,
;;; una pareja por línea, con el valor de cada pareja escrito por escribir-json.
;;; E: pares (lista de (cons string valor-json)), puerto y nivel actual.
;;; S: (ninguna; escribe directamente en el puerto).
(define (escribir-objeto pares puerto nivel)
  (cond
    [(null? pares) (display "{}" puerto)]
    [else
     (define sangria-hijos (indentar (+ nivel 1)))
     (define total (length pares))
     (display "{\n" puerto)
     (for ([par pares] [i (in-naturals)])
       (display sangria-hijos puerto)
       (write (car par) puerto)
       (display ": " puerto)
       (escribir-json (cdr par) puerto (+ nivel 1))
       (unless (= i (sub1 total)) (display "," puerto))
       (display "\n" puerto))
     (display (indentar nivel) puerto)
     (display "}" puerto)]))


;;; Función: escribir-arreglo
;;; Escribe una lista de valores como un arreglo JSON indentado, un valor
;;; por línea, con cada valor escrito por escribir-json.
;;; E: elementos (lista de valor-json), puerto y nivel actual.
;;; S: (ninguna; escribe directamente en el puerto).
(define (escribir-arreglo elementos puerto nivel)
  (cond
    [(null? elementos) (display "[]" puerto)]
    [else
     (define sangria-hijos (indentar (+ nivel 1)))
     (define total (length elementos))
     (display "[\n" puerto)
     (for ([elem elementos] [i (in-naturals)])
       (display sangria-hijos puerto)
       (escribir-json elem puerto (+ nivel 1))
       (unless (= i (sub1 total)) (display "," puerto))
       (display "\n" puerto))
     (display (indentar nivel) puerto)
     (display "]" puerto)]))


;;; Función: bloque->json
;;; Convierte un struct Bloque-Horario a un objeto-json. No se incluyen
;;; inicio_min/fin_min: esos minutos se usaron para calcular los choques
;;; dentro de la Etapa 1 y de choques.rkt; para cuando una combinación ya
;;; quedó armada (sin choques), ese cálculo ya se hizo y lo que sirve
;;; después es el horario en sí (dia, inicio, fin), no los minutos.
(define (bloque->json b)
  (objeto-json (list (cons "dia" (Bloque-Horario-dia b))
                      (cons "inicio" (Bloque-Horario-inicio b))
                      (cons "fin" (Bloque-Horario-fin b)))))


;;; Función: grupo->json
;;; Convierte un struct Grupo a un objeto-json. Se incluye codigo_opcion
;;; porque en los cursos comodín (ej. SE1100) es lo único que dice cuál
;;; actividad real se eligió (ej. SE1105); en un curso normal coincide con
;;; su propio código.
(define (grupo->json g)
  (objeto-json (list (cons "numero" (Grupo-numero g))
                      (cons "codigo_opcion" (Grupo-codigo-opcion g))
                      (cons "horarios" (arreglo-json (map bloque->json (Grupo-horarios g)))))))


;;; Función: seleccionado->json
;;; Convierte un struct Curso-Seleccionado al par curso-grupo 
(define (seleccionado->json sel)
  (objeto-json (list (cons "codigo" (Curso-Seleccionado-codigo-curso sel))
                      (cons "carrera" (Curso-Seleccionado-carrera sel))
                      (cons "nombre" (Curso-Seleccionado-nombre-curso sel))
                      (cons "creditos" (Curso-Seleccionado-creditos sel))
                      (cons "grupo" (grupo->json (Curso-Seleccionado-grupo sel))))))


;;; Función: combinacion->json
;;; Convierte un struct Combinacion a un objeto-json con los campos mínimos
;;; que pide el enunciado: la lista de pares curso-grupo seleccionados, los
;;; créditos totales y la carrera de origen de cada curso incluido (una
;;; entrada por curso, en el mismo orden que seleccionados).
(define (combinacion->json c)
  (objeto-json (list (cons "creditos_totales" (Combinacion-creditos-totales c))
                      (cons "carreras_origen" (arreglo-json (Combinacion-carreras-origen c)))
                      (cons "seleccionados" (arreglo-json (map seleccionado->json (Combinacion-seleccionados c)))))))


;;; Función: exportar-combinaciones-json
;;; Recorre el stream de combinaciones ya filtradas y escribe el archivo JSON con indentación legible. Si
;;; aviso trae un mensaje, se agrega como un campo
;;; más en la raíz del archivo, para que no quede solo un arreglo vacío sin explicación.
;;; E: ruta (ruta del archivo de salida), stream-combos (stream de struct
;;;    Combinacion ya filtradas por créditos), y aviso (string con el
;;;    motivo si el resultado quedó vacío, o #f si no aplica).
;;; S: (ninguna; escribe el archivo en la ruta indicada).
(define (exportar-combinaciones-json ruta stream-combos [aviso #f])
  (define lista-combos (stream->list stream-combos))
  (define pares-base
    (list (cons "total_combinaciones" (length lista-combos))
          (cons "combinaciones" (arreglo-json (map combinacion->json lista-combos)))))
  (define pares-raiz (if aviso (cons (cons "aviso" aviso) pares-base) pares-base))
  (define raiz (objeto-json pares-raiz))
  (call-with-output-file ruta
    #:exists 'replace
    (lambda (puerto) (escribir-json raiz puerto 0) (newline puerto) (void))))


;;; Pruebas
(module+ main
  (define g1-ce2103 (Grupo 1 "CE2103" "AED II"
                           (list (Bloque-Horario "MAR" "07:30" "09:20" 450 560)
                                 (Bloque-Horario "JUE" "07:30" "09:20" 450 560))))
  (define sel (Curso-Seleccionado "CE2103" "CE" "AED II" 4 12 3 "CURSO" '() '() #f #t '() #t #f g1-ce2103))
  (define combo (Combinacion (list sel) 4 12 (list "CE")))

  (displayln "--- escribir-json (una sola combinacion de prueba) ---")
  (escribir-json (combinacion->json combo) (current-output-port) 0)
  (newline)

  (displayln "\n--- exportar-combinaciones-json a un archivo temporal ---")
  (exportar-combinaciones-json "prueba_exportador.json" (stream combo))
  (displayln (file->string "prueba_exportador.json"))
  (delete-file "prueba_exportador.json")

  (displayln "\n--- exportar-combinaciones-json con un stream vacio y aviso ---")
  (exportar-combinaciones-json "prueba_exportador_vacio.json" empty-stream
                               "ninguna combinacion cumple el rango de creditos [20, 25]")
  (displayln (file->string "prueba_exportador_vacio.json"))
  (delete-file "prueba_exportador_vacio.json"))
