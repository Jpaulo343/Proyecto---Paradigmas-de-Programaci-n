# CEmestre — Etapa 3: Paradigma Lógico (Prolog)

[← Volver al índice del proyecto](../README.md)

## 1. Descripción general

La Etapa 3 del proyecto CEmestre corresponde al paradigma lógico, implementado mediante Prolog.

El objetivo de esta etapa es evaluar las combinaciones de horarios generadas por la Etapa 2 (Racket), aplicando un conjunto de reglas de política académica para determinar si cada combinación es válida.

1. Balance entre carreras: verifica que ninguna carrera supere el porcentaje máximo de créditos establecido.
2. Límite por área: comprueba que una combinación no exceda la cantidad máxima de cursos permitidos en una misma área académica.
3. Incompatibilidad: verifica que una combinación no contenga simultáneamente dos cursos declarados incompatibles.

**Decisión de alcance:** cada combinación pertenece a un solo plan de estudios. Un estudiante matricula los cursos del plan de su propia carrera, así que Racket genera las combinaciones de CE y las de PI por separado y no se mezclan cursos de ambas carreras en una misma combinación. Las tres reglas están implementadas para el caso general (por eso `curso_sel/6` incluye la carrera), pero con este alcance el balance entre carreras y los pares incompatibles entre planes distintos no se activan con los datos reales; se comprueban con pruebas escritas a mano. El detalle y sus consecuencias están en la sección 2.2.1, punto 5.

Cuando una combinación incumple alguna regla, el programa registra la primera regla que viola y genera una descripción del motivo. Esta etapa no genera nuevas combinaciones ni vuelve a filtrar por créditos: su responsabilidad es evaluar las combinaciones recibidas y exportar los resultados para que puedan ser utilizados por la Etapa 4 (Java).

---

## 2.1. Arquitectura del proyecto

### Organización de archivos

```text
Parte 3/
├── constantes.pl
├── lector.pl
├── reglas.pl
├── politicas.pl
├── exportador.pl
├── main.pl
├── README.md
└── data/
    ├── salida_CE.json
    └── salida_PI.json
```

### Responsabilidad de cada archivo

- **`constantes.pl`:** define las rutas de entrada y salida para cada carrera (`ruta_entrada/2` y `ruta_salida/2`).

- **`lector.pl`:** lee los archivos JSON producidos por la Etapa 2. Convierte los diccionarios JSON en términos compuestos de Prolog y registra las combinaciones como hechos `combinacion_cargada/3`. Utiliza `library(http/json)`. En versiones recientes de SWI-Prolog se muestra el aviso informativo `Library was moved: library(http/json) --> library(json)`; no afecta el funcionamiento.

- **`politicas.pl`:** contiene los hechos de política académica: porcentaje máximo por carrera, máximo de cursos por área y pares de cursos incompatibles. Los valores se pueden modificar sin tocar las reglas de validación.

- **`reglas.pl`:** implementa la lógica de evaluación. Contiene los predicados que calculan los créditos por carrera, identifican las áreas, cuentan cursos y comprueban las tres restricciones, además de determinar cuál es la primera regla incumplida. Incluye pruebas con datos escritos a mano que se ejecutan al correr el archivo directamente (`swipl reglas.pl`).

- **`exportador.pl`:** conecta la carga de datos con la evaluación y la generación de resultados. Recorre todas las combinaciones mediante backtracking (`findall/3`), construye la estructura JSON de cada resultado y escribe el archivo de salida. Ofrece `procesar_y_exportar/2` y `procesar_y_exportar/4` (esta última además devuelve cuántas combinaciones se evaluaron y cuántas cumplen).

- **`main.pl`:** punto de entrada. Procesa las carreras CE y PI y muestra en consola un resumen por carrera.

- **`data/`:** contiene los JSON de salida de esta etapa. Los de entrada se leen desde `../Parte 2/data/`.

### Cómo se conectan los módulos durante la ejecución

1. **Inicio (`main.pl`):** se ejecuta `main/0`, que procesa primero CE y después PI.

2. **Rutas (`constantes.pl`):** `ruta_entrada/2` y `ruta_salida/2` indican qué archivos corresponden a cada carrera.

