:- module(lector, [
    combinacion_cargada/3,
    dict_a_bloque/2,
    dict_a_curso_sel/2,
    dict_a_combinacion/3,
    cargar_combinaciones/1,
    leer_combinaciones_json/2
]).

:- use_module(library(http/json)).

% Este módulo lee el archivo JSON exportado por Racket y convierte cada
% combinación y sus cursos a términos compuestos y hechos de Prolog.

% Para cargar las combinaciones de entrada, se debe llamar a cargar_combinaciones/1
% con la ruta del archivo JSON generado por Racket.

% Se declara dinámico solo para cargar las combinaciones de entrada desde
% el archivo JSON (permitido por el enunciado para datos de entrada).
:- dynamic combinacion_cargada/3.



% Función: cargar_combinaciones
% Limpia cualquier carga previa, lee el archivo JSON indicado y deja todas
% las combinaciones cargadas como hechos combinacion_cargada(Id, Creditos, Cursos).
% E: ruta del archivo JSON generado por Racket.
% S: (ninguna; deja los hechos listos para consultar).
cargar_combinaciones(Ruta) :-
    retractall(combinacion_cargada(_, _, _)),
    leer_combinaciones_json(Ruta, Combinaciones),
    guardar_hechos_combinaciones(Combinaciones).


% Función: leer_combinaciones_json
% Abre el archivo JSON de Racket, lo lee como diccionario y devuelve la
% lista de combinaciones ya convertidas a términos compuestos.
% E: ruta (string o átomo con la ruta del archivo salida_XX.json).
% S: Lista de términos combinacion(Id, CreditosTotales, Cursos).
leer_combinaciones_json(Ruta, Combinaciones) :-
    setup_call_cleanup(
        open(Ruta, read, Puerto, [encoding(utf8)]),
        json_read_dict(Puerto, Datos),
        close(Puerto)
    ),
    dicts_a_combinaciones(Datos.combinaciones, 1, Combinaciones).




% Función: dict_a_bloque
% Toma un diccionario de un bloque de horario (del JSON) y arma el término
% compuesto bloque(Dia, Inicio, Fin).
% E: b (diccionario con dia, inicio y fin).
% S: Término compuesto bloque/3 unificado en el segundo argumento.
dict_a_bloque(B, bloque(B.dia, B.inicio, B.fin)).


% Función: lista_dicts_a_bloques
% Convierte recursivamente la lista de diccionarios de horario de un grupo
% en una lista de términos bloque/3.
% E: lista de diccionarios de horario.
% S: Lista de términos bloque(Dia, Inicio, Fin).
lista_dicts_a_bloques([], []).
lista_dicts_a_bloques([B | Resto], [Bloque | RestoBloques]) :-
    dict_a_bloque(B, Bloque),
    lista_dicts_a_bloques(Resto, RestoBloques).


% Función: dict_a_curso_sel
% Toma el diccionario de un curso seleccionado dentro de una combinación y
% construye el término compuesto curso_sel con los campos que ocupan las
% reglas de Prolog y la exportación final.
% E: c (diccionario con codigo, carrera, area, nombre, creditos y grupo).
% S: Término curso_sel(Codigo, Carrera, Area, Nombre, Creditos, GrupoTerms).
dict_a_curso_sel(C, curso_sel(C.codigo, C.carrera, C.area, C.nombre, C.creditos,
                              grupo(G.numero, G.codigo_opcion, Horarios))) :-
    G = C.grupo,
    lista_dicts_a_bloques(G.horarios, Horarios).


% Función: lista_dicts_a_cursos
% Recorre la lista "seleccionados" de una combinación y transforma cada
% diccionario en un término curso_sel/6.
% E: lista de diccionarios de cursos seleccionados.
% S: Lista de términos curso_sel/6.
lista_dicts_a_cursos([], []).
lista_dicts_a_cursos([C | Resto], [CursoSel | RestoCursos]) :-
    dict_a_curso_sel(C, CursoSel),
    lista_dicts_a_cursos(Resto, RestoCursos).


% Función: dict_a_combinacion
% Convierte el diccionario de una combinación a un término compuesto
% combinacion(Id, CreditosTotales, CursosSeleccionados).
% E: id (entero con el número de combinación) y combo (diccionario del JSON).
% S: Término combinacion/3 con sus créditos totales y su lista de curso_sel.
dict_a_combinacion(Id, Combo, combinacion(Id, Combo.creditos_totales, Cursos)) :-
    lista_dicts_a_cursos(Combo.seleccionados, Cursos).


% Función: dicts_a_combinaciones
% Recorre la lista de combinaciones del JSON numerándolas desde 1.
% E: lista de diccionarios de combinaciones y contador actual Id.
% S: Lista de términos combinacion(Id, CreditosTotales, Cursos).
dicts_a_combinaciones([], _, []).
dicts_a_combinaciones([ComboDict | Resto], Id, [Combo | RestoCombos]) :-
    dict_a_combinacion(Id, ComboDict, Combo),
    SiguienteId is Id + 1,
    dicts_a_combinaciones(Resto, SiguienteId, RestoCombos).




% Función: guardar_hechos_combinaciones
% Registra cada combinación leída como un hecho combinacion_cargada/3 en la
% base de datos para poder recorrerlas luego por backtracking.
% E: lista de términos combinacion/3.
% S: (ninguna; carga los hechos de entrada).
guardar_hechos_combinaciones([]).
guardar_hechos_combinaciones([combinacion(Id, Creditos, Cursos) | Resto]) :-
    assertz(combinacion_cargada(Id, Creditos, Cursos)),
    guardar_hechos_combinaciones(Resto).



