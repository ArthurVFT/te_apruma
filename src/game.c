/*
 * game.c - Implementacao do modulo principal: janela, loop e troca de telas.
 */

#include "game.h"
#include "player.h"
#include "world.h"   /* Logica da Fase 1 (entidades, pontos, vidas). */

/*
 * static = a variavel "e privada" deste arquivo. Ninguem de fora consegue
 * acessar diretamente; so as funcoes daqui de dentro.
 *
 * "state" guarda em qual tela o jogo esta. Comeca no MENU.
 * "screenWidth"/"screenHeight" definem o tamanho da janela (800x450 pixels).
 */
static GameState state = GAME_STATE_MENU;
static int screenWidth = 800;
static int screenHeight = 450;

/*
 * GameInit - chamada uma vez, no inicio.
 * Abre a janela do Raylib, fixa o limite de quadros por segundo e prepara
 * o jogador.
 */
void GameInit(void) {
    InitWindow(screenWidth, screenHeight, "Te Apruma!"); /* Cria a janela grafica. */
    SetTargetFPS(60);                                    /* Tenta rodar a 60 FPS.  */
    PlayerInit();                                        /* Prepara o jogador.     */
    WorldInit();                                         /* Prepara a Fase 1.      */
}

/*
 * GameRun - o "motor" do jogo.
 * Um loop infinito que so para quando a janela e fechada (WindowShouldClose).
 * A cada repeticao (a cada quadro) ele faz duas etapas:
 *   1) Update: ler teclas e atualizar o estado (o que mudou no jogo).
 *   2) Draw:   desenhar a cena atual na tela.
 */
void GameRun(void) {
    while (!WindowShouldClose()) {   /* WindowShouldClose = true ao clicar no X ou ESC. */
        /* ---------------- UPDATE (logica) ---------------- */
        switch (state) {
            case GAME_STATE_MENU:
                /* No menu, ENTER comeca um jogo novo (zera pontos e vidas). */
                if (IsKeyPressed(KEY_ENTER)) {
                    WorldReset();
                    state = GAME_STATE_PLAYING;
                }
                break;

            case GAME_STATE_PLAYING:
                PlayerUpdate();                          /* Move o jogador conforme as teclas. */
                WorldUpdate();                           /* Cria/move entidades, colisao e tempo. */
                if (WorldIsFinished()) {                 /* O tempo acabou? */
                    /* Venceu se terminou com pontuacao positiva. */
                    state = WorldDidWin() ? GAME_STATE_PHASE_COMPLETE : GAME_STATE_GAMEOVER;
                }
                if (IsKeyPressed(KEY_P)) {               /* P pausa.                           */
                    state = GAME_STATE_PAUSED;
                }
                break;

            case GAME_STATE_PAUSED:
                /* Na pausa, P ou ENTER voltam ao jogo. */
                if (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_ENTER)) {
                    state = GAME_STATE_PLAYING;
                }
                break;

            case GAME_STATE_PHASE_COMPLETE:
            case GAME_STATE_GAMEOVER:
            case GAME_STATE_CREDITS:
                /* Nessas telas, ENTER volta pro menu. */
                if (IsKeyPressed(KEY_ENTER)) {
                    state = GAME_STATE_MENU;
                }
                break;
        }

        /* ---------------- DRAW (desenho) ---------------- */
        BeginDrawing();                 /* Comeca um novo quadro de desenho. */
        ClearBackground(RAYWHITE);      /* Pinta o fundo de branco.          */

        switch (state) {
            case GAME_STATE_MENU:
                /* DrawText(texto, x, y, tamanho, cor) */
                DrawText("TE APRUMA!", 250, 150, 50, DARKGREEN);
                DrawText("Pressione ENTER para Jogar", 220, 220, 20, DARKGRAY);
                DrawText("Agente Comunitario - PIF 2026.2", 200, 280, 16, GRAY);
                DrawText("Setas ou WASD para mover  |  P para pausar", 170, 320, 16, GRAY);
                DrawText("Colete as sacolas antes do cachorro!", 200, 345, 16, GRAY);
                break;

            case GAME_STATE_PLAYING:
                /* Chao: faixa cinza na parte de baixo da tela (visual). */
                DrawRectangle(0, 420, screenWidth, screenHeight - 420, LIGHTGRAY);

                WorldDraw();                                        /* Desenha sacolas e cachorros.      */
                PlayerDraw();                                       /* Desenha o jogador.                */

                DrawText("Fase 1 - Rua Limpa", 20, 20, 30, DARKBLUE);
                /* HUD: TextFormat monta a string com os numeros (tipo printf). */
                DrawText(TextFormat("Pontos: %d", WorldGetScore()), 20, 60, 20, DARKGRAY);
                DrawText(TextFormat("Tempo: %.0f s", WorldGetTimeLeft()), 20, 85, 20, RED);
                DrawText("Pegue as sacolas! Cuidado com o cachorro.", 20, 420 - 24, 16, GRAY);

                DrawText("P - Pausar", screenWidth - 120, 20, 16, GRAY); /* Texto alinhado pela direita. */
                break;

            case GAME_STATE_PAUSED:
                DrawText("PAUSADO", 320, 200, 40, DARKGRAY);
                DrawText("Pressione P ou ENTER para continuar", 180, 250, 20, GRAY);
                break;

            case GAME_STATE_PHASE_COMPLETE:
                DrawText("FASE 1 CONCLUIDA!", 200, 150, 40, DARKGREEN);
                DrawText(TextFormat("Pontuacao: %d", WorldGetScore()), 300, 210, 26, DARKGRAY);
                DrawText("Voce esta pronto para a proxima fase!", 210, 255, 20, GRAY);
                DrawText("ENTER para voltar ao menu", 220, 300, 20, DARKGRAY);
                break;

            case GAME_STATE_GAMEOVER:
                DrawText("FASE FALHOU", 250, 150, 40, RED);
                /* Mostra a pontuacao final alcancada na partida. */
                DrawText(TextFormat("Pontuacao final: %d", WorldGetScore()), 270, 210, 24, DARKGRAY);
                DrawText("Termine a fase com pontos positivos!", 190, 255, 20, GRAY);
                DrawText("ENTER para voltar ao menu", 220, 300, 20, DARKGRAY);
                break;

            case GAME_STATE_CREDITS:
                DrawText("CREDITOS", 320, 150, 40, DARKGRAY);
                DrawText("Projeto PIF 2026.2 - CESAR School", 200, 220, 20, DARKGRAY);
                DrawText("ENTER para voltar ao menu", 220, 280, 20, DARKGRAY);
                break;
        }
        EndDrawing();   /* Finaliza o quadro e mostra tudo na tela. */
    }
}

/*
 * GameClose - chamada depois que o loop termina.
 * Fecha o jogador e a janela do Raylib, liberando recursos.
 */
void GameClose(void) {
    PlayerClose();   /* Libera o que o jogador eventualmente alocou. */
    WorldClose();    /* Libera as entidades da Fase 1 (malloc/free). */
    CloseWindow();   /* Fecha a janela e desliga o Raylib.           */
}

/*
 * GameGetState - permite que outros modulos saibam em que estado o jogo esta.
 * "return" devolve a copia do valor atual.
 */
GameState GameGetState(void) {
    return state;
}
