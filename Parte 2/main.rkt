#lang racket

(require "constantes.rkt"
         "lector.rkt"
         "preferencias.rkt"
         "procesador.rkt"
         "filtros.rkt"
         "exportador.rkt"
         "tipos.rkt")

;;; Función: procesar-carrera
;;; Para una carrera: carga su catálogo y sus
;;; preferencias, genera las combinaciones sin choques, y antes de exportar
;;; revisa si el resultado quedó vacío para avisar por qué (no se encontró
;;; ninguna combinación sin choques, o sí las hubo pero ninguna cae en el
;;; rango de créditos pedido). El aviso se imprime por consola y también
;;; queda escrito dentro del archivo JSON, para que no quede solo un
;;; arreglo vacío.
;;; E: nombre (texto para el mensaje en pantalla), ruta-catalogo,
;;;    ruta-preferencias y ruta-salida (rutas de los archivos de esa carrera).
;;; S: (ninguna; imprime el progreso y escribe el archivo de salida).
(define (procesar-carrera nombre ruta-catalogo ruta-preferencias ruta-salida)
  (displayln nombre)

  (displayln "[1/3] Cargando catalogo y preferencias...")
  (define catalogo (cargar-catalogo ruta-catalogo))
  (define preferencias (cargar-preferencias ruta-preferencias))
  (define min-cred (Preferencias-creditos-min preferencias))
  (define max-cred (Preferencias-creditos-max preferencias))
  (displayln (format "      - Rango de creditos deseado: ~a a ~a" min-cred max-cred))

  (displayln "[2/3] Generando combinaciones validas sin choques (Streams)...")
  (define todas (generar-combinaciones-sin-filtrar catalogo preferencias))
  (define validas (filtrar-por-creditos todas min-cred max-cred))

  (define aviso
    (cond
      [(stream-empty? todas)
       "No se encontraron combinaciones sin choques con los cursos de interes indicados."]
      [(stream-empty? validas)
       (format "Se generaron combinaciones sin choques, pero ninguna cumple el rango de creditos [~a, ~a]."
               min-cred max-cred)]
      [else #f]))
  (when aviso (displayln (format "      Aviso: ~a" aviso)))

  (displayln "[3/3] Exportando combinaciones a JSON...")
  (exportar-combinaciones-json ruta-salida validas aviso)
  (displayln (format "      -> Archivo generado en: ~a\n" ruta-salida)))


(displayln "  CEmestre - Etapa 2 (Paradigma Funcional)\n")

(procesar-carrera "Ingenieria en Computadores (CE)" RUTA-CATALOGO-CE RUTA-PREFERENCIAS-CE RUTA-SALIDA-CE)
(procesar-carrera "Produccion Industrial (PI)" RUTA-CATALOGO-PI RUTA-PREFERENCIAS-PI RUTA-SALIDA-PI)

(displayln "Proceso finalizado para ambas carreras.")
