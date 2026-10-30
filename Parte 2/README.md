# CEmestre — Etapa 2: Paradigma Funcional (Racket)

[← Volver al índice del proyecto](../README.md)

---

## 1. Descripción general

Esta etapa recibe el catálogo validado por la Etapa 1 (C) y, en Racket, con streams e inmutabilidad:

1. Carga el catálogo de cada carrera y las preferencias del estudiante (créditos mínimos/máximos y lista de cursos de interés, con los grupos que le sirven de cada uno).
2. Genera, de forma perezosa, las combinaciones de horario válidas: un grupo por cada curso de interés que esté matriculable, sin que ningún par de grupos elegidos choque entre sí.
3. Filtra esas combinaciones por el rango de créditos indicado.
4. Exporta el resultado a un archivo JSON por carrera, que es el insumo de la Etapa 3 (Prolog).

Tal como indica el enunciado, esta etapa **no** arma un horario final, no optimiza ni aplica preferencias más allá de las explícitamente descritas arriba, no tiene interfaz, y no aplica reglas de restricción más allá de créditos (eso lo hace Prolog).

No se usa mutación en ningún archivo: no hay `set!`, ni estructuras mutables, ni variables globales reasignadas. El estado (qué grupos ya se eligieron, qué combinaciones van saliendo) siempre se pasa como argumento o se construye como un valor nuevo, nunca se modifica en el lugar.

---

## 2.1. Arquitectura del proyecto

La arquitectura de la Etapa 2 se organiza de forma modular, dividiendo las responsabilidades entre la definición estricta de tipos, la deserialización de entradas, la evaluación de compatibilidad, la generación de *streams* y el filtrado funcional:

```text
Parte 2/
├── constantes.rkt          Centralización de rutas de entrada/salida por carrera (Typed Racket)
├── tipos.rkt               Structs inmutables del dominio: Bloque-Horario, Grupo, Curso,
│                            Curso-Interes, Preferencias, Curso-Seleccionado, Combinacion (Typed Racket)
├── lector.rkt              Carga y conversión del catálogo JSON (Etapa 1) a structs Curso
├── preferencias.rkt        Carga y conversión del JSON de preferencias a struct Preferencias
├── candidatos.rkt          Resuelve cada curso de interés en el catálogo y filtra los grupos pedidos
├── choques.rkt             Predicados para detectar choques entre dos bloques o dos grupos
├── generador.rkt           Motor de generación perezosa de combinaciones sin choques mediante streams
├── filtros.rkt             Polimorfismo paramétrico, conversión a Combinacion y filtro por créditos
├── procesador.rkt          Módulo orquestador del pipeline de candidatos, generador y conversión
├── exportador.rkt          Serialización propia (objeto-json / arreglo-json) a JSON indentado
├── main.rkt                Punto de entrada principal: corre el flujo completo para cada carrera
└── data/
    ├── preferencias_CE.json Preferencias de matrícula para Computadores
    ├── preferencias_PI.json Preferencias de matrícula para Producción Industrial
    ├── salida_CE.json       Combinaciones finales generadas para CE (Insumo para Etapa 3)
    └── salida_PI.json       Combinaciones finales generadas para PI (Insumo para Etapa 3)
```

Cada archivo tiene una sola responsabilidad y sus propias pruebas en `(module+ main ...)`, para poder correr y revisar cada pieza por separado (`racket choques.rkt`, `racket candidatos.rkt`, etc.) antes de conectarla con las demás — así se construyó esta etapa, pieza por pieza, igual que la Etapa 1.

En cuanto a dependencias (`require`), `tipos.rkt` y `constantes.rkt` (en Typed Racket) son la base y no dependen de ningún otro módulo. Todos los demás archivos importan `tipos.rkt` para compartir las mismas estructuras inmutables: `lector.rkt`, `preferencias.rkt`, `candidatos.rkt`, `choques.rkt`, `filtros.rkt` y `exportador.rkt` dependen únicamente de `tipos.rkt` (más librerías estándar como `json` o `racket/stream`). Por encima de ellos, `generador.rkt` importa `choques.rkt`; `procesador.rkt` importa `candidatos.rkt`, `generador.rkt` y `filtros.rkt`; y finalmente `main.rkt` importa `constantes.rkt`, `tipos.rkt`, `lector.rkt`, `preferencias.rkt`, `procesador.rkt`, `filtros.rkt` y `exportador.rkt`.

**El flujo en `main.rkt`, para cada carrera:**

