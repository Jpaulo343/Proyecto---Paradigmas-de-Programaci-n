El presente proyecto corresponde a la Etapa 1 (Paradigma Imperativo) del curso Paradigmas de Programación (CE1106). El objetivo es construir el motor de procesamiento en lenguaje C encargado de:

1. Parsear, limpiar y cargar los datos de los primeros 4 semestres de las carreras de Ingeniería en Computadores (CE) e Ingeniería en Producción Industrial (PI).

2. Procesar el historial del estudiante y validar la consistencia académica de requisitos y correquisitos.

3. Detectar choques de horarios entre los diferentes grupos ofertados en la Guía de Horarios institucional.

4. Evaluar cuáles cursos son actualmente elegibles para ser matriculados.

5. Exportar un contrato de datos en formato JSON para que sea consumido en la Etapa 2 (Paradigma Funcional en Racket).

## 2.1. Arquitectura del proyecto 

El sistema está desarrollado bajo una arquitectura modular y limpia en paradigma imperativo C, organizada de la siguiente manera:

Archivos de Configuración e Infraestructura:

`config.h`: Centraliza las rutas absolutas/relativas de los archivos de datos (CSV) y los archivos de salida (JSON), así como los identificadores de las carreras (`CARRERA_CE` y `CARRERA_PI`).

`constantes.h`: Define los tamaños máximos para buffers de texto (`TAM_CODIGO`, `TAM_NOMBRE`, etc.), límites de arreglos estáticos (`MAX_GRUPOS`, `MAX_BLOQUES`) y valores de retorno para gestión de errores (`EXITO`, `ERROR_ARCHIVO`).

`constantes_y_auxiliares.h`: Prototipos de funciones y definición del modelo de datos (`struct Curso`, `Grupo`, `BloqueHorario`, `Historial`, `Opcion` y la estructura genérica `Nodo`).

Núcleo del Motor (`constantes_y_auxiliares.c`):

Módulo de Lectura/Parsing: Ingesta de archivos CSV separando campos dinámicamente mediante `separarCampo` y manejo de cadenas con `quitarSaltoLinea`.

Módulo de Gestión de Memoria: Lista simplemente enlazada dinámica genérica (`insertarFinal`, `insertarInicio`, `liberarLista`).

Módulo de Validación Académica: Verificación de historial de materias aprobadas (`esCursoAprobado`), cumplimiento de prerrequisitos (`cumple_requisitos`, `validar_prerrequisitos`) y elegibilidad de correquisitos (`es_Correquisito_Posible`).

Módulo Horario y Detección de Choques: Conversión de horas (`horaAMinutos`, `minutosAHora`) y comparación cruzada de bloques de tiempo en el catálogo (`calcular_choques_catalogo`).

Módulo de Exportación: Serialización de la lista de cursos al contrato de salida JSON (`exportar_json`).

Punto de Entrada (`main.c`): Coordina la ejecución en orden secuencial para ambas carreras (CE y PI), gestionando la carga, procesamiento y posterior liberación de la memoria.

## 2.2. Decisiones de diseño 

### 2.2.1. Justificación de decisiones propias y específicas de su dataset. Deben dar ejemplos concretos con los planes de estudio. 
1. Gestión de Cursos Comodín y Opciones Electivas (`opciones.csv`):

- Problema en el Dataset: En los planes de estudio del TEC existen asignaturas generales o "comodines". Por ejemplo, en el plan de estudios figura la materia SE1100 (Actividad Cultural) o SE1400 (Actividad Deportiva). Sin embargo, en la oferta académica del semestre no aparece SE1100, sino materias específicas como SE1101 (Teatro), SE1102 (Danza) o SE1105 (Cine).

- Solución: Se implementó la estructura Opcion y un arreglo global que mapea estas materias. La función buscarCursoConOpciones permite buscar una oferta específica (ej: SE1101) y vincularla directamente al curso comodín raíz del plan (SE1100), manteniendo los datos de grupo y nombre de la opción en el struct Grupo para reflejarse fielmente en el JSON.

2. Representación del Tiempo en Minutos Continuos:
- Las horas del catálogo (ej: "07:30" a "09:20") se convierten a un número entero entero mediante `horaAMinutos` (ej: 7 * 60 + 30 = 450). Esto simplifica las comparaciones numéricas de intersección de rangos ($Inicio_A < Fin_B \land Inicio_B < Fin_A$) sin lidiar con parsing reiterado de texto.
### 2.2.2. Un caso límite real que encontraron y cómo lo resolvieron. 

**El caso límite - validación de cursos matriculables** 


Inicialmente el sistema solo consideraba los requisitos para determinar si se podía matricular un curso o no, sin tener en cuenta sus correquisitas,
pero se encontró un caso relacionado a los **correquisitos**, si el correquisito de un curso no se puede matricular, el curso no es elegible.

Por ejemplo, el curso *Laboratorio de Física II* tiene como correquisito *Física II*. Si el estudiante ya aprobó los requisitos del laboratorio
(*Laboratorio de Física I*), el sistema considera valido y lo marcaría como matriculable. 

Sin embargo, si el estudiante **no cumple los requisitos del correquisito** 
(En este caso no haber aprobado *Física I*) no debería ser capaz de matricular el curso, ya que,
si no puede matricular el correquisito, tampoco es posible matricular el laboratorio.

**Cómo lo resolvimos:**

Para solucionar esto, era necesario comprobar que el requisito fuera matriculable, entonces se implementó una función auxiliar llamada `es_Correquisito_Posible`. 
Esta función toma el correquisito y verifica si se puede matricular o no. Si el correquisito no es matriculable por falta de requisitos previos, 
el sistema bloquea automáticamente también el curso original.




  
### 2.2.3. Justificación del formato de salida ligada a una decisión de diseño real. 

Se seleccionó JSON como formato de exportación de datos. Esta decisión se basa directamente en la arquitectura global del proyecto:

- Interoperabilidad con la Etapa 2 (Racket): La siguiente fase del proyecto se desarrollará bajo el paradigma funcional utilizando el lenguaje Racket. Racket cuenta con librerías nativas (`json` / `jsexpr`) para leer archivos JSON y parsearlos a estructuras nativas de listas de asociación o tablas hash sin esfuerzo adicional.
- Representación Jerárquica Nivelada: Un plano CSV no permite expresar de forma limpia relaciones de uno a muchos sin duplicar filas (un `Curso` contiene $N$ `Grupos`, y un Grupo contiene $M$ `BloquesHorarios`). JSON permite representar esta jerarquía de forma natural y sin redundancia.
  
## 2.3. Estructuras de datos desarrolladas 
