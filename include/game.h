// game.h - modulo principal: telas e loop do jogo.

#ifndef GAME_H
#define GAME_H

#include "raylib.h"

// Cada valor representa uma tela do jogo.
typedef enum {
    GAME_STATE_MENU,
    GAME_STATE_PLAYING,
    GAME_STATE_PAUSED,
    GAME_STATE_PHASE_COMPLETE,
    GAME_STATE_GAMEOVER,
    GAME_STATE_RANKING,
    GAME_STATE_CREDITS
} GameState;

void GameInit(void);
void GameRun(void);
void GameClose(void);
GameState GameGetState(void);

#endif
