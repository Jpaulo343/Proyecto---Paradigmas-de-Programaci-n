# CEmestre — Etapa 1: Paradigma Imperativo (C)

Instituto Tecnológico de Costa Rica · Escuela de Ingeniería en Computadores
Paradigmas de Programación (CE1106) · II Semestre 2026

**Integrantes**
- Jean Paulo Poveda Chaves
- Josafat Solano Quirós
- Camila Navarro Valverde

---

## 1. Descripción general

El presente proyecto corresponde a la Etapa 1 (Paradigma Imperativo) de **CEmestre**, un sistema que ayuda a un estudiante a construir su horario de matrícula. Esta etapa es un programa en C que:

1. Carga y limpia los datos de los primeros 4 semestres de las carreras de Ingeniería en Computadores (CE, plan 2103) e Ingeniería en Producción Industrial (PI, plan 1425).
2. Carga el historial del estudiante y valida que sea consistente con su plan (requisitos y correquisitos).
3. Detecta choques de horario entre los grupos ofertados en la Guía de Horarios institucional.
4. Determina qué cursos puede matricular el estudiante según su historial.
5. Exporta el catálogo completo a un archivo JSON, que es el insumo de la Etapa 2 (Racket).

Tal como indica el enunciado, esta etapa **no** arma un horario final, no sugiere combinaciones, no aplica preferencias del estudiante y no tiene interfaz: el resultado es un archivo.

---

## 2.1. Arquitectura del proyecto

```
Parte 1/
├── constantes.h                 Todas las constantes del proyecto
├── constantes_y_auxiliares.h    Structs y prototipos de las funciones
├── constantes_y_auxiliares.c    Implementación: carga, validaciones, choques y exportación
├── main.c                       Programa principal
└── data/
    ├── CEmestre_datos.xlsx      Fuente editable de todos los datos
    ├── plan_CE.csv  plan_PI.csv
    ├── historial_CE.csv  historial_PI.csv
    ├── oferta_CE.csv  oferta_PI.csv
    └── opciones.csv
```

- **`constantes.h`**: contiene únicamente constantes: tamaños de los textos (`TAM_CODIGO`, `TAM_NOMBRE`…), límites de los arreglos (`MAX_GRUPOS`, `MAX_BLOQUES`, `MAX_OPCIONES`), rutas de los archivos de entrada y salida, el comodín `COMODIN_CULTURAL_DEPORTIVO` (`"SE1400"`) y los códigos de salida (`EXITO`, `ERROR_ARCHIVO`, `ERROR_DATOS`). Ningún otro archivo tiene números o rutas escritos directamente.
- **`constantes_y_auxiliares.h`**: define el modelo de datos (`Curso`, `Grupo`, `BloqueHorario`, `Historial`, `Opcion` y el nodo genérico `Nodo`) y los prototipos documentados.
- **`constantes_y_auxiliares.c`**, organizado por responsabilidad:
  - *Lista enlazada genérica*: `insertarInicio`, `insertarFinal`, `liberarLista`.
  - *Lectura de CSV*: `separarCampo` (separa por `;` sin saltarse los campos vacíos), `quitarSaltoLinea`.
  - *Carga de datos*: `cargar_plan_estudios`, `cargar_historial`, `cargar_oferta`, `cargar_opciones`.
  - *Validaciones*: `validar_historial`, `validar_prerrequisitos`, `esCursoAprobado`, `cumple_requisitos`, `es_Correquisito_Posible`.
  - *Matrícula*: `actualizar_matriculables`, `quitar_actividades_llevadas`.
  - *Horarios y choques*: `horaAMinutos`, `minutosAHora`, `calcular_choques_catalogo`.
  - *Exportación*: `exportar_json`.
- **`main.c`**: ejecuta el flujo para cada carrera en este orden:

```
cargar_opciones → cargar_plan_estudios → cargar_oferta → calcular_choques_catalogo
→ cargar_historial → validar_historial + validar_prerrequisitos
→ quitar_actividades_llevadas → actualizar_matriculables → exportar_json
```

Si el historial no es consistente con el plan, **no se exporta** el catálogo de esa carrera, porque el resultado sería incorrecto, y el programa termina con código de salida `ERROR_DATOS`.

---

## 2.2. Decisiones de diseño

### 2.2.1. Decisiones propias del dataset (con ejemplos de los planes)

