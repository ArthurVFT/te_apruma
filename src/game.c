// game.c - janela, loop principal e troca de telas.

#include "game.h"
#include "config.h"
#include "player.h"
#include "world.h"
#include "ranking.h"
#include "textures.h"  
#include "canal.h" 
#include "audio.h"

// Em qual tela o jogo esta. Comeca no menu.
static GameState state = GAME_STATE_MENU;

static int currentPhase = 1;   // fase inicial

// Resultado da fase 1 guardado ate a fase 2 acabar: a partida vira uma unica linha no ranking.
static int roundPoints = 0;
static int roundBags   = 0;


//  Menu principal

#define MENU_ITEMS 3

static const char* menuLabels[MENU_ITEMS] = { "JOGAR", "RANKING", "SAIR" };
static const int   menuY[MENU_ITEMS]      = { 240, 315, 390 };
static int menuChoice = 0;   // opcao destacada: 0 = Jogar, 1 = Ranking, 2 = Sair

//Menu de pausa (aberto com ESC ou P durante a fase)

#define PAUSE_ITEMS 3

static const char* pauseLabels[PAUSE_ITEMS] = { "CONTINUAR", "MENU PRINCIPAL", "SAIR" };
static const int   pauseY[PAUSE_ITEMS]      = { 260, 330, 400 };
static int pauseChoice = 0;  // opcao destacada: 0 = Continuar, 1 = Menu, 2 = Sair

// Vira true quando o jogador escolhe SAIR; o loop principal para ao ver isso.
static bool quitRequested = false;

// Retangulo (caixa) de um botao, centralizado na tela.
// Usado tanto para desenhar quanto para detectar o mouse (mesma area).
static Rectangle ButtonBox(const char* label, int y) {
    const int fontSize = 30;
    float width  = MeasureText(label, fontSize) + 80;
    float height = fontSize + 30;
    return (Rectangle){ (SCREEN_W - width) / 2.0f, (float)y, width, height };
}

static Rectangle MenuButtonBox(int index) {
    return ButtonBox(menuLabels[index], menuY[index]);
}

static Rectangle PauseButtonBox(int index) {
    return ButtonBox(pauseLabels[index], pauseY[index]);
}

// Executa a acao da opcao escolhida.
static void ActivateMenuItem(int index) {
    if (index == 0) {
        currentPhase = 1;
        roundPoints = 0;
        roundBags = 0;
        WorldReset();
        state = GAME_STATE_PLAYING;
    } else if (index == 1) {
        state = GAME_STATE_RANKING;
    } else {
        quitRequested = true;
    }
}

// Abandonar a partida nao registra pontuacao no ranking.
static void ActivatePauseItem(int index) {
    if (index == 0) {
        state = GAME_STATE_PLAYING;
    } else if (index == 1) {
        state = GAME_STATE_MENU;
    } else {
        quitRequested = true;
    }
}

// Desenha um texto centralizado na horizontal, em qualquer resolucao.
static void DrawTextCentered(const char* text, int y, int fontSize, Color color) {
    int width = MeasureText(text, fontSize);
    DrawText(text, (SCREEN_W - width) / 2, y, fontSize, color);
}

// Desenha uma lista de botoes com a opcao selecionada (borda dourada que pisca).
static void DrawButtons(const char** labels, const int* ys, int count, int choice) {
    const int fontSize = 30;

    for (int i = 0; i < count; i++) {
        Rectangle box = ButtonBox(labels[i], ys[i]);
        bool selected = (i == choice);

        int textW = MeasureText(labels[i], fontSize);
        DrawText(labels[i],
                 (int)(box.x + (box.width - textW) / 2.0f),
                 (int)(box.y + (box.height - fontSize) / 2.0f),
                 fontSize, selected ? GOLD : DARKGRAY);

        // A opcao selecionada pisca: aparece em metade dos "meio-segundos".
        if (selected && ((int)(GetTime() * 2.0)) % 2 == 0) {
            DrawRectangleLinesEx(box, 4, GOLD);
        }
    }
}

static void DrawMenu(void) {
    DrawTextCentered("TE APRUMA!", 90, 50, DARKGREEN);
    DrawButtons(menuLabels, menuY, MENU_ITEMS, menuChoice);

    DrawTextCentered("Setas/WASD ou mouse para escolher", 470, 16, GRAY);
    DrawTextCentered("ENTER ou clique para confirmar", 495, 16, GRAY);
}

