/* game.c - janela, loop principal e troca de telas. */

#include "game.h"
#include "config.h"
#include "player.h"
#include "world.h"
#include "ranking.h"

/* Em qual tela o jogo esta. Comeca no menu. */
static GameState state = GAME_STATE_MENU;

/* --- Menu principal --- */

#define MENU_ITEMS 2

static const char* menuLabels[MENU_ITEMS] = { "JOGAR", "RANKING" };
static const int   menuY[MENU_ITEMS]      = { 260, 340 };
static int menuChoice = 0;   /* opcao destacada: 0 = Jogar, 1 = Ranking */

/* Retangulo (caixa) de uma opcao do menu, centralizado na tela.
 * Usado tanto para desenhar quanto para detectar o mouse (mesma area). */
static Rectangle MenuButtonBox(int index) {
    const int fontSize = 30;
    float width  = MeasureText(menuLabels[index], fontSize) + 80;
    float height = fontSize + 30;
    return (Rectangle){ (SCREEN_W - width) / 2.0f, (float)menuY[index], width, height };
}

/* Executa a acao da opcao escolhida. */
static void ActivateMenuItem(int index) {
    if (index == 0) {
        WorldReset();
        state = GAME_STATE_PLAYING;
    } else {
        state = GAME_STATE_RANKING;
    }
}

/* Desenha um texto centralizado na horizontal, em qualquer resolucao. */
static void DrawTextCentered(const char* text, int y, int fontSize, Color color) {
    int width = MeasureText(text, fontSize);
    DrawText(text, (SCREEN_W - width) / 2, y, fontSize, color);
}

/* Desenha o menu com a opcao selecionada (borda dourada que pisca). */
static void DrawMenu(void) {
    const int fontSize = 30;

    DrawTextCentered("TE APRUMA!", 90, 50, DARKGREEN);

    for (int i = 0; i < MENU_ITEMS; i++) {
        Rectangle box = MenuButtonBox(i);
        bool selected = (i == menuChoice);

        int textW = MeasureText(menuLabels[i], fontSize);
        DrawText(menuLabels[i],
                 (int)(box.x + (box.width - textW) / 2.0f),
                 (int)(box.y + (box.height - fontSize) / 2.0f),
                 fontSize, selected ? GOLD : DARKGRAY);

        /* A opcao selecionada pisca: aparece em metade dos "meio-segundos". */
        if (selected && ((int)(GetTime() * 2.0)) % 2 == 0) {
            DrawRectangleLinesEx(box, 4, GOLD);
        }
    }

    DrawTextCentered("Setas/WASD ou mouse para escolher", 460, 16, GRAY);
    DrawTextCentered("ENTER ou clique para confirmar", 485, 16, GRAY);
}

void GameInit(void) {
    InitWindow(SCREEN_W, SCREEN_H, "Te Apruma!");
    SetTargetFPS(60);
    PlayerInit();
    WorldInit();
    RankingInit();
}