3. **Lectura (`lector.pl`):** se abre el JSON de Racket y se convierte en términos compuestos:
   - `combinacion/3`: identificador, créditos totales y cursos de la combinación.
   - `curso_sel/6`: código, carrera, área, nombre, créditos y grupo del curso.
   - `grupo/3`: número de grupo, código de opción y horarios.
   - `bloque/3`: día, hora de inicio y hora de fin.

   Cada combinación se registra como un hecho `combinacion_cargada/3` para poder recorrerlas por backtracking.

4. **Evaluación (`reglas.pl` y `politicas.pl`):** cada combinación se compara con los hechos de `politicas.pl`. Las reglas se revisan en este orden: balance entre carreras, límite por área, incompatibilidad. Al encontrar una violación se registra esa razón y no se revisan las siguientes.

5. **Resultados (`exportador.pl`):** se recorren todas las combinaciones con `findall/3` y cada resultado se transforma a JSON con los cursos, grupos, veredicto y razón.

6. **Finalización (`main.pl`):** se guarda el archivo de cada carrera y se muestra el resumen.

### Flujo general de información

```text
       ETAPA 2 (RACKET)
               |
               v
  Parte 2/data/salida_CE.json
  Parte 2/data/salida_PI.json
               |
               v
          lector.pl
     Lectura y conversión
       de datos a Prolog
               |
               v
        combinacion_cargada
               |
               v
         reglas.pl
       + politicas.pl
               |
               v
       Evaluación de reglas
       1. Balance de carreras
       2. Límite por área
       3. Incompatibilidad
               |
               v
         exportador.pl
       Recorrido y conversión
          de resultados
               |
               v
        data/salida_CE.json
        data/salida_PI.json
               |
               v
          ETAPA 4 (JAVA)
```

---

## 2.2. Decisiones de diseño

### 2.2.1. Modelado de las combinaciones y decisiones propias del proyecto

**1. Representación de las combinaciones mediante términos compuestos.**

Cada combinación se representa con:

```prolog
combinacion(Id, CreditosTotales, Cursos).
```

- `Id`: identificador de la combinación (se numeran desde 1 al leer el JSON).
- `CreditosTotales`: cantidad total de créditos.
- `Cursos`: lista de cursos seleccionados.

Cada curso:

```prolog
curso_sel(Codigo, Carrera, Area, Nombre, Creditos, Grupo).
```

El grupo elegido y cada bloque horario:

```prolog
grupo(Numero, CodigoOpcion, Horarios).
bloque(Dia, Inicio, Fin).
```

Se eligieron términos compuestos porque agrupan la información relacionada y se consultan por unificación y patrones, sin tener que volver al catálogo de la Etapa 1. Cada curso lleva consigo todo lo que las reglas necesitan: carrera (balance), área (límite) y código (incompatibilidad).

**2. Asignación de áreas académicas.**

El campo `area` se agrega en C: el plan de estudios de cada carrera (`plan_CE.csv`, `plan_PI.csv`) tiene una columna `area` por curso, C la lee y la exporta en el catálogo JSON, y Racket la propaga hasta la salida que consume Prolog. El criterio es el **campo académico del curso**, no el prefijo de su código ni la carrera donde aparece: dos cursos de la misma disciplina quedan en la misma área aunque pertenezcan a planes distintos.

| Área | Cursos en CE | Cursos en PI | Ejemplos |
|---|---|---|---|
| `ciencias_basicas` | 13 | 15 | MA1102 Cálculo, FI1101 Física I, QU1106 Química básica I |
| `computacion` | 11 | 1 | CE2103 Algoritmos y estructuras de datos II, CE3101 Bases de datos, CA2125 Elementos de computación |
| `electronica` | 13 | 0 | EL2113 Circuitos en corriente continua, CE1107 Fundamentos de arquitectura, CE4301 Arquitectura de computadores I |
| `industrial_gestion` | 2 | 24 | PI3501 Costos industriales I, PI4303 Control de producción, PI2609 Probabilidad y estadística |
| `humanidades_complementarias` | 7 | 12 | CS2101 Ambiente humano, CI1407 Habilidades de comunicación, CS3404 Seminario de ética |
| `administracion` | 1 | 0 | AE4208 Desarrollo de emprendedores |
| `X` | 3 | 3 | SE1100, SE1200, SE1400 |

Casos que conviene explicar:

