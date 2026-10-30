# CEmestre — constructor de horarios

Instituto Tecnológico de Costa Rica · Escuela de Ingeniería en Computadores
Paradigmas de Programación (CE1106) · II Semestre 2026

**Integrantes**
- Jean Paulo Poveda Chaves
- Josafat Solano Quirós
- Camila Navarro Valverde

---

CEmestre es un sistema que ayuda a un estudiante a construir su horario de matrícula, tomando en cuenta los cursos de su plan de estudios, validando requisitos, correquisitos y choques de horario. Se construye en 4 etapas, una por cada paradigma visto en el curso. Cada etapa lee lo que exportó la anterior y le agrega valor; el sistema **no** es un único ejecutable.

| Etapa | Lenguaje | Qué hace | Documentación |
|---|---|---|---|
| 1 | C (imperativo) | Construye el catálogo de cursos y calcula, para cada grupo, choques de horario y elegibilidad por requisitos/correquisitos | [Parte 1/README.md](Parte%201/README.md) |
| 2 | Racket (funcional) | Genera y filtra, con streams, las combinaciones válidas de horario a partir del catálogo de la Etapa 1 | [Parte 2/README.md](Parte%202/README.md) |
| 3 | Prolog (lógico) | Aplica reglas de restricción (créditos máximos, compatibilidades) sobre las combinaciones generadas | pendiente |
| 4 | Java (orientado a objetos) | Integra el resultado final y lo presenta al usuario | pendiente |

Cada carpeta (`Parte 1/`, `Parte 2/`...) tiene su propio README con la arquitectura, las decisiones de diseño y las estructuras de datos de esa etapa.
