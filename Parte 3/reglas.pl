:- module(reglas, [
    creditos_por_carrera/3,
    pertenece_area/2,
    contar_cursos_area/3,
    incumple_balance_carreras/3,
    incumple_limite_area/2,
    incumple_incompatibilidad/3,
    evaluar_combinacion/3
]).

:- use_module(politicas).
:- use_module(library(lists)).

% Este módulo contiene las reglas de política académica (balance entre
% carreras, límite de cursos por área, incompatibilidad) y la razón de
% rechazo de una combinación. 
% Un curso seleccionado llega desde lector.pl como el término
% curso_sel(Codigo, Carrera, Area, Nombre, Creditos, Grupo).

% Función: creditos_por_carrera
% Suma los créditos de los cursos de una combinación que pertenecen a una
% carrera dada, usando un acumulador.
% E: cursos (lista de curso_sel/6) y carrera (string, "CE" o "PI").
% S: total (créditos acumulados de esa carrera).
creditos_por_carrera(Cursos, Carrera, Total) :-
    creditos_por_carrera_acc(Cursos, Carrera, 0, Total).

creditos_por_carrera_acc([], _, Acumulado, Acumulado).
creditos_por_carrera_acc([curso_sel(_, Carrera, _, _, Creditos, _) | Resto], Carrera, Acumulado, Total) :-
    !,
    Acumulado1 is Acumulado + Creditos,
    creditos_por_carrera_acc(Resto, Carrera, Acumulado1, Total).
creditos_por_carrera_acc([_ | Resto], Carrera, Acumulado, Total) :-
    creditos_por_carrera_acc(Resto, Carrera, Acumulado, Total).

% Función: carreras_presentes
% Devuelve, sin repetir, las carreras de origen de los cursos de una combinación.
% E: cursos (lista de curso_sel/6).
% S: carreras (lista de strings sin duplicados).
carreras_presentes(Cursos, Carreras) :-
    findall(Carrera, member(curso_sel(_, Carrera, _, _, _, _), Cursos), ConDuplicados),
    list_to_set(ConDuplicados, Carreras).

% Función: pertenece_area
% Es el predicado usable en más de un modo. Dice que
% "area" es el área de al menos uno de los cursos de la combinación, igual
% que el pertenece(X, Lista) de la lección de listas, pero mirando el campo
% area de cada curso_sel en vez de comparar el elemento completo.
% E: area (string o variable) y cursos (lista de curso_sel/6).
% S: (es una relación; tiene éxito una vez por cada curso cuya area coincide).
pertenece_area(Area, Cursos) :-
    member(curso_sel(_, _, Area, _, _, _), Cursos).

% Función: contar_cursos_area
% Cuenta cuántos cursos de una combinación pertenecen a un área dada,
% reutilizando pertenece_area
% E: cursos (lista de curso_sel/6) y area (string, ya instanciada).
% S: cantidad (entero con la cantidad de cursos de esa área).
contar_cursos_area(Cursos, Area, Cantidad) :-
    aggregate_all(count, pertenece_area(Area, Cursos), Cantidad).

% Función: incumple_balance_carreras
% Regla de política #1. Una combinación incumple el balance si alguna de
% sus carreras supera el porcentaje máximo de créditos definido en
% politicas.pl, calculado sobre el total de créditos de la combinación.
% Se corta con "!" en la primera carrera que lo incumple, porque solo
% hace falta reportar una razón de rechazo, no todas las que fallan.
% E: cursos (lista de curso_sel/6) y creditos-totales (entero).
% S: carrera (la primera carrera, en el orden de la lista, que excede su máximo).
incumple_balance_carreras(Cursos, CreditosTotales, Carrera) :-
    carreras_presentes(Cursos, Carreras),
    member(Carrera, Carreras),
    creditos_por_carrera(Cursos, Carrera, CreditosCarrera),
    max_porcentaje_carrera(Carrera, MaxPorcentaje),
    CreditosCarrera * 100 > MaxPorcentaje * CreditosTotales,
    !.

% Función: incumple_limite_area
% Regla de política #2. Una combinación incumple el límite por área si
% alguna área tiene más cursos que el máximo N definido en politicas.pl.
% Usa pertenece_area para descubrir, curso por curso, qué áreas hay en la
% combinación, y contar_cursos_area para saber cuántos cursos tiene esa
% área en total.
% E: cursos (lista de curso_sel/6).
% S: area (la primera área, por backtracking, que excede su máximo de cursos).
incumple_limite_area(Cursos, Area) :-
    pertenece_area(Area, Cursos),
    contar_cursos_area(Cursos, Area, Cantidad),
    max_cursos_area(Area, Maximo),
    Cantidad > Maximo,
    !.