- **`PI2609` (Probabilidad y estadística)** figura en el plan de CE pero se dicta como curso de producción industrial; su área es `industrial_gestion` aunque su carrera sea `CE`. Por eso área y carrera son campos distintos: la carrera dice de qué plan viene el curso; el área, a qué disciplina pertenece.
- **`CE1111` (Análisis numérico para la ingeniería)** está en `ciencias_basicas` porque es una materia de matemática aplicada, aunque su código empiece con `CE`.
- **`X`** es el área de los cursos comodín de actividad cultural y deportiva (`SE1100`, `SE1200`, `SE1400`). No son cursos académicos de ninguna disciplina, así que se agrupan en un área aparte con su propio máximo en `politicas.pl`.

Por ejemplo, en un horario de CE: `CE2103` es `computacion`, `MA2104` es `ciencias_basicas` y `CS2101` es `humanidades_complementarias`, de modo que Prolog puede contar cuántos cursos de cada área hay en la combinación.

Los máximos por área son parámetros de política (`computacion` e `industrial_gestion`, las áreas más amplias, permiten 4; el resto, 2) y se pueden cambiar sin tocar las reglas.

**3. Declaración de cursos incompatibles.**

Los pares incompatibles son hechos `incompatible/2` en `politicas.pl`:

```prolog
incompatible("PI2609", "PI2610").
incompatible("CE1101", "CA2125").
incompatible("CE3101", "PI3403").
```

| Par | Por qué son incompatibles |
|---|---|
| `PI2609` / `PI2610` | Es la misma materia (Probabilidad y estadística) en los dos planes: CE la lleva como `PI2609` y PI como `PI2610`. Un estudiante no debe llevar ambas. |
| `CE1101` / `CA2125` | Cursos introductorios de computación (Introducción a la programación y Elementos de computación) con contenido que se solapa. |
| `CE3101` / `PI3403` | Bases de datos y Sistema de información abordan el manejo de datos y sus contenidos se superponen. |

Los pares se identificaron revisando los dos planes de estudio en busca de cursos con equivalencias o superposición temática. Cumple el mínimo de dos pares entre los planes que pide el enunciado.

Cada par se declara **una sola vez** y la simetría se resuelve en `reglas.pl`, probando ambos sentidos:

```prolog
( incompatible(Codigo1, Codigo2)
; incompatible(Codigo2, Codigo1)
).
```

Así no importa el orden en que aparezcan los cursos y no se duplica información. Los tres pares son entre un curso del plan de CE y uno del plan de PI, por lo que solo pueden coincidir en una combinación que mezcle ambos planes (ver punto 5).

**4. Orden de evaluación de las reglas y un caso donde cambia la razón.**

El orden de prioridad para determinar la primera razón de rechazo es:

1. Balance entre carreras.
2. Límite por área.
3. Incompatibilidad.

Se implementa con condicionales encadenados (`->` y `;`) en `evaluar_combinacion/3`.

Caso donde el orden cambia el resultado. Se considera la combinación (de prueba) con `PI2609`, `PI2610`, `PI3501`, `PI3502` y `PI2304`, todos de área `industrial_gestion`, cuyo máximo es 4:

- Hay 5 cursos de `industrial_gestion` (5 > 4): incumple el límite por área.
- `PI2609` y `PI2610` forman un par incompatible: incumple la incompatibilidad.
- El balance no se incumple (el máximo por carrera es 100 %).

Como el límite por área se revisa antes que la incompatibilidad, Prolog reporta:

```prolog
resultado(false, limite_area(industrial_gestion))
```

Si se invierte el orden y se revisa primero la incompatibilidad, el mismo caso reporta:

```prolog
resultado(false, incompatibilidad("PI2609", "PI2610"))
```

Ambas reglas se incumplen en los dos casos; lo que cambia es cuál se informa, porque el programa solo devuelve la primera violada. Por eso el orden es parte del diseño.

**5. Cada combinación es de un solo plan de estudios.**

Racket genera las combinaciones de cada carrera por separado (CE con el catálogo de CE y PI con el de PI), de modo que ninguna combinación mezcla cursos de las dos carreras. Esto responde a que, en la práctica, un estudiante matricula los cursos de su propio plan. Consecuencias que se documentan para la defensa:

