# CEmestre — Etapa 3: Paradigma Lógico (Prolog)

[← Volver al índice del proyecto](../README.md)

## 1. Descripción general

La Etapa 3 del proyecto CEmestre corresponde al paradigma lógico, implementado mediante Prolog.

El objetivo de esta etapa es evaluar las combinaciones de horarios generadas por la Etapa 2 (Racket), aplicando un conjunto de reglas de política académica para determinar si cada combinación es válida.

1. Balance entre carreras: verifica que ninguna carrera supere el porcentaje máximo de créditos establecido.
2. Límite por área: comprueba que una combinación no exceda la cantidad máxima de cursos permitidos en una misma área académica.
3. Incompatibilidad: verifica que una combinación no contenga simultáneamente dos cursos declarados incompatibles.

Cuando una combinación incumple alguna regla, el programa registra la primera regla que viola y genera una descripción del motivo. Además esta etapa no genera nuevas combinaciones. Su responsabilidad es evaluar las combinaciones recibidas y exportar los resultados para que puedan ser utilizados por la Etapa 4 (Java).

---

## 2.1. Arquitectura del proyecto

### Organización de archivos

La carpeta de la Parte 3 tiene la siguiente estructura:

```text
Parte 3/
├── constantes.pl
├── lector.pl
├── reglas.pl
├── politicas.pl
├── exportador.pl
├── main.pl
└── data/
    ├── salida_CE.json
    └── salida_PI.json
```

### Responsabilidad de cada archivo

- **`constantes.pl`:** define las rutas de entrada y salida para cada carrera. Permite identificar los archivos JSON generados por Racket y los archivos donde se guardarán los resultados de Prolog.

- **`lector.pl`:** se encarga de leer los archivos JSON producidos por la Etapa 2. Convierte los diccionarios JSON en términos compuestos de Prolog y registra las combinaciones como hechos `combinacion_cargada/3`. Este módulo utiliza la biblioteca `library(http/json)`.

- **`politicas.pl`:** contiene los hechos que representan las políticas académicas. Aquí se establecen los porcentajes máximos por carrera, los límites de cursos por área y los pares de cursos incompatibles. Los valores pueden modificarse sin necesidad de cambiar las reglas de validación.

- **`reglas.pl`:** implementa la lógica de evaluación de las combinaciones. Contiene los predicados encargados de calcular los créditos por carrera, identificar las áreas, contar cursos y comprobar las tres restricciones académicas. También determina cuál es la primera regla incumplida.

- **`exportador.pl`:** conecta la carga de datos con la evaluación y la generación de resultados. Recorre todas las combinaciones mediante backtracking, convierte los resultados a diccionarios JSON y escribe el archivo de salida.

- **`main.pl`:** contiene el punto de entrada del programa. Ejecuta el procesamiento para las carreras CE y PI, mostrando en consola la cantidad de combinaciones evaluadas y cuántas cumplen las políticas.

- **`data/`:** almacena los archivos JSON de entrada y salida. Los archivos de entrada provienen de Racket, mientras que los de salida contienen los resultados de la evaluación realizada en Prolog.

### Cómo se conectan los módulos durante la ejecución

El funcionamiento de la Etapa 3 sigue un flujo secuencial:

1. **Inicio del programa (`main.pl`):** se ejecuta el predicado `main/0`, que procesa primero la carrera CE y después la carrera PI.

2. **Obtención de las rutas (`constantes.pl`):** mediante `ruta_entrada/2` y `ruta_salida/2`, se determinan los archivos que corresponden a cada carrera.

3. **Lectura de combinaciones (`lector.pl`):** se abre el JSON generado por Racket y se convierten sus datos en términos compuestos:
   - `combinacion/3`: representa una combinación, su identificador, sus créditos totales y sus cursos.
   - `curso_sel/6`: representa un curso seleccionado con su código, carrera, área, nombre, créditos y grupo.
   - `grupo/3`: contiene el número de grupo, el código de opción y sus horarios.
   - `bloque/3`: representa cada bloque horario mediante el día, la hora de inicio y la hora de finalización.

   Finalmente, cada combinación se registra como un hecho `combinacion_cargada/3` para poder recorrerlas posteriormente.