```
cargar-catalogo + cargar-preferencias
→ generar-combinaciones-sin-filtrar (candidatos.rkt + generador.rkt + filtros.rkt)
→ ¿quedó vacío? → aviso (por qué)
→ filtrar-por-creditos
→ ¿quedó vacío? → aviso (por qué)
→ exportar-combinaciones-json
```

### Explicación de la conexión

1. **Carga e Ingesta de Datos (`constantes.rkt`, `lector.rkt`, `preferencias.rkt`):**
   * `main.rkt` inicia el flujo en `procesar-carrera` leyendo las rutas definidas en `constantes.rkt` (`RUTA-CATALOGO-CE` / `RUTA-CATALOGO-PI` en `../Parte 1/`, y `RUTA-PREFERENCIAS-CE` / `RUTA-PREFERENCIAS-PI` en `data/`).
   * Invoca a `cargar-catalogo` en `lector.rkt`, que usa `read-json` y mapea los datos mediante `json->curso`, `json->grupo` y `json->bloque` hacia structs tipados de `Curso`, `Grupo` y `Bloque-Horario` definidos en `tipos.rkt`.
   * En paralelo, `cargar-preferencias` en `preferencias.rkt` deserializa el archivo JSON de configuración del estudiante (usando `json->curso-interes`) a un struct `Preferencias`.

2. **Selección de Candidatos (`procesador.rkt` → `candidatos.rkt`):**
   * `main.rkt` invoca `generar-combinaciones-sin-filtrar` en `procesador.rkt`, el cual toma el catálogo y las preferencias y ejecuta `candidatos-de-interes` por cada `Curso-Interes`.
   * Este módulo busca cada curso solicitado por el alumno con `buscar-curso`, confirma que esté marcado como `matriculable = #t` y filtra sus grupos con `filtrar-grupos-pedidos` según los números de grupo solicitados. Devuelve una lista limpia de pares `(cons Curso (listof Grupo))`.

3. **Generación Perezosa de Combinaciones y Control de Choques (`procesador.rkt` → `generador.rkt` → `choques.rkt`):**
   * La lista de candidatos ingresa a `generar-combinaciones-crudas` en `generador.rkt`.
   * Utilizando recursión e inmutabilidad, para cada curso de interés se prueba cada grupo candidato y `compatible-con-elegidos?` lo compara contra **todos** los grupos ya elegidos en esa rama (no solo el último), llamando a `grupos-chocan?` y `bloques-chocan?` en `choques.rkt` para verificar si algún bloque de horario se traslapa en el mismo día (`inicio_A < fin_B` e `inicio_B < fin_A`).
   * Gracias al uso de `stream-cons`, si un grupo choca, esa rama se descarta en el momento en que choca sin esperar a tener la combinación completa (poda al vuelo) — por eso el resultado es un stream perezoso de listas de pares `(cons Curso Grupo)`, no una lista que se construye toda de una vez y se filtra después.

4. **Transformación y Filtrado Paramétrico (`procesador.rkt` / `main.rkt` → `filtros.rkt`):**
   * Dentro de `generar-combinaciones-sin-filtrar`, el stream crudo de listas de pares `(cons Curso Grupo)` pasa a `stream-pares->combinaciones` en `filtros.rkt`, que aplica `stream-map` con `pares->combinacion` y `par->curso-seleccionado` para calcular el total de créditos, horas y carreras de origen, empaquetándolos en un stream de estructuras `Combinacion`.
   * Posteriormente, en `main.rkt`, `filtrar-por-creditos` llama a `filtrar-stream-por-criterios` y utiliza la función genérica con polimorfismo paramétrico `cumple-criterios?` (`parametric->/c`) para evaluar que la suma de créditos esté dentro del rango `[creditos-min, creditos-max]`.
   * `main.rkt` verifica con `stream-empty?` si el stream antes o después del filtro de créditos quedó vacío para construir, cuando corresponda, el mensaje `aviso`.

5. **Serialización y Salida (`main.rkt` → `exportador.rkt`):**
   * `main.rkt` invoca `exportar-combinaciones-json` en `exportador.rkt`, pasando la ruta de salida, el stream filtrado y el posible `aviso`.
   * Allí se materializa el stream con `stream->list`, se transforma cada combinación con `combinacion->json` → `seleccionado->json` → `grupo->json` → `bloque->json` hacia las estructuras `objeto-json` y `arreglo-json`, y `escribir-json` guarda en disco el archivo JSON indentado final (`data/salida_CE.json` o `data/salida_PI.json`).

---

