:- module(constantes, [
    ruta_entrada/2,
    ruta_salida/2
]).

% Rutas de entrada y salida por carrera, una constante por carrera (mismo
% criterio que constantes.rkt en la Etapa 2, para no mezclar CE y PI).
% La entrada es el JSON que exporta Racket y la salida es el JSON
% que arma prolog.

ruta_entrada("CE", "../Parte 2/data/salida_CE.json").
ruta_entrada("PI", "../Parte 2/data/salida_PI.json").

ruta_salida("CE", "data/salida_CE.json").
ruta_salida("PI", "data/salida_PI.json").
