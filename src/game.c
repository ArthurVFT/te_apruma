/*
 * game.c - Implementacao do modulo principal: janela, loop e troca de telas.
 */

#include "game.h"
#include "config.h"  /* SCREEN_W / SCREEN_H / GROUND_Y (tamanho da janela). */
#include "player.h"
#include "world.h"   /* Logica da Fase 1 (entidades, pontos, tempo). */

/*
 * static = a variavel "e privada" deste arquivo. Ninguem de fora consegue
 * acessar diretamente; so as funcoes daqui de dentro.
 *
 * "state" guarda em qual tela o jogo esta. Comeca no MENU.
 * O tamanho da janela agora vem do config.h (SCREEN_W x SCREEN_H).
 */
static GameState state = GAME_STATE_MENU;

/*
 * DrawTextCentered - desenha um texto centralizado na horizontal.
 * MeasureText devolve a largura, em pixels, que o texto vai ocupar; entao
 * (SCREEN_W - largura) / 2 centraliza em QUALQUER resolucao.
 */
static void DrawTextCentered(const char* text, int y, int fontSize, Color color) {
    int width = MeasureText(text, fontSize);
    DrawText(text, (SCREEN_W - width) / 2, y, fontSize, color);
}

/*
 * GameInit - chamada uma vez, no inicio.
 * Abre a janela do Raylib, fixa o limite de quadros por segundo e prepara
 * o jogador.
 */
void GameInit(void) {
    InitWindow(SCREEN_W, SCREEN_H, "Te Apruma!"); /* Cria a janela grafica. */
    SetTargetFPS(60);                             /* Tenta rodar a 60 FPS.  */
    PlayerInit();                                 /* Prepara o jogador.     */
    WorldInit();                                  /* Prepara a Fase 1.      */
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
                DrawTextCentered("TE APRUMA!", 150, 50, DARKGREEN);
                DrawTextCentered("Pressione ENTER para Jogar", 220, 20, DARKGRAY);
                DrawTextCentered("Agente Comunitario - PIF 2026.2", 280, 16, GRAY);
                DrawTextCentered("Setas ou WASD para mover  |  P para pausar", 320, 16, GRAY);
                DrawTextCentered("Colete as sacolas antes do cachorro!", 345, 16, GRAY);
                break;

            case GAME_STATE_PLAYING:
                /* Calcada: faixa cinza na parte de baixo (posicao vem do config.h). */
                DrawRectangle(0, GROUND_Y, SCREEN_W, GROUND_HEIGHT, LIGHTGRAY);

                WorldDraw();                                        /* Desenha sacolas e cachorros.      */
                PlayerDraw();                                       /* Desenha o jogador.                */

                DrawText("Fase 1 - Rua Limpa", 20, 20, 30, DARKBLUE);
                /* HUD: TextFormat monta a string com os numeros (tipo printf). */
                DrawText(TextFormat("Pontos: %d", WorldGetScore()), 20, 60, 20, DARKGRAY);
                DrawText(TextFormat("Tempo: %.0f s", WorldGetTimeLeft()), 20, 85, 20, RED);
                DrawText("Pegue as sacolas! Cuidado com o cachorro.", 20, GROUND_Y - 24, 16, GRAY);

                DrawText("P - Pausar", SCREEN_W - 120, 20, 16, GRAY); /* Texto alinhado pela direita. */
                break;

            case GAME_STATE_PAUSED:
                DrawTextCentered("PAUSADO", 200, 40, DARKGRAY);
                DrawTextCentered("Pressione P ou ENTER para continuar", 250, 20, GRAY);
                break;

            case GAME_STATE_PHASE_COMPLETE:
                DrawTextCentered("FASE 1 CONCLUIDA!", 150, 40, DARKGREEN);
                DrawTextCentered(TextFormat("Pontuacao: %d", WorldGetScore()), 210, 26, DARKGRAY);
                DrawTextCentered("Voce esta pronto para a proxima fase!", 255, 20, GRAY);
                DrawTextCentered("ENTER para voltar ao menu", 300, 20, DARKGRAY);
                break;

            case GAME_STATE_GAMEOVER:
                DrawTextCentered("FASE FALHOU", 150, 40, RED);
                /* Mostra a pontuacao final alcancada na partida. */
                DrawTextCentered(TextFormat("Pontuacao final: %d", WorldGetScore()), 210, 24, DARKGRAY);
                DrawTextCentered("Termine a fase com pontos positivos!", 255, 20, GRAY);
                DrawTextCentered("ENTER para voltar ao menu", 300, 20, DARKGRAY);
                break;

            case GAME_STATE_CREDITS:
                DrawTextCentered("CREDITOS", 150, 40, DARKGRAY);
                DrawTextCentered("Projeto PIF 2026.2 - CESAR School", 220, 20, DARKGRAY);
                DrawTextCentered("ENTER para voltar ao menu", 280, 20, DARKGRAY);
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
