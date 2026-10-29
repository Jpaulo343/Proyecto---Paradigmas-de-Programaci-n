#lang typed/racket

(provide (struct-out Bloque-Horario)
         (struct-out Grupo)
         (struct-out Curso)
         (struct-out Curso-Interes)
         (struct-out Preferencias)
         (struct-out Curso-Seleccionado)
         (struct-out Combinacion))

;;; Estructura: Bloque-Horario
;;; Representa una sesión semanal de clase de un grupo ofertado.
;;; Atributos:
;;;   - dia: Día de la semana (ej. "MAR", "JUE").
;;;   - inicio: Hora de inicio en formato texto ("07:30").
;;;   - fin: Hora de fin en formato texto ("09:20").
;;;   - inicio-min: Minutos desde las 00:00 para la hora de inicio (ej. 450).
;;;   - fin-min: Minutos desde las 00:00 para la hora de fin (ej. 560).
(struct Bloque-Horario ([dia : String]
                        [inicio : String]
                        [fin : String]
                        [inicio-min : Integer]
                        [fin-min : Integer])
  #:transparent)


;;; Estructura: Grupo
;;; Representa un grupo ofertado con sus bloques de horario y datos de la actividad.
;;; Atributos:
;;;   - numero: Número entero de grupo.
;;;   - codigo-opcion: Código real (en comodines indica la actividad, ej. "SE1105").
;;;   - nombre-opcion: Nombre de la actividad o curso.
;;;   - horarios: Lista de struct Bloque-Horario del grupo.
(struct Grupo ([numero : Integer]
               [codigo-opcion : String]
               [nombre-opcion : String]
               [horarios : (Listof Bloque-Horario)])
  #:transparent)


;;; Estructura: Curso
;;; Representa un curso individual del catálogo validado por C.
;;; Atributos:
;;;   - codigo: Código del curso (ej. "MA1102").
;;;   - carrera: Carrera que aporta el curso ("CE" o "PI").
;;;   - nombre: Nombre del curso en el plan.
;;;   - creditos: Cantidad de créditos del curso.
;;;   - horas: Horas semanales de dedicación.
;;;   - semestre: Nivel en el plan (0..6).
;;;   - tipo: Tipo de curso ("CURSO" o "COMODIN").
;;;   - requisitos: Lista de códigos de cursos requisito según el plan.
;;;   - correquisitos: Lista de códigos de cursos correquisito según el plan.
;;;   - aprobado: #t si el estudiante ya aprobó el curso en su historial.
;;;   - cumple-requisitos: #t si el estudiante aprobó todos los requisitos del curso.
;;;   - correquisitos-pendientes: Lista de códigos de correquisitos aún no aprobados.
;;;   - matriculable: #t si el módulo de C determinó que el estudiante puede llevarlo.
;;;   - tiene-choque: #t si al menos uno de sus grupos choca con otro curso del catálogo.
;;;   - grupos: Lista de struct Grupo ofertados para este curso.
(struct Curso ([codigo : String]
               [carrera : String]
               [nombre : String]
               [creditos : Integer]
               [horas : Integer]
               [semestre : Integer]
               [tipo : String]
               [requisitos : (Listof String)]
               [correquisitos : (Listof String)]
               [aprobado : Boolean]
               [cumple-requisitos : Boolean]
               [correquisitos-pendientes : (Listof String)]
               [matriculable : Boolean]
               [tiene-choque : Boolean]
               [grupos : (Listof Grupo)])
  #:transparent)


;;; Estructura: Curso-Interes
;;; Representa un curso que el estudiante desea matricular y los grupos que le sirven.
;;; Atributos:
;;;   - codigo: Código del curso de interés (ej. "CE2103").
;;;   - numeros-grupos: Lista de números de grupo deseados (ej. '(1 5)).
(struct Curso-Interes ([codigo : String]
                       [numeros-grupos : (Listof Integer)])
  #:transparent)


;;; Estructura: Preferencias
;;; Representa el archivo completo de preferencias de matrícula del estudiante.
;;; Atributos:
;;;   - creditos-min: Cantidad mínima de créditos deseados.
;;;   - creditos-max: Cantidad máxima de créditos deseados.
;;;   - cursos-interes: Lista de struct Curso-Interes solicitados.
(struct Preferencias ([creditos-min : Integer]
                      [creditos-max : Integer]
                      [cursos-interes : (Listof Curso-Interes)])
  #:transparent)


;;; Estructura: Curso-Seleccionado
;;; Representa un par (Curso + 1 Grupo específico elegido) dentro de una combinación,
;;; conservando toda la información del curso para exportarla hacia la Etapa 3 (Prolog).
;;; Atributos:
;;;   - codigo-curso: Código del curso seleccionado.
;;;   - carrera: Carrera de origen del curso ("CE" o "PI").
;;;   - nombre-curso: Nombre del curso seleccionado.
;;;   - creditos: Créditos que aporta el curso.
;;;   - horas: Horas semanales que aporta el curso.
;;;   - semestre: Semestre del curso en el plan.
;;;   - tipo: Tipo de curso ("CURSO" o "COMODIN").
;;;   - requisitos: Lista de códigos de requisitos del curso.
;;;   - correquisitos: Lista de códigos de correquisitos del curso.
;;;   - aprobado: #t si el curso figura como aprobado.
;;;   - cumple-requisitos: #t si cumple todos los requisitos previos.
;;;   - correquisitos-pendientes: Correquisitos que Prolog validará en la Etapa 3.
;;;   - matriculable: #t si el curso es elegible para matrícula.
;;;   - tiene-choque: Indicador de choque en catálogo proveniente de C.
;;;   - grupo: El struct Grupo específico elegido para este curso en la combinación.
(struct Curso-Seleccionado ([codigo-curso : String]
                            [carrera : String]
                            [nombre-curso : String]
                            [creditos : Integer]
                            [horas : Integer]
                            [semestre : Integer]
                            [tipo : String]
                            [requisitos : (Listof String)]
                            [correquisitos : (Listof String)]
                            [aprobado : Boolean]
                            [cumple-requisitos : Boolean]
                            [correquisitos-pendientes : (Listof String)]
                            [matriculable : Boolean]
                            [tiene-choque : Boolean]
                            [grupo : Grupo])
  #:transparent)


;;; Estructura: Combinacion
;;; Representa una combinación válida de horario generada por el sistema.
;;; Atributos:
;;;   - seleccionados: Lista de struct Curso-Seleccionado que integran el horario.
;;;   - creditos-totales: Suma total de créditos de la combinación.
;;;   - horas-totales: Suma total de horas semanales de la combinación.
;;;   - carreras-origen: Lista de carreras de origen de los cursos incluidos.
(struct Combinacion ([seleccionados : (Listof Curso-Seleccionado)]
                     [creditos-totales : Integer]
                     [horas-totales : Integer]
                     [carreras-origen : (Listof String)])
  #:transparent)
