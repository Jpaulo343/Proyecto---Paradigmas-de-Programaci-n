:- use_module(constantes).
:- use_module(exportador).

% Función: procesar_carrera
% Corre la Etapa 3 para una carrera: lee las combinaciones que exportó Racket,
% las evalúa contra las reglas de política y escribe el resultado en JSON.
% E: carrera (string, "CE" o "PI").
% S: (ninguna; imprime el progreso y escribe el archivo de salida).
procesar_carrera(Carrera) :-
    ruta_entrada(Carrera, RutaEntrada),
    ruta_salida(Carrera, RutaSalida),
    format("~w~n", [Carrera]),
    format("[1/2] Cargando combinaciones desde ~w...~n", [RutaEntrada]),
    format("[2/2] Evaluando combinaciones contra las reglas de politica...~n", []),
    procesar_y_exportar(RutaEntrada, RutaSalida, Total, TotalCumplen),
    format("      -> ~w combinaciones evaluadas, ~w cumplen las reglas de politica.~n", [Total, TotalCumplen]),
    format("      -> Archivo generado en: ~w~n~n", [RutaSalida]).


main :-
    writeln('CEmestre - Etapa 3 (Paradigma Logico)'),
    procesar_carrera("CE"),
    procesar_carrera("PI"),
    writeln('Proceso finalizado para ambas carreras.').

:- initialization(main, main).