% Función: incumple_incompatibilidad
% Regla de política #3. Una combinación incumple si incluye los dos cursos
% de un par declarado incompatible en politicas.pl. La relación es simétrica:
% politicas.pl solo declara un sentido de cada par, así que aquí se prueban
% los dos sentidos con ";" en vez de duplicar los hechos.
% E: cursos (lista de curso_sel/6).
% S: codigo1 y codigo2 (el primer par incompatible encontrado en la combinación).
incumple_incompatibilidad(Cursos, Codigo1, Codigo2) :-
    member(curso_sel(Codigo1, _, _, _, _, _), Cursos),
    member(curso_sel(Codigo2, _, _, _, _, _), Cursos),
    Codigo1 \= Codigo2,
    ( incompatible(Codigo1, Codigo2) ; incompatible(Codigo2, Codigo1) ),
    !.

% Función: evaluar_combinacion
% Evalúa una combinación contra las tres reglas, en orden, y arma la razón
% de rechazo con la primera regla que incumple. El "->" encadenado es la
% implementación de corte + negación por falla: si incumple_balance_carreras
% tiene éxito, Prolog se compromete a esa rama (equivale a un corte) y nunca
% intenta las reglas siguientes; si falla, sigue de largo (negación por
% falla) a la próxima regla. El ORDEN de las reglas aquí es el orden en que
% se revisan: una combinación que incumple balance Y área siempre reporta
% balance primero, porque esa regla se revisa primero
% E: cursos (lista de curso_sel/6) y creditos-totales (entero).
% S: cumple (true/false) y razon (ninguna, o un término que identifica
%    cuál regla y con qué datos se incumplió).
evaluar_combinacion(Cursos, CreditosTotales, resultado(Cumple, Razon)) :-
    ( incumple_balance_carreras(Cursos, CreditosTotales, Carrera) ->
        Cumple = false, Razon = balance_carrera(Carrera)
    ; incumple_limite_area(Cursos, Area) ->
        Cumple = false, Razon = limite_area(Area)
    ; incumple_incompatibilidad(Cursos, Codigo1, Codigo2) ->
        Cumple = false, Razon = incompatibilidad(Codigo1, Codigo2)
    ; Cumple = true, Razon = ninguna
    ).


% Pruebas (sin depender del JSON de Racket). 
:- initialization(main, main).

main :-
    CursosOk = [
        curso_sel("CE2103", "CE", "computacion", "AED II", 4, grupo(1, "CE2103", [])),
        curso_sel("MA2104", "CE", "ciencias_basicas", "Calculo superior", 4, grupo(4, "MA2104", []))
    ],
    writeln('--- combinacion que deberia cumplir ---'),
    evaluar_combinacion(CursosOk, 8, R1),
    writeln(R1),

    CursosExcesoArea = [
        curso_sel("MA1102", "CE", "ciencias_basicas", "Calculo", 4, grupo(1, "MA1102", [])),
        curso_sel("FI1101", "CE", "ciencias_basicas", "Fisica I", 3, grupo(1, "FI1101", [])),
        curso_sel("QU1106", "CE", "ciencias_basicas", "Quimica", 3, grupo(1, "QU1106", []))
    ],
    writeln('--- combinacion con 3 cursos de ciencias_basicas (max 2) ---'),
    evaluar_combinacion(CursosExcesoArea, 10, R2),
    writeln(R2),

    CursosIncompatibles = [
        curso_sel("PI2609", "PI", "ciencias_basicas", "Probabilidad", 2, grupo(1, "PI2609", [])),
        curso_sel("PI2610", "PI", "ciencias_basicas", "Probabilidad II", 2, grupo(1, "PI2610", []))
    ],
    writeln('--- combinacion con un par incompatible ---'),
    evaluar_combinacion(CursosIncompatibles, 4, R3),
    writeln(R3),

    writeln('--- pertenece_area en modo (-,+): enumera el area de cada curso ---'),
    findall(Area, pertenece_area(Area, CursosExcesoArea), Areas),
    writeln(Areas),

    writeln('--- pertenece_area en modo (+,+): verifica una area concreta ---'),
    ( pertenece_area("computacion", CursosOk) -> writeln(si_tiene_computacion) ; writeln(no_tiene_computacion) ).
