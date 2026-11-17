:- module(exportador, [
    procesar_y_exportar/2
]).

:- use_module(library(http/json)).
:- use_module(lector).
:- use_module(reglas).

% Utiliza la ruta del archivo JSON de entrada (generado por Racket) y Ruta del archivo de salida.
% Genera un archivo JSON con la evaluación de todas las combinaciones.
procesar_y_exportar(RutaEntrada, RutaSalida) :-
    % Cargar las combinaciones
    lector:cargar_combinaciones(RutaEntrada),
    
    % Recorrer todas las combinaciones mediante backtracking y recolectar resultados
    encontrar_todos_los_resultados(Resultados),
    
    % Escribir los resultados en el archivo de salida (JSON)
    guardar_resultados_json(RutaSalida, Resultados).


% Recorre usando backtracking todas las combinaciones cargadas evaluando cada una contra las reglas de política.
encontrar_todos_los_resultados(Resultados) :-
    findall(
        json([
            id = Id,
            creditos_totales = Creditos,
            cursos = CursosJson,
            cumple = Cumple,
            razon = TermRazon
        ]),
        (
            lector:combinacion_cargada(Id, Creditos, Cursos),
            cursos_a_json(Cursos, CursosJson),
            reglas:evaluar_combinacion(Cursos, Creditos, resultado(Cumple, RazonObj)),
            razon_a_string(RazonObj, TermRazon)
        ),
        Resultados
    ).

% Extrae los pares curso-grupo para la parte con Java
cursos_a_json([], []).
cursos_a_json([curso_sel(Codigo, _, _, _, _, grupo(Numero, _, _)) | Resto],
              [json([codigo = Codigo, grupo = Numero]) | RestoJson]) :-
    cursos_a_json(Resto, RestoJson).


% Convierte el término compuesto de la razón en un formato legible
razon_a_string(ninguna, "Ninguna").
razon_a_string(balance_carrera(Carrera), Texto) :-
    format(string(Texto), "Incumple balance de la carrera: ~w", [Carrera]),
    !.
razon_a_string(limite_area(Area), Texto) :-
    format(string(Texto), "Incumple limite de cursos en el area: ~w", [Area]),
    !.
razon_a_string(incompatibilidad(C1, C2), Texto) :-
    format(string(Texto), "Cursos incompatibles: ~w y ~w", [C1, C2]).


% Abre el archivo de salida con UTF-8 y escribe la lista de resultados.
guardar_resultados_json(RutaSalida, Resultados) :-
    setup_call_cleanup(
        open(RutaSalida, write, Puerto, [encoding(utf8)]),
        json_write_dict(Puerto, json([combinaciones=Resultados])),
        close(Puerto)
    ).