## 2.2. Decisiones de diseño

### 2.2.1. Decisiones propias y justificación

**1. Cada carrera se procesa por separado, con su propio catálogo y su propio archivo de salida.**
Al principio se intentó combinar los catálogos de CE y PI en una sola lista, pero varios códigos existen en los dos catálogos a la vez (`FI1101`, `CS2101`, `FH1000`, `MA1103`, entre otros — cursos generales que ambas carreras comparten), cada uno con su propio `matriculable`/`aprobado` según el historial de *ese* estudiante. Juntarlos producía duplicados ambiguos: al buscar `FI1101`, el programa podía quedarse con la versión de la carrera equivocada. Además, en el TEC un estudiante matricula según la malla de **una sola** carrera — no existe el caso real de combinar ambas. Por eso cada carrera corre su propio flujo completo, con su propio `catalogo_XX.json` y su propio `preferencias_XX.json`, y produce su propio `salida_XX.json`.

**2. La combinatoria se acota a los cursos de interés del estudiante, no a todo el catálogo matriculable.**
El enunciado dice que el módulo "NO debe... aplicar preferencias del estudiante", pero también pide que el archivo de preferencias incluya "más de un grupo, cuando aplique" por curso de interés — una instrucción que solo tiene sentido si esta etapa efectivamente prueba cada grupo alternativo que el estudiante listó. Se interpretó así: "aplicar preferencias" se refiere a optimizar o elegir subjetivamente (qué horario es "mejor", qué curso adicional conviene agregar), no a usar los datos estructurados que el propio enunciado describe como entrada de esta etapa (la lista de interés y el rango de créditos). La alternativa — considerar *todos* los cursos matriculables del catálogo y decidir qué subconjunto incluir — se descartó: si se exige una combinación por cada curso matriculable, el total de créditos sería casi el mismo en todas las combinaciones (los créditos son del curso, no del grupo), y el filtro de créditos no tendría nada que filtrar. Para que ese filtro tenga sentido, hace falta que distintas combinaciones puedan incluir distintos cursos — y decidir cuáles es exactamente el tipo de criterio propio que el enunciado prohíbe.

**3. El formato de salida es un JSON propio, escrito a mano, no con `write-json`.**
Ver la justificación completa en la sección 2.2.3.

**4. El polimorfismo paramétrico se resolvió con `parametric->/c` de `racket/contract`, no con Typed Racket.**
`cumple-criterios?` revisa si un valor de cualquier tipo `A` cumple una lista de predicados sobre `A`. Se usa tal cual en el filtro de créditos (`A` = `Combinacion`) y se probó también con `A` = `Curso`, sin escribir una función distinta para cada caso. Se eligió un contrato en vez de `(All (A) ...)` de Typed Racket porque así la función corre directo dentro del flujo real (`filtros.rkt` es Racket normal, no Typed Racket), sin tener que mezclar un módulo tipado solo para esta función.

**5. `candidatos-de-interes` avisa y excluye, en vez de fallar.**
Si un curso de interés no existe en el catálogo, no es matriculable, o pide un número de grupo que no se oferta, se imprime un aviso y ese curso simplemente no entra en la combinatoria — el programa no se detiene por un dato de preferencias inválido. Es la misma filosofía de manejo de errores de la Etapa 1.

**6. Límite conocido: los cursos comodín (actividades culturales/deportivas) no se pueden seleccionar por `numero` de forma confiable.**
En el catálogo de la Etapa 1, un curso comodín como `SE1100` agrupa *todas* las actividades reales (`SE1101` Artes dramáticas, `SE1102` Acción social, etc.) como "grupos", y cada actividad reinicia su propia numeración desde 1 — por lo que el grupo 1 de una actividad y el grupo 1 de otra comparten el mismo número. `candidatos-de-interes` filtra por `numero`, así que pedir "`SE1100`, grupo 1" sería ambiguo entre actividades distintas. Con los datos de prueba actuales esto no se manifiesta, porque el estudiante de CE ya tiene aprobada su actividad cultural y deportiva (`SE1100`/`SE1200` salen `matriculable: false` y se excluyen antes de llegar a ese problema). Se documenta como limitación conocida: para resolverlo de raíz haría falta que el archivo de preferencias pueda pedir una actividad por su `codigo_opcion` (ej. `SE1105`) y no solo por número de grupo.

### 2.2.2. Caso límite real y cómo se resolvió

**El choque real entre `CE2103` y `PI3501`.**

