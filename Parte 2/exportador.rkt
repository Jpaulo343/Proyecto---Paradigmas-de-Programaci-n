#lang racket

(require json
         racket/contract
         "tipos.rkt")

(provide (contract-out
          [combinacion->jsexpr (-> Combinacion? jsexpr?)]
          [exportar-combinaciones-json (-> path-string? stream? void?)]))

;;; Convierte un struct Curso-Seleccionado a expresiones compatibles con JSON 
(define (curso-seleccionado->jsexpr cs)
  (hasheq
   'codigo_curso (Curso-Seleccionado-codigo-curso cs)
   'carrera (Curso-Seleccionado-carrera cs)
   'nombre_curso (Curso-Seleccionado-nombre-curso cs)
   'creditos (Curso-Seleccionado-creditos cs)
   'horas (Curso-Seleccionado-horas cs)
   'semestre (Curso-Seleccionado-semestre cs)
   'tipo (Curso-Seleccionado-tipo cs)
   'requisitos (Curso-Seleccionado-requisitos cs)
   'correquisitos (Curso-Seleccionado-correquisitos cs)
   'aprobado (Curso-Seleccionado-aprobado cs)
   'cumple_requisitos (Curso-Seleccionado-cumple-requisitos cs)
   'correquisitos_pendientes (Curso-Seleccionado-correquisitos-pendientes cs)
   'matriculable (Curso-Seleccionado-matriculable cs)
   'tiene_choque (Curso-Seleccionado-tiene-choque cs)
   'grupo (let ([g (Curso-Seleccionado-grupo cs)])
            (hasheq
             'numero (Grupo-numero g)
             'codigo_opcion (Grupo-codigo-opcion g)
             'nombre_opcion (Grupo-nombre-opcion g)
             'horarios (map (lambda (b)
                              (hasheq
                               'dia (Bloque-Horario-dia b)
                               'inicio (Bloque-Horario-inicio b)
                               'fin (Bloque-Horario-fin b)
                               'inicio_min (Bloque-Horario-inicio-min b)
                               'fin_min (Bloque-Horario-fin-min b)))
                            (Grupo-horarios g))))))

;;; Convierte un struct Combinacion a jsexpr
(define (combinacion->jsexpr combo)
  (hasheq
   'creditos_totales (Combinacion-creditos-totales combo)
   'horas_totales (Combinacion-horas-totales combo)
   'carreras_origen (Combinacion-carreras-origen combo)
   'seleccionados (map curso-seleccionado->jsexpr (Combinacion-seleccionados combo))))

;;; Exporta el stream completo de combinaciones válidas a un archivo JSON
(define (exportar-combinaciones-json ruta stream-combos)
  (define lista-combos (stream->list stream-combos))
  (define datos-json
    (hasheq
     'total_combinaciones (length lista-combos)
     'combinaciones (map combinacion->jsexpr lista-combos)))
  
  (call-with-output-file ruta
    (lambda (out)
      (write-json datos-json out))
    #:exists 'replace))
