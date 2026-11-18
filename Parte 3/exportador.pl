:- module(exportador, [
    procesar_y_exportar/2,
    procesar_y_exportar/4
]).

:- use_module(library(http/json)).
:- use_module(lector).
:- use_module(reglas).

% Función: procesar_y_exportar
% Utiliza la ruta del archivo JSON de entrada (generado por Racket) y la ruta
% del archivo de salida. Genera un archivo JSON con la evaluación de todas las
% combinaciones. Si la carpeta de salida no existe, la crea.
% E: ruta de entrada (JSON de Racket) y ruta de salida (JSON de esta etapa).
% S: (ninguna; escribe el archivo de salida).
procesar_y_exportar(RutaEntrada, RutaSalida) :-
    procesar_y_exportar(RutaEntrada, RutaSalida, _, _).

% Función: procesar_y_exportar
% Igual que procesar_y_exportar/2, pero además devuelve cuántas combinaciones
% se evaluaron y cuántas cumplen las reglas, para mostrar un resumen.
% E: ruta de entrada y ruta de salida.
% S: Total (combinaciones evaluadas) y TotalCumplen (las que cumplen).
procesar_y_exportar(RutaEntrada, RutaSalida, Total, TotalCumplen) :-
    % Cargar las combinaciones
    cargar_combinaciones(RutaEntrada),

    % Recorrer todas las combinaciones mediante backtracking y recolectar resultados
    encontrar_todos_los_resultados(Resultados),

    % Escribir los resultados en el archivo de salida (JSON)
    guardar_resultados_json(RutaSalida, Resultados),

    length(Resultados, Total),
    aggregate_all(count, (member(json(Campos), Resultados), memberchk(cumple = @(true), Campos)), TotalCumplen).

% Función: encontrar_todos_los_resultados
% Recorre usando backtracking todas las combinaciones cargadas, evaluando cada
% una contra las reglas de política. Cada resultado lleva el veredicto (cumple),
% la razón (primera regla violada) y los cursos con su grupo.
% E: (ninguna).
% S: Resultados (lista de diccionarios, uno por combinación).
encontrar_todos_los_resultados(Resultados) :-
    findall(
        json([ id = Id,
               creditos_totales = Creditos,
               cumple = @(Cumple),
               razon = Codigo,
               detalle = Detalle,
               cursos = CursosJson ]),
        (
            combinacion_cargada(Id, Creditos, Cursos),
            cursos_a_json(Cursos, CursosJson),
            evaluar_combinacion(Cursos, Creditos, resultado(Cumple, Razon)),
            razon_a_texto(Razon, Codigo, Detalle)
        ),
        Resultados
    ).

% Función: cursos_a_json
% Convierte cada curso seleccionado de una combinación a un diccionario con
% el par curso-grupo y los datos extra que necesita la Etapa 4 (Java) para
% presentar el horario sin volver a leer el catálogo: carrera, nombre, créditos,
% área y los horarios del grupo elegido.
% E: Cursos (lista de curso_sel/6).
% S: lista de diccionarios.
cursos_a_json([], []).
cursos_a_json([curso_sel(Codigo, Carrera, Area, Nombre, Creditos, grupo(Numero, CodigoOpcion, Horarios)) | Resto],
              [json([ codigo = Codigo,
                      grupo = Numero,
                      codigo_opcion = CodigoOpcion,
                      carrera = Carrera,
                      nombre = Nombre,
                      creditos = Creditos,
                      area = Area,
                      horarios = HorariosJson ]) | RestoJson]) :-
    bloques_a_json(Horarios, HorariosJson),
    cursos_a_json(Resto, RestoJson).

% Función: bloques_a_json
% Convierte la lista de bloque(Dia, Inicio, Fin) de un grupo a diccionarios.
% E: lista de bloque/3.
% S: lista de diccionarios con dia, inicio y fin.
bloques_a_json([], []).
bloques_a_json([bloque(Dia, Inicio, Fin) | Resto],
               [json([dia = Dia, inicio = Inicio, fin = Fin]) | RestoJson]) :-
    bloques_a_json(Resto, RestoJson).

% Función: razon_a_texto
% Convierte el término de la razón en un código corto (para que Java pueda
% comparar sin interpretar texto) y un detalle legible con los datos.
% E: Razon (ninguna, balance_carrera/1, limite_area/1 o incompatibilidad/2).
% S: Codigo (átomo) y Detalle (string).
razon_a_texto(ninguna, ninguna, "Ninguna").
razon_a_texto(balance_carrera(Carrera), balance_carrera, Detalle) :-
    format(string(Detalle), "Incumple balance de la carrera: ~w", [Carrera]).
razon_a_texto(limite_area(Area), limite_area, Detalle) :-
    format(string(Detalle), "Incumple limite de cursos en el area: ~w", [Area]).
razon_a_texto(incompatibilidad(C1, C2), incompatibilidad, Detalle) :-
    format(string(Detalle), "Cursos incompatibles: ~w y ~w", [C1, C2]).

% Función: guardar_resultados_json
% Crea la carpeta de salida si hace falta y escribe la lista de resultados
% con codificación UTF-8, junto con el total de combinaciones.
% E: RutaSalida y Resultados (lista de diccionarios).
% S: (ninguna; escribe el archivo).
guardar_resultados_json(RutaSalida, Resultados) :-
    file_directory_name(RutaSalida, Carpeta),
    make_directory_path(Carpeta),
    length(Resultados, Total),
    setup_call_cleanup(
        open(RutaSalida, write, Puerto, [encoding(utf8)]),
        json_write(Puerto, json([total_combinaciones = Total, combinaciones = Resultados])),
        close(Puerto)
    ).
 