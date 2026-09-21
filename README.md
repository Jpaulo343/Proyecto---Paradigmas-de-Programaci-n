## 2.1. Arquitectura del proyecto 

## 2.2. Decisiones de diseño 

### 2.2.1. Justificación de decisiones propias y específicas de su dataset. Deben dar ejemplos concretos con los planes de estudio. 
    
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

    
  
## 2.3. Estructuras de datos desarrolladas 
