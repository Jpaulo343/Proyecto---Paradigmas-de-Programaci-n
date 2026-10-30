#lang racket

(require "constantes.rkt"
         "lector.rkt"
         "preferencias.rkt"
         "procesador.rkt"
         "exportador.rkt"
         "tipos.rkt")

(define (ejecutar-etapa-2)
  (displayln "  CEmestre - Etapa 2 (Paradigma Funcional)\n")
  
  ;; Ingeniería en Computadores (CE)
  (displayln " Ingenieria en Computadores (CE)")
  
  (displayln "[1/3] Cargando catalogo y preferencias de CE...")
  (define catalogo-ce (cargar-catalogo RUTA-CATALOGO-CE))
  (define preferencias-ce (cargar-preferencias RUTA-PREFERENCIAS-CE))
  
  (displayln (format "      - Rango de creditos deseado: ~a a ~a"
                     (Preferencias-creditos-min preferencias-ce)
                     (Preferencias-creditos-max preferencias-ce)))
  
  (displayln "[2/3] Generando combinaciones validas sin choques (Streams)...")
  (define combos-ce (procesar-combinaciones catalogo-ce preferencias-ce))
  
  (displayln "[3/3] Exportando combinaciones de CE a JSON...")
  (exportar-combinaciones-json RUTA-SALIDA-CE combos-ce)
  (displayln (format "      -> Archivo generado con exito en: ~a\n" RUTA-SALIDA-CE))


  ;; Producción Industrial (PI)
  (displayln "Procesando Carrera: Produccion Industrial (PI)")
  
  (displayln "[1/3] Cargando catalogo y preferencias de PI...")
  (define catalogo-pi (cargar-catalogo RUTA-CATALOGO-PI))
  (define preferencias-pi (cargar-preferencias RUTA-PREFERENCIAS-PI))
  
  (displayln (format "      - Rango de creditos deseado: ~a a ~a"
                     (Preferencias-creditos-min preferencias-pi)
                     (Preferencias-creditos-max preferencias-pi)))
  
  (displayln "[2/3] Generando combinaciones validas sin choques (Streams)...")
  (define combos-pi (procesar-combinaciones catalogo-pi preferencias-pi))
  
  (displayln "[3/3] Exportando combinaciones de PI a JSON...")
  (exportar-combinaciones-json RUTA-SALIDA-PI combos-pi)
  (displayln (format "      -> Archivo generado con exito en: ~a\n" RUTA-SALIDA-PI))

  (displayln "Proceso finalizado para ambas carreras."))

;; Ejecutar flujo principal
(ejecutar-etapa-2)