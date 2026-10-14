#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"

/* Dados do jogador (o agente comunitario). */
typedef struct {
    Vector2 position;   /* canto superior esquerdo, em pixels */
    Vector2 size;       /* largura e altura do retangulo      */
    Vector2 velocity;   /* reservado para uso futuro          */
    Color color;
    float speed;        /* pixels por segundo                 */
} Player;

void PlayerInit(void);
void PlayerUpdate(void);
void PlayerDraw(void);
void PlayerClose(void);
Player* PlayerGet(void);

#endif