Al construir el generador se necesitaba un caso de prueba verificable a mano, y resultó ser justo el tipo de caso límite que interesa documentar: `CE2103` (Algoritmos y Estructuras de Datos II, exclusivo de Computadores) tiene un único grupo, martes y jueves de 07:30 a 09:20. `PI3501` (Costos Industriales I, exclusivo de Producción Industrial) tiene 3 grupos:

| Grupo de `PI3501` | Horario | ¿Choca con `CE2103`? |
|---|---|---|
| 1 | Martes 07:30–10:20 | **Sí** (se traslapa el martes) |
| 2 | Martes 17:00–19:50 | No |
| 3 | Miércoles 13:00–15:50 | No (otro día) |

De las 3 combinaciones posibles (`CE2103` solo tiene 1 grupo, así que son 3), **solo 1** se descarta por choque. Esto sirvió para confirmar, con un caso real y verificado a mano, que `compatible-con-elegidos?` compara correctamente contra todos los grupos ya elegidos y que `generar-combinaciones-crudas` efectivamente descarta la rama del grupo 1 sin completarla.

**El caso límite del filtro de créditos: `preferencias_PI.json` nunca alcanza su propio mínimo.**

Al correr el programa con los datos reales de Producción Industrial se encontró que, sin importar qué grupo se elija de `FI1101`, `FI1201` y `PI3501`, los créditos totales de cualquier combinación siempre suman 7 (los créditos son del curso, no del grupo), mientras que `preferencias_PI.json` pedía un rango de `[10, 13]`. El resultado era un archivo JSON vacío, sin ninguna explicación.

**Cómo se resolvió:** se separó la generación en dos pasos (`generar-combinaciones-sin-filtrar` y después `filtrar-por-creditos`), para poder distinguir *por qué* el resultado quedó vacío: si no hubo ninguna combinación sin choques, o si sí las hubo pero ninguna cumple el rango de créditos. En este caso es lo segundo, y ahora el archivo de salida lo dice explícitamente:

```json
{
  "aviso": "Se generaron combinaciones sin choques, pero ninguna cumple el rango de creditos [10, 13].",
  "total_combinaciones": 0,
  "combinaciones": []
}
```

y el mismo mensaje se imprime por consola al correr `main.rkt`. Este caso se dejó así a propósito (sin ajustar el rango de `preferencias_PI.json`) porque demuestra, con datos reales, que el aviso funciona.

### 2.2.3. Justificación del formato de salida

**Serialización: JSON, escrito con un impresor propio (`exportador.rkt`), no con `write-json`.**
Se mantiene JSON por la misma razón que en la Etapa 1: Racket lo lee nativamente y los datos son jerárquicos (una combinación tiene varios cursos seleccionados, cada uno con un grupo, que tiene varios bloques de horario). No se usó la función `write-json` de la librería estándar por dos razones concretas: no tiene ninguna opción para indentar (escribe todo en una sola línea), y las tablas hash de Racket no garantizan el orden en que se recorren sus llaves, así que el orden de los campos en el archivo habría sido impredecible. En su lugar, cada "objeto" se representa como una lista **ordenada** de parejas (clave . valor) envuelta en el struct `objeto-json`, y un impresor recursivo propio (`escribir-json`) la escribe con la sangría y el orden que se decidió.

**Campos mínimos y justificados, no todo el catálogo de la Etapa 1 repetido.**
El enunciado pide como mínimo, por cada combinación: la lista de pares curso-grupo, los créditos totales, y la carrera de origen de cada curso incluido. La primera versión de este módulo exportaba además `requisitos`, `correquisitos`, `aprobado`, `cumple_requisitos`, `correquisitos_pendientes`, `matriculable`, `tiene_choque`, `horas`, `semestre` y `tipo` por curso — exactamente los datos que la Etapa 1 **ya usó** para decidir si ese curso-grupo podía entrar en una combinación. Una vez que el curso-grupo ya está dentro de una combinación válida, repetir esos campos no le aporta nada a la Etapa 3: por eso se recortaron. Lo que quedó por curso seleccionado es `codigo`, `carrera`, `nombre` y `creditos` (necesario para el total), más `grupo` con `numero`, `codigo_opcion` (en los comodines es lo único que dice qué actividad real se eligió) y `horarios` (`dia`, `inicio`, `fin` — sin `inicio_min`/`fin_min`, porque esos minutos se usaron para calcular los choques dentro de esta misma etapa, y para cuando una combinación ya quedó armada ese cálculo ya se hizo).