4. **Evaluación de políticas (`reglas.pl` y `politicas.pl`):** cada combinación se compara con los hechos definidos en `politicas.pl`. Se ejecutan las reglas en el siguiente orden:
   1. Balance entre carreras.
   2. Límite por área.
   3. Incompatibilidad.

   Si se encuentra una violación, se registra esa razón y no se continúan evaluando las siguientes reglas para esa combinación.

5. **Generación de resultados (`exportador.pl`):** se recorren todas las combinaciones cargadas mediante `findall/3`, aprovechando el backtracking de Prolog. Cada resultado se transforma a una estructura JSON que incluye los cursos, grupos, veredicto y razón.

6. **Finalización (`main.pl`):** se guarda el archivo JSON correspondiente a cada carrera y se muestra un resumen de las combinaciones procesadas.

### Flujo general de información

```text
       ETAPA 2 (RACKET)
               |
               v
       salida_CE.json
       salida_PI.json
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

Se decidió representar cada combinación mediante el término:

```prolog
combinacion(Id, CreditosTotales, Cursos).
```

Donde:

- `Id`: identificador de la combinación.
- `CreditosTotales`: cantidad total de créditos.
- `Cursos`: lista de cursos seleccionados.

Cada curso se representa mediante:

```prolog
curso_sel(Codigo, Carrera, Area, Nombre, Creditos, Grupo).
```

Esta estructura permite acceder directamente a la información necesaria para evaluar las políticas sin tener que consultar nuevamente el catálogo de la Etapa 1.

Además, el grupo se representa mediante:

```prolog
grupo(Numero, CodigoOpcion, Horarios).
```

Y cada bloque horario mediante:

```prolog
bloque(Dia, Inicio, Fin).
```

Se eligieron términos compuestos porque permiten organizar la información relacionada y acceder a sus campos mediante unificación y patrones de Prolog.

**2. Asignación de áreas académicas.**

Se incorporó el campo `area` a cada curso para poder aplicar la restricción de cantidad máxima de cursos por área.

Las áreas utilizadas en el proyecto son:

| Área | Ejemplos de cursos |
|---|---|
| `ciencias_basicas` | MA1102, FI1101, QU1106 |
| `computacion` | CE2103 |
| `electronica` | Cursos del área de electrónica |
| `industrial_gestion` | Cursos del área de producción industrial |
| `humanidades_complementarias` | CS2101 |
| `administracion` | Cursos del área de administración |
| `X` | Comodín |

La asignación se realiza desde los datos del catálogo de cursos y se mantiene durante el procesamiento de Racket hasta llegar a Prolog.

Por ejemplo, en la carrera de Ingeniería en Computadores:

- `CE2103` (Algoritmos y Estructuras de Datos II) pertenece a `computacion`.
- `MA2104` (Cálculo Superior) pertenece a `ciencias_basicas`.
- `CS2101` (Ambiente Humano) pertenece a `humanidades_complementarias`.

Esto permite que Prolog identifique los cursos que pertenecen a una misma área y contabilice cuántos aparecen en cada combinación.

**3. Declaración de cursos incompatibles.**

Los pares incompatibles se representan mediante hechos `incompatible/2` dentro de `politicas.pl`.

Actualmente se definieron los siguientes pares:

```prolog
incompatible("PI2609", "PI2610").
incompatible("CE1101", "CA2125").
incompatible("CE3101", "PI3403").
```

Se decidió almacenar cada par una sola vez, sin duplicar la relación en sentido contrario.

La simetría se resuelve directamente en `reglas.pl`, comprobando ambas posibilidades:

```prolog
( incompatible(Codigo1, Codigo2)
; incompatible(Codigo2, Codigo1)
).
```

De esta forma, no importa el orden en que aparezcan los cursos dentro de la combinación: si ambos pertenecen a un par incompatible, la regla detecta la violación.

Para establecer los pares de cursos incompatibles, se realizó una revisión de los planes de estudio de Ingeniería en Computadores e Ingeniería en Producción Industrial. Se decidió identificar cursos que presentan similitudes en sus contenidos o que pertenecen a campos académicos relacionados, con el propósito de evitar que determinadas combinaciones incluyan cursos con posibles equivalencias o superposiciones temáticas. Estas incompatibilidades fueron definidas como parte de las decisiones de diseño del grupo y se representan mediante hechos incompatible/2 en el archivo `politicas.pl`. 

Se decidió declarar cada par una sola vez y manejar la simetría desde `reglas.pl`, evitando duplicar información y permitiendo detectar la incompatibilidad independientemente del orden en que aparezcan los cursos en una combinación.

**4. Orden de evaluación de las reglas.**

Se estableció un orden de prioridad para determinar la primera razón de rechazo de una combinación:

1. Balance entre carreras.
2. Límite por área.
3. Incompatibilidad.

Esto se implementa mediante condicionales encadenados (`->` y `;`) en el predicado `evaluar_combinacion/3`.

Por ejemplo, se puede considerar una combinación que incluya los cursos `PI2609`, `PI2610` y `PI5516`, suponiendo que sus áreas sean `industrial_gestion` y que los parámetros de política se mantengan como están definidos.

En este caso:

- Los tres cursos pertenecen a `industrial_gestion`, cuyo máximo es de dos cursos.
- `PI2609` y `PI2610` forman un par declarado incompatible.
- El balance entre carreras no se incumple con los porcentajes actuales del 100 %.

Como el límite por área se revisa antes que la incompatibilidad, Prolog reporta:

```prolog
resultado(false, limite_area("industrial_gestion"))
```

Si se cambiara el orden de evaluación y se revisara primero la incompatibilidad, la razón reportada sería:

```prolog
resultado(false, incompatibilidad("PI2609", "PI2610"))
```

Ambas restricciones se incumplen, pero el resultado cambia porque el programa solamente devuelve la primera regla violada.

### 2.2.2. Caso límite real y cómo se resolvió

**El caso: una combinación que alcanza exactamente el máximo permitido de cursos en un área.**

Una situación importante es cuando una combinación tiene exactamente la cantidad máxima de cursos permitidos en una determinada área.

Por ejemplo, si el máximo de `ciencias_basicas` es de dos cursos y una combinación contiene:

- `MA1102` (Cálculo).
- `FI1101` (Física I).

La combinación tiene exactamente dos cursos de esa área.

La condición implementada en `incumple_limite_area/2` es:

```prolog
Cantidad > Maximo
```

Se utiliza el operador `>` en lugar de `>=`, porque la política establece que una combinación incumple únicamente cuando supera el límite, no cuando lo alcanza.

Por lo tanto:

- Dos cursos: cumple el límite.
- Tres cursos: incumple el límite.

Esta decisión evita rechazar combinaciones que utilizan correctamente toda la cantidad permitida de cursos en un área.

---

## 2.3. Representación de datos y predicados principales

### Predicados principales

| Predicado | Funcionalidad |
|---|---|
| `cargar_combinaciones/1` | Lee el JSON de entrada y carga las combinaciones como hechos. |
| `dict_a_curso_sel/2` | Convierte un diccionario de curso a un término compuesto. |
| `creditos_por_carrera/3` | Calcula los créditos pertenecientes a una carrera. |
| `pertenece_area/2` | Identifica los cursos que pertenecen a un área. |
| `contar_cursos_area/3` | Cuenta los cursos que pertenecen a una misma área. |
| `incumple_balance_carreras/3` | Comprueba si alguna carrera supera su porcentaje máximo. |
| `incumple_limite_area/2` | Comprueba si alguna área supera su cantidad máxima de cursos. |
| `incumple_incompatibilidad/3` | Identifica pares de cursos incompatibles. |
| `evaluar_combinacion/3` | Aplica las reglas en orden y determina el resultado. |
| `procesar_y_exportar/4` | Carga, evalúa y exporta todas las combinaciones. |

### Predicado utilizado en más de un modo de consulta

Para cumplir este requerimiento se implementó `pertenece_area/2`.

Este predicado permite identificar si un área aparece dentro de una combinación y también permite enumerar las áreas de los cursos cuando la variable `Area` no está instanciada.

Su implementación es:

```prolog
pertenece_area(Area, Cursos) :-
    member(curso_sel(_, _, Area, _, _, _), Cursos).
