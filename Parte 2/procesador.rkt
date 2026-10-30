#lang racket

(require racket/contract
         racket/stream
         "tipos.rkt"
         "candidatos.rkt"
         "generador.rkt"
         "filtros.rkt")

(provide (contract-out
          [generar-combinaciones-sin-filtrar (-> (listof Curso?) Preferencias? stream?)]
          [procesar-combinaciones (-> (listof Curso?) Preferencias? stream?)]))

;;;Función: generar-combinaciones-sin-filtrar
;;; Genera el stream de combinaciones validas (un grupo por cada curso de
;;; interes habilitado, sin choques entre si),
;;; E: catalogo (lista de struct Curso) y preferencias (struct Preferencias).
;;; S: Stream de struct Combinacion, sin filtrar por creditos.
(define (generar-combinaciones-sin-filtrar catalogo preferencias)
  (define cursos-interes (Preferencias-cursos-interes preferencias))

  ;;Obtiene candidatos de interés habilitados
  (define lista-candidatos
    (filter values (map (lambda (ci) (candidatos-de-interes catalogo ci)) cursos-interes)))

  ;;Genera stream de combinaciones crudas (descartando choques)
  (define stream-crudas (generar-combinaciones-crudas lista-candidatos '()))

  ;;Transforma el stream de pares a structs Combinacion usando el módulo filtros
  (stream-pares->combinaciones stream-crudas))


;;;Función: procesar-combinaciones
;;; Orquesta la generación, transformación y filtrado de combinaciones a
;;; partir del catálogo y preferencias. Reutiliza generar-combinaciones-sin-filtrar
;;; y le aplica encima el filtro de créditos.
(define (procesar-combinaciones catalogo preferencias)
  (filtrar-por-creditos (generar-combinaciones-sin-filtrar catalogo preferencias)
                        (Preferencias-creditos-min preferencias)
                        (Preferencias-creditos-max preferencias)))