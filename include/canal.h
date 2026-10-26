#ifndef CANAL_H
#define CANAL_H

#include "raylib.h"
#include "list.h"
#include <stdbool.h>

// Estado completo da Fase 2
typedef struct {
    List* entities;      // lista de Entity* em cena
    int   score;         // +1 por sacola coletada, -1 por sacola comida
    float timeLeft;      // segundos restantes
    bool  finished;      // true quando o tempo acabou
    bool  won;           // resultado (valido se finished): score > 0 ?
    int peopleRescued; // quantas pessoas o agente tirou
    int peopleInWater; // quantas pessoas estão no canal agora
    float personTimer; // tempo até a proxima pessoa aparecer
} Canal;

void CanalInit(void);
void CanalReset(void);
void CanalUpdate(void);
void CanalDraw(void);
void CanalClose(void);

int   CanalGetScore(void);
int   CanalGetPeopleRescued(void);
float CanalGetTimeLeft(void);
bool  CanalIsFinished(void);
bool  CanalDidWin(void);

#endif
