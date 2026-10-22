#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include <stdbool.h>

// Dados do jogador (o agente comunitario).
typedef struct {
    Vector2 position;   // canto superior esquerdo, em pixels
    Vector2 size;       // largura e altura do retangulo
    Vector2 velocity;   // reservado para uso futuro
    Color color;
    float speed;        // pixels por segundo
    int   frame;        // quadro atual da animacao
    float frameTimer;   // tempo acumulado no quadro
    bool  facingLeft;   // true = desenha espelhado
} Player;

void PlayerInit(void);
void PlayerUpdate(void);
void PlayerDraw(void);
void PlayerClose(void);
Player* PlayerGet(void);

#endif