void GameInit(void) {
    InitWindow(SCREEN_W, SCREEN_H, "Te Apruma!");
    // Sem isso, o raylib fecha o jogo ao apertar ESC. Agora ESC abre o menu de pausa.
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);
    TexturesLoad();
    PlayerInit();
    WorldInit();
    CanalInit();
    RankingInit();
    AudioLoad();
}

void GameRun(void) {
    while (!WindowShouldClose() && !quitRequested) {
        AudioUpdate();   // atualiza a musica de fundo
        // UPDATE (logica)
        switch (state) {
            case GAME_STATE_MENU: {
                AudioResumeMusic();  // caso venha do ranking ou creditos, a musica volta
                // Teclado (com wrap: do fim volta ao comeco).
                if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
                    menuChoice = (menuChoice + MENU_ITEMS - 1) % MENU_ITEMS;
                if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))
                    menuChoice = (menuChoice + 1) % MENU_ITEMS;

                // Mouse: passar por cima destaca, clicar confirma.
                Vector2 mouse = GetMousePosition();
                for (int i = 0; i < MENU_ITEMS; i++) {
                    if (CheckCollisionPointRec(mouse, MenuButtonBox(i))) {
                        menuChoice = i;
                        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                            ActivateMenuItem(i);
                        }
                    }
                }

                // Teclado: ENTER confirma a opcao destacada.
                if (IsKeyPressed(KEY_ENTER)) ActivateMenuItem(menuChoice);
                if (IsKeyPressed(KEY_R))      state = GAME_STATE_RANKING;   // atalho
                break;
            }

            case GAME_STATE_PLAYING:
                PlayerUpdate();
                if (currentPhase == 1) {
                    WorldUpdate();
                    if (WorldIsFinished()) {
                        if (WorldDidWin()) {
                            // Venceu: a partida continua na fase 2 e so entra no ranking no fim.
                            roundPoints = WorldGetScore();
                            roundBags = WorldGetBagsCollected();
                            state = GAME_STATE_PHASE_COMPLETE;
                            AudioPlayWin();
                        } else {
                            // Perdeu a fase 1: a partida acaba aqui, sem pessoas resgatadas.
                            RankingAdd(WorldGetScore(), WorldGetBagsCollected(), 0);
                            state = GAME_STATE_GAMEOVER;
                            AudioPlayGameOver();
                        }
                    }
                } else if (currentPhase == 2) {
                    CanalUpdate();
                    if (CanalIsFinished()) {
                        // Fim da partida: uma linha com pontos das duas fases, sacolas da fase 1 e pessoas da fase 2.
                        RankingAdd(roundPoints + CanalGetScore(), roundBags, CanalGetPeopleRescued());
                        state = CanalDidWin() ? GAME_STATE_PHASE_COMPLETE
                                              : GAME_STATE_GAMEOVER;
                        if (state == GAME_STATE_GAMEOVER) AudioPlayGameOver();
                        else                              AudioPlayWin();
                    }
                }
                if (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_ESCAPE)) {
                    pauseChoice = 0;
                    state = GAME_STATE_PAUSED;
                }
                break;

            case GAME_STATE_PAUSED: {
                if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
                    pauseChoice = (pauseChoice + PAUSE_ITEMS - 1) % PAUSE_ITEMS;
                if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))
                    pauseChoice = (pauseChoice + 1) % PAUSE_ITEMS;

                Vector2 mouse = GetMousePosition();
                for (int i = 0; i < PAUSE_ITEMS; i++) {
                    if (CheckCollisionPointRec(mouse, PauseButtonBox(i))) {
                        pauseChoice = i;
                        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                            ActivatePauseItem(i);
                        }
                    }
                }

                if (IsKeyPressed(KEY_ENTER)) ActivatePauseItem(pauseChoice);
                // ESC ou P tambem voltam ao jogo, como atalho.
                if (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_ESCAPE)) state = GAME_STATE_PLAYING;
                break;
            }

            case GAME_STATE_PHASE_COMPLETE:
                if (IsKeyPressed(KEY_ENTER)) {
                    AudioResumeMusic();
                    if (currentPhase == 1) {
                        currentPhase = 2;
                        CanalReset();
                        state = GAME_STATE_PLAYING;
                    } else {
                        state = GAME_STATE_MENU;
                    }
                }
                break;

            case GAME_STATE_GAMEOVER:
            case GAME_STATE_RANKING:
            case GAME_STATE_CREDITS:
                if (IsKeyPressed(KEY_ENTER)) {
                    AudioResumeMusic();
                    state = GAME_STATE_MENU;
                }
                break;
        }

        // DRAW (desenho) 
        BeginDrawing();
        ClearBackground(RAYWHITE);

        switch (state) {
            case GAME_STATE_MENU:
                DrawMenu();
                break;

            case GAME_STATE_PLAYING:
                if (currentPhase == 1) {
                    if (TexturesStreetBgLoaded()) {
                        DrawTexture(TexturesGetStreetBg(), 0, 0, WHITE);
                    } else {
                        DrawRectangle(0, GROUND_Y, SCREEN_W, GROUND_HEIGHT, LIGHTGRAY);
                    }
                    WorldDraw();
                } else {
                    CanalDraw();
                }
                PlayerDraw();

                if (currentPhase == 1) {
                    DrawText("Fase 1 - Rua Limpa", 20, 20, 30, DARKBLUE);
                    DrawText(TextFormat("Pontos: %d", WorldGetScore()), 20, 60, 20, DARKGRAY);
                    DrawText(TextFormat("Tempo: %.0f s", WorldGetTimeLeft()), 20, 85, 20, RED);
                } else {
                    DrawText("Fase 2 - Canal", 20, 20, 30, DARKBLUE);
                    DrawText(TextFormat("Pontos: %d", CanalGetScore()), 20, 60, 20, DARKGRAY);
                    DrawText(TextFormat("Tempo: %.0f s", CanalGetTimeLeft()), 20, 85, 20, RED);
                    DrawText(TextFormat("Na agua: %d/6", CanalGetPeopleInWater()), 20, 110, 20, BLUE);
                }
                DrawText("ESC - Pausar", SCREEN_W - 140, 20, 16, GRAY);
                break;

            case GAME_STATE_PAUSED:
                DrawTextCentered("PAUSADO", 200, 40, DARKGRAY);
                DrawButtons(pauseLabels, pauseY, PAUSE_ITEMS, pauseChoice);
                DrawTextCentered("Setas/WASD ou mouse para escolher", 470, 16, GRAY);
                DrawTextCentered("ESC ou P para voltar ao jogo", 495, 16, GRAY);
                break;

            case GAME_STATE_PHASE_COMPLETE:
                DrawTextCentered(TextFormat("FASE %d CONCLUIDA!", currentPhase), 250, 40, DARKGREEN);
                DrawTextCentered(TextFormat("Pontuacao: %d",
                                 currentPhase == 1 ? WorldGetScore() : CanalGetScore()),
                                 310, 26, DARKGRAY);
                DrawTextCentered(currentPhase == 1 ? "ENTER para ir a fase 2"
                                                   : "ENTER para voltar ao menu",
                                 370, 20, DARKGRAY);
                break;

            case GAME_STATE_GAMEOVER:
                DrawTextCentered(TextFormat("FASE %d FALHOU", currentPhase), 250, 40, RED);
                DrawTextCentered(TextFormat("Pontuacao final: %d",
                                 currentPhase == 1 ? WorldGetScore() : CanalGetScore()),
                                 310, 24, DARKGRAY);
                DrawTextCentered("ENTER para voltar ao menu", 370, 20, DARKGRAY);
                break;

            case GAME_STATE_RANKING:
                DrawTextCentered("RANKING - MELHORES PONTUACOES", 70, 30, DARKBLUE);

                if (RankingCount() == 0) {
                    DrawTextCentered("Nenhuma pontuacao registrada ainda.", 300, 22, GRAY);
                } else {
                    // Percorre as LINHAS da matriz e mostra cada posicao.
                    for (int i = 0; i < RankingCount(); i++) {
                        int pontos   = RankingGet(i, RANKING_POINTS);
                        int sacolas  = RankingGet(i, RANKING_BAGS);
                        int resgates = RankingGet(i, RANKING_RESCUED);

                        DrawTextCentered(
                            TextFormat("%d.  %d pontos   (%d sacolas, %d pessoas)",
                                       i + 1, pontos, sacolas, resgates),
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
    CanalClose();
    TexturesUnload();
    AudioUnload();
    CloseWindow();
}

GameState GameGetState(void) {
    return state;
}