```

Se puede utilizar en dos modos diferentes:

**Modo 1: variable en el área (`-`, `+`).**

Se utiliza para obtener las áreas de los cursos presentes en una combinación.

```prolog
?- findall(Area,
           pertenece_area(Area, [
               curso_sel("CE2103", "CE", "computacion",
                         "AED II", 4, grupo(1, "CE2103", [])),
               curso_sel("MA2104", "CE", "ciencias_basicas",
                         "Calculo superior", 4, grupo(4, "MA2104", []))
           ]),
           Areas).
```

Resultado:

```prolog
Areas = ["computacion", "ciencias_basicas"].
```

**Modo 2: área instanciada (`+`, `+`).**

Se utiliza para comprobar si una combinación contiene cursos de un área específica.

```prolog
?- pertenece_area("computacion", [
       curso_sel("CE2103", "CE", "computacion",
                 "AED II", 4, grupo(1, "CE2103", [])),
       curso_sel("MA2104", "CE", "ciencias_basicas",
                 "Calculo superior", 4, grupo(4, "MA2104", []))
   ]).
```

Resultado:

```prolog
true.
```

Si se consulta un área que no aparece en la combinación, el predicado devuelve `false`.

La ventaja de este diseño es que el mismo predicado se reutiliza dentro de `contar_cursos_area/3` y `incumple_limite_area/2`, evitando duplicar la lógica para buscar cursos por área.

---

## 3. Formato de entrada y salida

### Entrada

La Etapa 3 recibe los archivos JSON generados por Racket:

- `../Parte 2/data/salida_CE.json`
- `../Parte 2/data/salida_PI.json`

Cada combinación contiene sus créditos totales y una lista de cursos seleccionados. Cada curso incluye su código, carrera, nombre, créditos, área, grupo y horarios.

### Salida

Prolog genera:

- `data/salida_CE.json`
- `data/salida_PI.json`

Cada archivo contiene el total de combinaciones evaluadas y una lista de resultados.

Cada resultado incluye:

| Campo | Descripción |
|---|---|
| `id` | Identificador de la combinación. |
| `creditos_totales` | Total de créditos de la combinación. |
| `cumple` | Valor booleano que indica si cumple las políticas. |
| `razon` | Código de la primera regla incumplida. |
| `detalle` | Descripción legible del resultado. |
| `cursos` | Lista de cursos seleccionados con sus grupos y horarios. |

Los códigos posibles para el campo `razon` son:

- `ninguna`
- `balance_carrera`
- `limite_area`
- `incompatibilidad`

Se eligió JSON porque permite mantener una estructura organizada y facilita la comunicación entre las diferentes etapas del proyecto. Además, Java puede acceder directamente a los campos sin tener que interpretar estructuras específicas de Prolog.

---

## 4. Ejecución del programa

### Requisitos

- SWI-Prolog instalado.
- Archivos JSON generados por la Etapa 2.

### Ejecución

Dentro de la carpeta `Parte 3` :

```bash
swipl -s main.pl
```

El programa procesa ambas carreras y genera los archivos de resultados en la carpeta `data/`.

Durante la ejecución se muestra un resumen similar al siguiente:

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

El resultado se exporta en archivos JSON que contienen tanto el veredicto como la razón de rechazo y la información de los cursos seleccionados.
