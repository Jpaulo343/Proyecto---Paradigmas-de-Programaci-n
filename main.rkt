#lang racket

(require "constantes.rkt"
         "lector.rkt"
         "preferencias.rkt"
         "procesador.rkt"
         "exportador.rkt")

(define (ejecutar-etapa-2)
  (displayln "  CEmestre - Etapa 2 (Paradigma Funcional)")
  
  ;;Cargar Catálogos de C y Preferencias del Estudiante
  (displayln "\n[1/4] Cargando catalogos y preferencias...")
  (define catalogo-ce (cargar-catalogo RUTA-CATALOGO-CE))
  (define catalogo-pi (cargar-catalogo RUTA-CATALOGO-PI))
  (define catalogo-completo (append catalogo-ce catalogo-pi))
  
  (define preferencias (cargar-preferencias RUTA-PREFERENCIAS))
  (displayln (format "      - Rango de creditos deseado: ~a a ~a"
                     (Preferencias-creditos-min preferencias)
                     (Preferencias-creditos-max preferencias)))
  
  ;;Procesa y Genera Combinaciones (Streams)
  (displayln "\n[2/4] Generando combinaciones validas sin choques...")
  (define combinaciones-filtradas (procesar-combinaciones catalogo-completo preferencias))
  
  ;;Exportar Resultado a JSON para Prolog 
  (displayln "\n[3/4] Exportando combinaciones a JSON...")
  (exportar-combinaciones-json RUTA-SALIDA combinaciones-filtradas)
  
  (displayln (format "\n[4/4] Exito: Proceso finalizado. Archivo generado en: ~a" RUTA-SALIDA)))

;; Ejecutar flujo principal
(ejecutar-etapa-2)