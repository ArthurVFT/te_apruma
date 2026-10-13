#ifndef GAME_H
#define GAME_H

#include "raylib.h"

typedef enum {
    GAME_STATE_MENU,
    GAME_STATE_PLAYING,
    GAME_STATE_PAUSED,
    GAME_STATE_GAMEOVER,
    GAME_STATE_CREDITS
} GameState;

void GameInit(void);
void GameRun(void);
void GameClose(void);
GameState GameGetState(void);

#endif // GAME_H