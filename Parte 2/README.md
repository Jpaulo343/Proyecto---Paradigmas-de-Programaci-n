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