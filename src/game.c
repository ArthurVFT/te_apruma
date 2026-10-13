#include "game.h"
#include "player.h"

static GameState state = GAME_STATE_MENU;
static int screenWidth = 800;
static int screenHeight = 450;

void GameInit(void) {
    InitWindow(screenWidth, screenHeight, "Te Apruma!");
    SetTargetFPS(60);
    PlayerInit();
}

void GameRun(void) {
    while (!WindowShouldClose()) {
        // Update
        switch (state) {
            case GAME_STATE_MENU:
                if (IsKeyPressed(KEY_ENTER)) {
                    state = GAME_STATE_PLAYING;
                }
                break;
            case GAME_STATE_PLAYING:
                PlayerUpdate();
                if (IsKeyPressed(KEY_P)) {
                    state = GAME_STATE_PAUSED;
                }
                break;
            case GAME_STATE_PAUSED:
                if (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_ENTER)) {
                    state = GAME_STATE_PLAYING;
                }
                break;
            case GAME_STATE_GAMEOVER:
            case GAME_STATE_CREDITS:
                if (IsKeyPressed(KEY_ENTER)) {
                    state = GAME_STATE_MENU;
                }
                break;
        }

        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);

        switch (state) {
            case GAME_STATE_MENU:
                DrawText("TE APRUMA!", 250, 150, 50, DARKGREEN);
                DrawText("Pressione ENTER para Jogar", 220, 220, 20, DARKGRAY);
                DrawText("Agente Comunitario - PIF 2026.2", 200, 280, 16, GRAY);
                break;
            case GAME_STATE_PLAYING:
                DrawText("Fase 1 - Rua Limpa", 20, 20, 30, DARKBLUE);
                PlayerDraw();
                DrawText("P - Pausar", screenWidth - 120, 20, 16, GRAY);
                break;
            case GAME_STATE_PAUSED:
                DrawText("PAUSADO", 320, 200, 40, DARKGRAY);
                DrawText("Pressione P ou ENTER para continuar", 180, 250, 20, GRAY);
                break;
            case GAME_STATE_GAMEOVER:
                DrawText("GAME OVER", 280, 200, 40, RED);
                DrawText("ENTER para voltar ao menu", 220, 250, 20, DARKGRAY);
                break;
            case GAME_STATE_CREDITS:
                DrawText("CREDITOS", 320, 150, 40, DARKGRAY);
                DrawText("Projeto PIF 2026.2 - CESAR School", 200, 220, 20, DARKGRAY);
                DrawText("ENTER para voltar ao menu", 220, 280, 20, DARKGRAY);
                break;
        }
        EndDrawing();
    }
}

void GameClose(void) {
    PlayerClose();
    CloseWindow();
}

GameState GameGetState(void) {
    return state;
}