**El campo `aviso`.**
No pedido explícitamente, pero justificado por el caso límite de la sección 2.2.2: sin él, un resultado vacío no distingue "no hay combinaciones posibles" de "sí las hay, pero ninguna cumple el rango de créditos", y la Etapa 3 (o quien revise el archivo) se quedaría sin saber por qué.

---

## 2.3. Estructuras de datos desarrolladas

| Struct | Campos principales | Uso |
|---|---|---|
| `Bloque-Horario` | `dia`, `inicio`, `fin`, `inicio-min`, `fin-min` | Una sesión de clase |
| `Grupo` | `numero`, `codigo-opcion`, `nombre-opcion`, `horarios` | Un grupo ofertado, con sus bloques |
| `Curso` | `codigo`, `carrera`, `nombre`, `creditos`, `semestre`, `requisitos`, `correquisitos`, `matriculable`, `tiene-choque`, `grupos`... | Un curso del catálogo validado por la Etapa 1 |
| `Curso-Interes` | `codigo`, `numeros-grupos` | Un curso que el estudiante quiere, con los grupos que le sirven |
| `Preferencias` | `creditos-min`, `creditos-max`, `cursos-interes` | El archivo de preferencias completo |
| `Curso-Seleccionado` | Los mismos datos de `Curso`, más `grupo` (el elegido) | Un curso ya decidido dentro de una combinación |
| `Combinacion` | `seleccionados`, `creditos-totales`, `horas-totales`, `carreras-origen` | Una combinación de horario válida |
| `objeto-json` / `arreglo-json` | `pares` / `elementos` | Representación propia de JSON, con orden de campos controlado (ver 2.2.3) |

**Todos los structs del dominio (`tipos.rkt`) están en Typed Racket**, con tipos explícitos en cada campo. Los módulos de lógica (`lector.rkt`, `preferencias.rkt`, `candidatos.rkt`, `choques.rkt`, `generador.rkt`, `filtros.rkt`, `exportador.rkt`) están en Racket normal, pero **cada función pública tiene un contrato** (`racket/contract`) que revisa sus tipos de entrada y salida en tiempo de ejecución — la misma combinación de Typed Racket + contratos que ya usaba esta etapa desde que se cargó el catálogo por primera vez.

**Streams, no listas.** El resultado de `generador.rkt` y de cada paso de `filtros.rkt` es un stream (`stream-cons`, `stream-filter`, `stream-map`), no una lista completa. Solo se convierte a lista en un único punto de todo el programa: dentro de `exportar-combinaciones-json`, porque escribir el archivo obliga a recorrer todas las combinaciones aceptadas.

**Manejo de errores**, igual que en la Etapa 1: se reporta por consola y se excluye, en vez de detener el programa, cuando un curso de interés no existe en el catálogo, no es matriculable, o pide un grupo que no se oferta; y se avisa, tanto por consola como en el JSON de salida, cuando ninguna combinación cumple el rango de créditos.

---

## 3. Datos de entrada

- **Catálogo** (`catalogo_CE.json`, `catalogo_PI.json`): la salida de la Etapa 1, sin modificar su lógica de choques/requisitos, ahora con los primeros 6 semestres de cada carrera y el campo `carrera` por curso.
- **Preferencias** (`data/preferencias_CE.json`, `data/preferencias_PI.json`): créditos mínimos y máximos deseados, y la lista de cursos de interés con los números de grupo que le sirven a cada uno. Ejemplo real (CE):

```json
{
    "creditos_min": 8,
    "creditos_max": 12,
    "cursos_interes": [
        { "codigo": "CE2103", "numeros_grupos": [1] },
        { "codigo": "CS2101", "numeros_grupos": [1, 5] },
        { "codigo": "MA2104", "numeros_grupos": [4, 5] }
    ]
}
```

Con estos datos reales, CE genera 3 combinaciones (todas de 10 créditos, dentro del rango `[8, 12]`); PI genera 8 combinaciones sin choques, pero ninguna cumple su rango de créditos `[10, 13]` (ver el caso límite de la sección 2.2.2).

---

## 4. Compilación y ejecución

Requiere Racket (se probó con DrRacket / `racket` desde la línea de comandos). Desde la carpeta `Parte 2`:

```bash
racket main.rkt
```

Genera `data/salida_CE.json` y `data/salida_PI.json`. Cada archivo individual tiene sus propias pruebas y se puede correr por separado, por ejemplo:

```bash
racket choques.rkt
racket candidatos.rkt
racket generador.rkt
racket filtros.rkt
racket exportador.rkt
```
