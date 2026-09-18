//
// Created by jpaul on 9/17/2026.
//

#ifndef PARTE_1_CONSTANTES_Y_AUXILIARES_H
#define PARTE_1_CONSTANTES_Y_AUXILIARES_H
struct Curso {
    char codigo[15];
    char nombre[100];
    int creditos;
    int horas;
    int semestre;
    char requisitos[30];
    char correquisitos[10];
    char tipo[10];
};

struct Nodo {
    struct Curso curso;
    struct Nodo *siguiente;
};

void liberarLista(Nodo **inicio);

void insertarInicio(Nodo **inicio, Curso Curso);

void cargar_plan_estudios(Nodo **inicio);

#endif //PARTE_1_CONSTANTES_Y_AUXILIARES_H