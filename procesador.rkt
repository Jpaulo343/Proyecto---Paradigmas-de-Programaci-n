#lang racket

(require racket/contract
         racket/stream
         "tipos.rkt"
         "candidatos.rkt"
         "generador.rkt"
         "filtros.rkt")

(provide (contract-out
          [procesar-combinaciones (-> (listof Curso?) Preferencias? stream?)]))

;;;Función: procesar-combinaciones 
;;; Orquesta la generación, transformación y filtrado de combinaciones a partir del catálogo y preferencias.
(define (procesar-combinaciones catalogo preferencias)
  (define cursos-interes (Preferencias-cursos-interes preferencias))
  
  ;;Obtiene candidatos de interés habilitados
  (define lista-candidatos
    (filter values (map (lambda (ci) (candidatos-de-interes catalogo ci)) cursos-interes)))
  
  ;;Genera stream de combinaciones crudas (descartando choques)
  (define stream-crudas (generar-combinaciones-crudas lista-candidatos '()))
  
  ;;Transforma el stream de pares a structs Combinacion usando el módulo filtros
  (define stream-combos-structs (stream-pares->combinaciones stream-crudas))
  
  ;;Filtra por rango de créditos usando los criterios paramétricos 
  (filtrar-por-creditos stream-combos-structs
                        (Preferencias-creditos-min preferencias)
                        (Preferencias-creditos-max preferencias)))