**1. Un catálogo por carrera, porque los requisitos dependen del plan.**
El mismo curso puede tener reglas distintas según la carrera. *Física General I* (`FI1101`) tiene a `MA1102` como **requisito** en Computadores, pero como **correquisito** en Producción Industrial. Lo mismo pasa con *Ambiente Humano* (`CS2101`): no tiene requisitos en CE, pero en PI requiere `CI1107`. Por eso cada carrera tiene su propio plan, su propia oferta y su propio archivo de salida.

**2. Cursos comodín y actividades culturales/deportivas (`opciones.csv`).**
En el plan aparecen `SE1100` (Actividad cultural I), `SE1200` (Actividad deportiva I) y `SE1400` (Actividad cultural-deportiva), pero en la Guía de Horarios esos códigos nunca aparecen. Lo que se oferta son actividades concretas, por ejemplo `SE1101` Artes dramáticas, `SE1105` Artes musicales, `SE1107` Apreciación de cine, `SE1204` Natación o `SE1209` Béisbol-softbol, y el estudiante elige cuál llevar.
Se modeló así: el comodín es el curso del plan y **cada actividad aparece como un grupo del comodín**, identificado con su código real (`codigo_opcion`). La estructura `Opcion` guarda qué actividad cumple qué comodín.
- `SE1100` recibe los grupos de las actividades culturales (`SE11xx`) y `SE1200` los de las deportivas (`SE12xx`).
- `SE1400` recibe los grupos de **todas** las actividades, pero `quitar_actividades_llevadas` elimina las que el estudiante ya llevó, porque una actividad no se puede repetir. En el caso de prueba de CE, el estudiante llevó `SE1105` y `SE1209`, así que `SE1400` queda con 15 actividades de 17.

**3. Pruebas de inglés excluidas.**
Por indicación del profesor, las pruebas de inglés (`CI0205` en CE y `CI0200` en PI) no se cuentan, porque su matrícula es aparte de la regular. Al quitarlas, *Inglés I* (`CI1230`, PI) queda solo con el requisito `CI0202`, para que no apunte a un curso que ya no existe.

**4. Centros de Formación Humanística (`FH1000`) como un solo curso.**
Los CFH no se ofrecen por semestre sino en ciclos propios. Por indicación del profesor se modelaron como un único curso con un horario representativo: el de `FH0035` *Globalización y humanismo* (martes 17:00–19:50, Cartago).

**5. Se incluye el bloque 0 del plan.**
El bloque 0 (nivelación) no es parte de "los 4 primeros semestres", pero sus cursos son requisito de cursos del bloque 1 (por ejemplo, `MA0101` es requisito de `MA1102`). Si no se incluyera, esos requisitos apuntarían a cursos inexistentes.

**6. Correquisitos con dirección.**
Los correquisitos no siempre son recíprocos: `CE1104` tiene a `CE1101` como correquisito, pero `CE1101` no tiene a `CE1104`. Solo algunos son mutuos, como `QU1102` ↔ `QU1106`. Por eso la validación revisa cada correquisito en la dirección en que aparece en el plan.

**7. Horas en minutos.**
Las horas se convierten a minutos desde las 00:00 con `horaAMinutos` ("07:30" → 450). Así, dos sesiones chocan si son el mismo día y `inicio_A < fin_B` y `inicio_B < fin_A`. Si una termina justo cuando empieza la otra, no hay choque. La guía tiene horas irregulares (por ejemplo 12:30–16:05 o 17:05), y con minutos se comparan sin problema.

**8. Límites de la oferta.**
`MAX_GRUPOS` es 70 porque hay cursos con muchos grupos (`CI1107` tiene 33 en PI) y `SE1400` recibe los grupos de todas las actividades (28 culturales + 34 deportivas = 62). Si un curso superara el límite, `cargar_oferta` lo reporta en vez de descartar grupos en silencio.

### 2.2.2. Caso límite real y cómo se resolvió

**El caso: un laboratorio matriculable cuyo correquisito no se puede llevar.**

Inicialmente el sistema solo consideraba los requisitos para determinar si se podía matricular un curso o no, sin tener en cuenta sus correquisitas,
pero se encontró un caso relacionado a los **correquisitos**, si el correquisito de un curso no se puede matricular, el curso no es elegible.

El estudiante de prueba de CE aprobó *Laboratorio de Física General I* (`FI1201`) pero reprobó la teoría, *Física General I* (`FI1101`). Esto es real y válido: son correquisitos, se llevan juntos, y se puede aprobar uno y reprobar el otro.