- Con una sola carrera, esa carrera aporta el 100 % de los créditos. La regla de balance **nunca se incumple mientras el máximo sea 100** (valor actual). Si el máximo baja de 100 (por ejemplo 60 o 40), **todas** las combinaciones se rechazan con `balance_carrera`: se verificó con CE (5 combinaciones) y PI (132 combinaciones), todas rechazadas. Es el comportamiento que pide el enunciado: una combinación de una sola carrera es válida mientras no supere su máximo.
- Los tres pares incompatibles son entre cursos de planes distintos, por lo que solo se activarían en una combinación que mezcle ambos planes. Las reglas están implementadas y probadas con datos escritos a mano (`swipl reglas.pl`) para ese caso.

**6. Uso del corte y la negación por falla.**

- **Razón de rechazo (`evaluar_combinacion/3`):** se usa `Condicion -> Entonces ; Otra ...`. El `->` equivale a un corte seguido de negación por falla: si `incumple_balance_carreras/3` tiene éxito, Prolog se compromete con esa rama (corte) y no prueba las demás reglas; si falla, continúa con la siguiente regla (negación por falla). Así la razón es siempre la primera regla violada en el orden elegido, y la rama final `Cumple = true, Razon = ninguna` solo se alcanza si ninguna regla se incumple.
- **Corte al final de `incumple_balance_carreras/3`, `incumple_limite_area/2` e `incumple_incompatibilidad/3`:** estos predicados buscan por backtracking una carrera, un área o un par que viole la regla. Con `!` se detienen en la primera solución, porque solo se necesita una razón; sin él, Prolog seguiría buscando más violaciones al reintentar.
- **Corte en `creditos_por_carrera_acc/4`:** las dos cláusulas recursivas son excluyentes (el curso es de la carrera buscada o no lo es). El `!` en la primera, que suma los créditos, evita que al reintentar Prolog tome también la segunda cláusula y cuente mal.

**7. Manejo del estado: sin `assert`/`retract` ni variables globales.**

El enunciado prohíbe almacenar resultados intermedios con `assert`/`retract` o variables globales. En esta etapa:

- El único uso de `assertz`/`retractall` está en `lector.pl` (`cargar_combinaciones/1`), y es únicamente para cargar los datos de entrada, que es lo permitido.
- Los créditos acumulados se llevan como argumento (acumulador) en `creditos_por_carrera_acc/4`.
- Los resultados se recolectan con `findall/3` y se pasan como argumento hasta el exportador.
- Las reglas devuelven su resultado por unificación (`Carrera`, `Area`, `Codigo1`, `Codigo2`, `Razon`).

### 2.2.2. Casos límite reales y cómo se resolvieron

**Caso 1: una combinación que alcanza exactamente el máximo permitido de cursos en un área.**

Si el máximo de `ciencias_basicas` es de dos cursos y una combinación contiene `MA1102` (Cálculo) y `FI1101` (Física I), tiene exactamente dos cursos de esa área. La condición implementada en `incumple_limite_area/2` es:

```prolog
Cantidad > Maximo
```

Se usa `>` y no `>=` porque la política establece que una combinación incumple solo cuando supera el límite, no cuando lo alcanza. Dos cursos cumplen; tres incumplen. Así no se rechazan combinaciones que usan correctamente toda la cantidad permitida.

**Caso 2: una carrera sin ninguna combinación.**

Cuando las preferencias de PI pedían entre 10 y 13 créditos pero los cursos de interés sumaban como máximo 7, Racket generó `salida_PI.json` con `"total_combinaciones": 0`, una lista vacía y un campo extra `aviso`. Prolog debía tolerarlo y no fallar: `dicts_a_combinaciones/3` tiene el caso base para la lista vacía, no se cargan hechos `combinacion_cargada/3`, `findall/3` devuelve `[]` y el exportador escribe un archivo válido con `total_combinaciones` en 0. El resumen muestra `0 combinaciones evaluadas, 0 cumplen`.

**Caso 3: error al convertir los cursos con datos reales.**

La primera versión de `dict_a_curso_sel/2` usaba la notación de diccionarios (`G.numero`) dentro de la cabeza de la cláusula, pero el grupo `G` solo se obtenía en el cuerpo. Con datos reales fallaba con `Arguments are not sufficiently instantiated`, algo que no se veía con las pruebas manuales. Se resolvió extrayendo cada campo en el cuerpo (`Codigo = C.codigo`, `G = C.grupo`, `Numero = G.numero`, ...) antes de construir el término.

---

## 2.2.3. Extensión de las etapas previas

El enunciado pide agregar el campo `area` sin modificar la lógica ya construida. Solo se propagó un dato nuevo:

