:- module(politicas, [
    max_porcentaje_carrera/2,
    limite_cursos_area/1,
    max_cursos_area/2,
    incompatible/2
]).

% Este módulo contiene los hechos de política académica que usan las
% reglas de validación. Los valores se pueden cambiar aquí directamente
% sin tocar la lógica de reglas.pl.


% Hecho: max_porcentaje_carrera
% Define el porcentaje máximo de los créditos totales de una combinación
% que pueden pertenecer a una misma carrera.
% E: carrera (string, "CE" o "PI") y porcentaje máximo (entero de 0 a 100).
max_porcentaje_carrera("CE", 100).
max_porcentaje_carrera("PI", 100).


% Hechos: limite_cursos_area y max_cursos_area
% Definen el máximo N de cursos de una misma área que puede llevar una
% combinación (tanto el valor general N como por nombre de área).
% E: área (string) y cantidad máxima de cursos permitidos (entero).
limite_cursos_area(2).

max_cursos_area("X", 2).
max_cursos_area("ciencias_basicas", 2).
max_cursos_area("computacion", 4).
max_cursos_area("electronica", 2).
max_cursos_area("industrial_gestion", 4).
max_cursos_area("humanidades_complementarias", 2).
max_cursos_area("administracion", 2).



% Hecho: incompatible
% Declara pares de cursos que no se pueden llevar juntos en la misma
% combinación. Solo se ponen en una dirección porque la simetría se
% resuelve en reglas.pl para no enciclar a Prolog.
% E: codigo1 y codigo2 (strings con los códigos de cada par incompatible).
incompatible("PI2609", "PI2610").
incompatible("CE1101", "CA2125").
incompatible("CE3101", "PI3403").