Con eso, *Laboratorio de Física General II* (`FI1202`) salía como **matriculable**, porque su requisito (`FI1201`) estaba aprobado. Pero su correquisito es *Física General II* (`FI1102`), que requiere `FI1101`. Como el estudiante no puede matricular `FI1102`, tampoco puede matricular `FI1202`.

**Cómo se resolvió:** Para solucionar esto, era necesario comprobar que el requisito fuera matriculable, entonces se implementó una función auxiliar llamada `es_Correquisito_Posible`. 
Esta función toma el correquisito y verifica si se puede matricular o no. Si el correquisito no es matriculable por falta de requisitos previos, 
el sistema bloquea automáticamente también el curso original.

Resultado en `catalogo_CE.json`:

```json
"codigo": "FI1202",
"requisitos": ["FI1201"],
"correquisitos": ["FI1102"],
"cumple_requisitos": true,
"correquisitos_pendientes": ["FI1102"],
"matriculable": false
```

**Un segundo problema que apareció al implementarlo:** la primera versión recorría los correquisitos con `strtok` y, adentro, `es_Correquisito_Posible` volvía a usar `strtok` para los requisitos. `strtok` guarda su posición en una sola variable interna, así que el de adentro dañaba el recorrido del de afuera, que terminaba leyendo memoria inválida. El síntoma fue que `QU1104` (PI) salía como no matriculable. Se resolvió recorriendo los correquisitos con `strchr` en lugar de `strtok`.

### 2.2.3. Justificación del formato de salida

Se eligió **JSON**:

- **Lo consume directamente la Etapa 2.** Racket incluye la librería `json`, y `read-json` convierte el archivo en listas y tablas hash sin escribir un parser.
- **Los datos son jerárquicos.** Un curso tiene N grupos y cada grupo tiene M sesiones. En CSV habría que repetir los datos del curso en cada fila o partirlo en varios archivos y volver a unirlos. En JSON la jerarquía es natural.
- **Lleva lo que necesita la etapa siguiente, no solo lo mínimo.** Además de los campos que pide el enunciado, cada curso incluye `correquisitos_pendientes` (los que habría que matricular junto con el curso) y cada sesión incluye la hora en texto y en minutos (`inicio_min`, `fin_min`), para que Racket compare horarios sin volver a convertirlos.

La entrada, en cambio, es **CSV**, porque Excel la exporta directamente y en C se lee con `fgets` y `separarCampo`, sin necesitar un parser de JSON.

---

## 2.3. Estructuras de datos desarrolladas

| Estructura | Campos principales | Uso |
|---|---|---|
| `BloqueHorario` | `dia`, `horaInicio`, `horaFin` (en minutos) | Una sesión de clase |
| `Grupo` | `numeroGrupo`, `codigoOpcion`, `nombreOpcion`, `bloques[MAX_BLOQUES]`, `cantidadBloques` | Un grupo ofertado. En los comodines, `codigoOpcion` indica la actividad real (ej. `SE1105`) |
| `Curso` | `codigo`, `nombre`, `creditos`, `horas`, `semestre`, `requisitos`, `correquisitos`, `tipo`, `matriculable`, `grupos[MAX_GRUPOS]`, `cantidadGrupos`, `tieneChoque` | Un curso del plan con su oferta y los resultados calculados |
| `Historial` | `codigo`, `aprobado`, `opcion` | Una fila del historial del estudiante |
| `Opcion` | `codigoPlan`, `codigoOpcion`, `nombreOpcion`, `categoria` | Qué actividad cumple qué comodín |
| `Nodo` | `void *dato`, `struct Nodo *siguiente` | Nodo de la lista enlazada genérica |

**Lista enlazada genérica con `void*`.** El plan y el historial se guardan en listas enlazadas simples, porque no se sabe de antemano cuántas líneas tiene cada archivo. El nodo guarda un `void*`, así que la misma lista sirve para `Curso` y para `Historial`. `insertarInicio` e `insertarFinal` reservan memoria para el nodo y para una copia del dato (`malloc` + `memcpy`); `insertarFinal` se usa al cargar para conservar el orden del archivo. `liberarLista` libera el dato y el nodo de cada elemento al terminar.

**Arreglo estático para las opciones.** Las opciones se guardan en un arreglo de tamaño fijo (`MAX_OPCIONES`), porque son pocas (34 filas), su cantidad es conocida y solo se consultan por código.

