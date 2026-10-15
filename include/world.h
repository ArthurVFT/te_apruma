#ifndef WORLD_H
#define WORLD_H

#include "raylib.h"
#include "list.h"
#include <stdbool.h>

// Estado completo da Fase 1.
typedef struct {
    List* entities;      // lista de Entity* em cena
    int   score;         // +1 por sacola coletada, -1 por sacola comida
    int   bagsCollected; // quantas sacolas o jogador pegou (sem descontos)
    float timeLeft;      // segundos restantes
    float bagTimer;      // espera ate a proxima sacola
    float dogTimer;      // espera ate o proximo cachorro
    bool  finished;      // true quando o tempo acabou
    bool  won;           // resultado (valido se finished): score > 0 ?
} World;

void WorldInit(void);
void WorldReset(void);
void WorldUpdate(void);
void WorldDraw(void);
void WorldClose(void);

int   WorldGetScore(void);
int   WorldGetBagsCollected(void);
float WorldGetTimeLeft(void);
bool  WorldIsFinished(void);
bool  WorldDidWin(void);

#endif
