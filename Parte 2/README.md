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
├── constantes.rkt          Centralización de rutas de entrada/salida y constantes de sistema
├── tipos.rkt               Definición de structs inmutables y tipado estricto (Typed Racket)
├── lector.rkt              Carga e hidratación del catálogo JSON a structs
├── preferencias.rkt        Carga y parsing del archivo de preferencias de matrícula JSON
├── candidatos.rkt          Filtrado e intersección de cursos de interés habilitados
├── choques.rkt             Predicados de validación temporal y espacial de bloques
├── generador.rkt           Motor de generación perezosa de combinaciones mediante streams
├── filtros.rkt             Polimorfismo paramétrico, conversión de pares y filtro por créditos
├── procesador.rkt          Módulo orquestador del pipeline funcional principal
├── exportador.rkt          Serialización de estructuras a jsexpr y exportación a JSON
├── main.rkt                Punto de entrada principal del programa
└── data/
    ├── catalogo_CE.json     Catálogo procesado de Computadores (Salida de la Etapa 1)
    ├── catalogo_PI.json     Catálogo procesado de Producción Industrial (Salida de la Etapa 1)
    ├── preferencias_CE.json Preferencias de matrícula para Computadores
    ├── preferencias_PI.json Preferencias de matrícula para Producción Industrial
    ├── salida_CE.json       Combinaciones finales generadas para CE (Insumo para Etapa 3)
    └── salida_PI.json       Combinaciones finales generadas para PI (Insumo para Etapa 3)
```

---

### 2.2. Explicación de la conexión 

1. **Carga e Ingesta de Datos (`constantes.rkt`, `lector.rkt`, `preferencias.rkt`):**
   * `main.rkt` inicia el flujo leyendo las rutas definidas en `constantes.rkt` (`RUTA-CATALOGO-CE`, `RUTA-PREFERENCIAS-CE`, etc.).
   * Invoca a `cargar-catalogo` en `lector.rkt`, que usa `read-json` y mapea los datos hacia structs tipados de `Curso`, `Grupo` y `Bloque-Horario` definidos en `tipos.rkt`.
   * En paralelo, `cargar-preferencias` en `preferencias.rkt` deserializa el archivo JSON de configuración del estudiante a un struct `Preferencias`.

2. **Selección de Candidatos (`candidatos.rkt`):**
   * `procesador.rkt` toma el catálogo y las preferencias y ejecuta `candidatos-de-interes`.
   * Este módulo busca cada curso solicitado por el alumno, confirma que esté marcado como `matriculable = #t` y filtra sus grupos según los números de grupo solicitados. Devuelve una lista de pares `(Curso . listof Grupo)`.

3. **Generación Perezosa de Combinaciones y Control de Choques (`generador.rkt`, `choques.rkt`):**
   * La lista de candidatos ingresa a `generar-combinaciones-crudas` en `generador.rkt`.
   * Utilizando recursión e inmutabilidad, se explora la combinación de un grupo por cada curso.
   * Antes de incluir un grupo candidato, `compatible-con-elegidos?` llama a `grupos-chocan?` en `choques.rkt`, el cual verifica si algún bloque de horario se traslapa en el mismo día (`inicio_A < fin_B` e `inicio_B < fin_A`).
   * Gracias al uso de `stream-cons`, si una rama de combinación presenta un choque, se descarta inmediatamente sin construir ni evaluar el resto de esa rama (poda al vuelo).

4. **Transformación y Filtrado Paramétrico (`filtros.rkt`):**
   * El stream de listas de pares `(Curso . Grupo)` es recibido por `stream-pares->combinaciones` en `filtros.rkt`.
   * Por cada elemento, se calcula el total de créditos, horas y carreras de origen, empaquetándolos en una estructura `Combinacion`.
   * Posteriormente, `filtrar-por-creditos` utiliza la función genérica con polimorfismo paramétrico `cumple-criterios?` para evaluar que la suma de créditos esté dentro del rango `[creditos-min, creditos-max]`.

5. **Serialización y Salida (`exportador.rkt`, `main.rkt`):**
   * `procesador.rkt` entrega el stream filtrado a `main.rkt`.
   * `main.rkt` invoca `exportar-combinaciones-json` en `exportador.rkt`, convirtiendo las estructuras de Racket a expresiones hash (`jsexpr`) y guardando en disco el archivo JSON final (`salida_CE.json` o `salida_PI.json`).

---
