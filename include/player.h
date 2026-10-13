#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"

typedef struct {
    Vector2 position;
    Vector2 size;
    Vector2 velocity;
    Color color;
    float speed;
} Player;

void PlayerInit(void);
void PlayerUpdate(void);
void PlayerDraw(void);
void PlayerClose(void);
Player* PlayerGet(void);

#endif // PLAYER_H