**Manejo de errores.** Se reportan, con el archivo y el número de línea cuando aplica:
- archivos que no se pueden abrir;
- valores inválidos en la columna `aprobado`;
- cursos del historial que no están en el plan, y cursos del plan que faltan en el historial;
- cursos aprobados sin sus requisitos aprobados;
- cursos que superan `MAX_GRUPOS` o `MAX_BLOQUES`.

El programa devuelve `EXITO` (0) si todo salió bien y `ERROR_DATOS` (2) si algo falló.

---

## 3. Datos de entrada: recolección y limpieza

**Fuentes**
- **Planes de estudio:** páginas oficiales del TEC (tec.ac.cr). Computadores plan 2103 y Producción Industrial plan 1425, última actualización 20 de octubre de 2025. Bloques 0 a 4.
- **Oferta de horarios:** Guía de Horarios institucional, año 2026, II semestre, Campus Tecnológico Central Cartago, modalidad semestral. Se descartaron otras sedes y otras modalidades (bimestre, cuatrimestre, etc.).

**Limpieza y verificación**
- Todos los datos se organizaron en `CEmestre_datos.xlsx` (hojas `Plan_CE`, `Plan_PI`, `Historial_CE`, `Historial_PI`, `Oferta_CE`, `Oferta_PI`, `Opciones`) y cada hoja se exporta como *CSV UTF-8* separado por `;`.
- Se usa `;` y no `,` porque hay nombres con comas, por ejemplo el CFH *"Sexo y género, en la vida y el amor"*.
- Se verificó que los créditos, horas y nombres de cada curso del plan coinciden con los de la Guía de Horarios.

**Formato de los archivos**

| Archivo | Columnas |
|---|---|
| `plan_XX.csv` | `codigo;nombre;creditos;horas;semestre;requisitos;correquisitos;tipo` (listas separadas por coma, vacío = no tiene; `tipo` = `CURSO` o `COMODIN`) |
| `historial_XX.csv` | `codigo;aprobado;opcion` (mismas filas que el plan; `aprobado` = `SI`/`NO`; `opcion` = actividad llevada, solo en SE1100/SE1200/SE1400) |
| `oferta_XX.csv` | `codigo;grupo;dia;inicio;fin;modalidad;profesor` (una fila por sesión) |
| `opciones.csv` | `codigo_plan;codigo_opcion;nombre_opcion;categoria` |

**Estudiantes de prueba incluidos**
- **CE:** aprobó los bloques 0 a 2 excepto `FI1101` (sí aprobó el laboratorio `FI1201`). Llevó `SE1105` para SE1100 y `SE1209` para SE1200.
- **PI:** aprobó los bloques 0 y 1, más `MA1102` y `CI1107`. Llevó `SE1107` para SE1100; SE1200 pendiente.

---

## 4. Formato de salida

Se generan `catalogo_CE.json` y `catalogo_PI.json`:

```json
{
  "carrera": "CE",
  "periodo": "2026-2",
  "cursos": [
    {
      "codigo": "FI1202",
      "nombre": "Laboratorio física general II",
      "creditos": 1, "horas": 2, "semestre": 3, "tipo": "CURSO",
      "requisitos": ["FI1201"],
      "correquisitos": ["FI1102"],
      "aprobado": false,
      "cumple_requisitos": true,
      "correquisitos_pendientes": ["FI1102"],
      "matriculable": false,
      "tiene_choque": true,
      "grupos": [
        {
          "numero": 1,
          "codigo_opcion": "FI1202",
          "nombre_opcion": "Laboratorio física general II",
          "horarios": [
            { "dia": "MAR", "inicio": "09:30", "fin": "11:20", "inicio_min": 570, "fin_min": 680 }
          ]
        }
      ]
    }
  ]
}
```

| Campo | Significado |
|---|---|
| `aprobado` | El estudiante ya aprobó el curso |
| `cumple_requisitos` | Todos los requisitos están aprobados |
| `correquisitos_pendientes` | Correquisitos no aprobados, que habría que matricular en el mismo semestre |
| `matriculable` | No está aprobado, se ofrece en el periodo, cumple requisitos y sus correquisitos se pueden llevar |
| `tiene_choque` | Al menos uno de sus grupos choca con un grupo de otro curso del catálogo |
| `codigo_opcion` | Código real del grupo; en los comodines es la actividad concreta (ej. `SE1105`) |