- **Etapa 1 (C):** el plan de cada carrera incluye la columna `area`; C la lee en `cargar_plan_estudios` y la escribe en el catálogo JSON. Los planes se extendieron a los primeros 8 semestres de cada carrera reutilizando el mismo módulo con más datos.
- **Etapa 2 (Racket):** el struct `Curso` y el struct `Curso-Seleccionado` reciben el campo `area`; `lector.rkt` lo lee del catálogo, `filtros.rkt` lo copia al armar cada selección y `exportador.rkt` lo escribe en la salida. No se tocó la generación de combinaciones, los choques ni el filtro de créditos.
- **Etapa 3 (Prolog):** el área llega en `curso_sel/6` y es la que usa la regla de límite por área.

Ajuste justificado a la exportación de Racket: la salida ya incluía código, grupo, créditos y carrera; se agregó `area` porque es un campo obligatorio de esta etapa y la regla 2 no se puede evaluar sin él.

---

## 2.3. Representación de datos y predicados principales

### Predicados principales

| Predicado | Funcionalidad |
|---|---|
| `cargar_combinaciones/1` | Lee el JSON de entrada y carga las combinaciones como hechos. |
| `dict_a_curso_sel/2` | Convierte un diccionario de curso a un término compuesto. |
| `creditos_por_carrera/3` | Suma los créditos de una carrera (con acumulador). |
| `pertenece_area/2` | Relación entre un área y los cursos de la combinación (usable en más de un modo). |
| `contar_cursos_area/3` | Cuenta los cursos de una misma área. |
| `incumple_balance_carreras/3` | Comprueba si alguna carrera supera su porcentaje máximo. |
| `incumple_limite_area/2` | Comprueba si alguna área supera su cantidad máxima de cursos. |
| `incumple_incompatibilidad/3` | Identifica un par de cursos incompatibles en la combinación. |
| `evaluar_combinacion/3` | Aplica las reglas en orden y devuelve `resultado(Cumple, Razon)`. |
| `procesar_y_exportar/2` y `/4` | Cargan, evalúan y exportan todas las combinaciones (la versión `/4` devuelve los totales). |

### Predicado utilizado en más de un modo de consulta

Se implementó `pertenece_area/2`:

```prolog
pertenece_area(Area, Cursos) :-
    member(curso_sel(_, _, Area, _, _, _), Cursos).
```

Permite comprobar si un área aparece en una combinación y también enumerar las áreas de sus cursos cuando `Area` no está instanciada.

**Modo 1: área sin instanciar (`-`, `+`).** Obtiene las áreas de los cursos de la combinación:

```prolog
?- findall(Area,
           pertenece_area(Area, [
               curso_sel("CE2103", "CE", "computacion",
                         "AED II", 4, grupo(1, "CE2103", [])),
               curso_sel("MA2104", "CE", "ciencias_basicas",
                         "Calculo superior", 4, grupo(4, "MA2104", []))
           ]),
           Areas).
Areas = ["computacion", "ciencias_basicas"].
```

**Modo 2: área instanciada (`+`, `+`).** Comprueba si la combinación contiene cursos de un área concreta:

```prolog
?- pertenece_area("computacion", [
       curso_sel("CE2103", "CE", "computacion",
                 "AED II", 4, grupo(1, "CE2103", [])),
       curso_sel("MA2104", "CE", "ciencias_basicas",
                 "Calculo superior", 4, grupo(4, "MA2104", []))
   ]).
true.
```

Si el área no aparece, responde `false`. Con un área repetida (dos cursos de la misma área) el modo 1 devuelve esa área una vez por cada curso.

El mismo predicado se reutiliza en `contar_cursos_area/3` (modo `+`, `+`, dentro de `aggregate_all/3`) y en `incumple_limite_area/2` (modo `-`, `+`, para descubrir qué áreas hay), evitando duplicar la lógica de buscar cursos por área.

---

## 3. Formato de entrada y salida

### Entrada

La Etapa 3 recibe los JSON generados por Racket:

- `../Parte 2/data/salida_CE.json`
- `../Parte 2/data/salida_PI.json`

Cada combinación contiene sus créditos totales y la lista de cursos seleccionados; cada curso incluye código, carrera, nombre, créditos, área y grupo con sus horarios.

### Salida