void GameRun(void) {
    while (!WindowShouldClose()) {
        /* ---------------- UPDATE (logica) ---------------- */
        switch (state) {
            case GAME_STATE_MENU: {
                /* Teclado (com wrap: do fim volta ao comeco). */
                if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
                    menuChoice = (menuChoice + MENU_ITEMS - 1) % MENU_ITEMS;
                if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))
                    menuChoice = (menuChoice + 1) % MENU_ITEMS;

                /* Mouse: passar por cima destaca, clicar confirma. */
                Vector2 mouse = GetMousePosition();
                for (int i = 0; i < MENU_ITEMS; i++) {
                    if (CheckCollisionPointRec(mouse, MenuButtonBox(i))) {
                        menuChoice = i;
                        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                            ActivateMenuItem(i);
                        }
                    }
                }

                /* Teclado: ENTER confirma a opcao destacada. */
                if (IsKeyPressed(KEY_ENTER)) ActivateMenuItem(menuChoice);
                if (IsKeyPressed(KEY_R))      state = GAME_STATE_RANKING;   /* atalho */
                break;
            }

            case GAME_STATE_PLAYING:
                PlayerUpdate();
                WorldUpdate();
                if (WorldIsFinished()) {
                    /* Ao terminar, registra a pontuacao no ranking. */
                    RankingAdd(WorldGetScore(), WorldGetBagsCollected());
                    state = WorldDidWin() ? GAME_STATE_PHASE_COMPLETE
                                          : GAME_STATE_GAMEOVER;
                }
                if (IsKeyPressed(KEY_P)) {
                    state = GAME_STATE_PAUSED;
                }
                break;

            case GAME_STATE_PAUSED:
                if (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_ENTER)) {
                    state = GAME_STATE_PLAYING;
                }
                break;

            case GAME_STATE_PHASE_COMPLETE:
            case GAME_STATE_GAMEOVER:
            case GAME_STATE_RANKING:
            case GAME_STATE_CREDITS:
                if (IsKeyPressed(KEY_ENTER)) {
                    state = GAME_STATE_MENU;
                }
                break;
        }

        /* ---------------- DRAW (desenho) ---------------- */
        BeginDrawing();
        ClearBackground(RAYWHITE);

        switch (state) {
            case GAME_STATE_MENU:
                DrawMenu();
                break;

            case GAME_STATE_PLAYING:
                DrawRectangle(0, GROUND_Y, SCREEN_W, GROUND_HEIGHT, LIGHTGRAY);
                WorldDraw();
                PlayerDraw();

                DrawText("Fase 1 - Rua Limpa", 20, 20, 30, DARKBLUE);
                DrawText(TextFormat("Pontos: %d", WorldGetScore()), 20, 60, 20, DARKGRAY);
                DrawText(TextFormat("Tempo: %.0f s", WorldGetTimeLeft()), 20, 85, 20, RED);
                DrawText("P - Pausar", SCREEN_W - 120, 20, 16, GRAY);
                break;

            case GAME_STATE_PAUSED:
                DrawTextCentered("PAUSADO", 300, 40, DARKGRAY);
                DrawTextCentered("P ou ENTER para continuar", 350, 20, GRAY);
                break;

            case GAME_STATE_PHASE_COMPLETE:
                DrawTextCentered("FASE 1 CONCLUIDA!", 250, 40, DARKGREEN);
                DrawTextCentered(TextFormat("Pontuacao: %d", WorldGetScore()), 310, 26, DARKGRAY);
                DrawTextCentered("ENTER para voltar ao menu", 370, 20, DARKGRAY);
                break;

            case GAME_STATE_GAMEOVER:
                DrawTextCentered("FASE FALHOU", 250, 40, RED);
                DrawTextCentered(TextFormat("Pontuacao final: %d", WorldGetScore()), 310, 24, DARKGRAY);
                DrawTextCentered("ENTER para voltar ao menu", 370, 20, DARKGRAY);
                break;

            case GAME_STATE_RANKING:
                DrawTextCentered("RANKING - MELHORES PONTUACOES", 70, 30, DARKBLUE);

                if (RankingCount() == 0) {
                    DrawTextCentered("Nenhuma pontuacao registrada ainda.", 300, 22, GRAY);
                } else {
                    /* Percorre as LINHAS da matriz e mostra cada posicao. */
                    for (int i = 0; i < RankingCount(); i++) {
                        int pontos  = RankingGet(i, RANKING_POINTS);
                        int sacolas = RankingGet(i, RANKING_BAGS);

                        DrawTextCentered(
                            TextFormat("%d.  %d pontos   (%d sacolas)", i + 1, pontos, sacolas),
                            160 + i * 50, 26, DARKGRAY
                        );
                    }
                }

                DrawTextCentered("ENTER para voltar ao menu", 620, 20, GRAY);
                break;

            case GAME_STATE_CREDITS:
                DrawTextCentered("CREDITOS", 250, 40, DARKGRAY);
                DrawTextCentered("Projeto PIF 2026.2 - CESAR School", 320, 20, DARKGRAY);
                DrawTextCentered("ENTER para voltar ao menu", 380, 20, DARKGRAY);
                break;
        }
        EndDrawing();
    }
}

void GameClose(void) {
    PlayerClose();
    WorldClose();
    CloseWindow();
}

GameState GameGetState(void) {
    return state;
}