Prolog genera `data/salida_CE.json` y `data/salida_PI.json`. Cada archivo contiene `total_combinaciones` y la lista `combinaciones`; cada resultado incluye:

| Campo | Descripción |
|---|---|
| `id` | Identificador de la combinación. |
| `creditos_totales` | Total de créditos de la combinación. |
| `cumple` | Booleano que indica si cumple las políticas. |
| `razon` | Código de la primera regla incumplida. |
| `detalle` | Descripción legible del resultado. |
| `cursos` | Lista de cursos con su grupo, créditos, carrera, área y horarios. |

Los valores posibles de `razon` son `ninguna`, `balance_carrera`, `limite_area` e `incompatibilidad`.

Ejemplo de una combinación rechazada (recortada):

```json
{
  "id": 1,
  "creditos_totales": 13,
  "cumple": false,
  "razon": "limite_area",
  "detalle": "Incumple limite de cursos en el area: ciencias_basicas",
  "cursos": [
    {
      "codigo": "FI1101",
      "grupo": 1,
      "codigo_opcion": "FI1101",
      "carrera": "PI",
      "nombre": "Física general I",
      "creditos": 3,
      "area": "ciencias_basicas",
      "horarios": [
        {"dia": "MAR", "inicio": "07:30", "fin": "09:20"}
      ]
    }
  ]
}
```

### Justificación del formato

- **JSON** mantiene la misma convención que las etapas 1 y 2, es un formato estándar y Java puede leer sus campos por nombre sin interpretar estructuras propias de Prolog.
- **`razon` como código y `detalle` como texto:** el código corto permite a Java comparar sin interpretar frases; el detalle es para mostrar al usuario e incluye los datos (la carrera, el área o los cursos involucrados).
- **Información agregada respecto al mínimo del enunciado** (pares curso-grupo, veredicto y razón): se incluyen carrera, nombre, créditos, área y horarios de cada curso. Se justifica porque la Etapa 4 debe presentar el horario al usuario y, sin estos datos, tendría que volver a leer el catálogo de la Etapa 1. También se agrega `creditos_totales` y `total_combinaciones`.
- El JSON se escribe con `json/1` de la librería estándar, que respeta el orden de los campos y escapa correctamente los caracteres especiales.

---

## 4. Ejecución del programa

### Requisitos

- SWI-Prolog instalado.
- Archivos JSON generados por la Etapa 2 (`racket main.rkt` en `Parte 2`).

### Ejecución

Dentro de la carpeta `Parte 3`:

```bash
swipl main.pl
```

El programa procesa ambas carreras y genera los archivos en `data/`. Muestra un resumen como el siguiente (los totales dependen de las preferencias usadas en la Etapa 2):

```text
CEmestre - Etapa 3 (Paradigma Logico)
CE
[1/2] Cargando combinaciones desde ../Parte 2/data/salida_CE.json...
[2/2] Evaluando combinaciones contra las reglas de politica...
      -> 5 combinaciones evaluadas, 5 cumplen las reglas de politica.
      -> Archivo generado en: data/salida_CE.json

PI
[1/2] Cargando combinaciones desde ../Parte 2/data/salida_PI.json...
[2/2] Evaluando combinaciones contra las reglas de politica...
      -> 132 combinaciones evaluadas, 110 cumplen las reglas de politica.
      -> Archivo generado en: data/salida_PI.json

Proceso finalizado para ambas carreras.
```

### Pruebas de las reglas

Las reglas tienen pruebas con datos escritos a mano, sin depender del JSON de Racket:

```bash
swipl reglas.pl
```

```text
--- combinacion que deberia cumplir ---
resultado(true,ninguna)
--- combinacion con 3 cursos de ciencias_basicas (max 2) ---
resultado(false,limite_area(ciencias_basicas))
--- combinacion con un par incompatible ---
resultado(false,incompatibilidad(PI2609,PI2610))
--- pertenece_area en modo (-,+): enumera el area de cada curso ---
[ciencias_basicas,ciencias_basicas,ciencias_basicas]
--- pertenece_area en modo (+,+): verifica una area concreta ---
si_tiene_computacion
```

### Cambiar los parámetros de política

Los parámetros están en `politicas.pl` y se pueden modificar sin tocar las reglas: `max_porcentaje_carrera/2`, `max_cursos_area/2` e `incompatible/2`. Después de editarlos basta volver a ejecutar `swipl main.pl` para ver el nuevo resultado de cada combinación.
