/*
 * game.h - Cabecalho (header) do modulo principal do jogo.
 *
 * Um arquivo ".h" serve para declarar o que existe (tipos e funcoes), sem
 * implementar. Assim outros arquivos ".c" sabem o que podem chamar.
 */

/*
 * Include guard: impede que este arquivo seja incluido duas vezes no mesmo
 * arquivo .c (o que causaria erro de definicao duplicada).
 * A ideia: se GAME_H ainda nao foi definido, define e compila o conteudo.
 * Se ja foi definido, o compilador pula tudo ate o #endif.
 */
#ifndef GAME_H
#define GAME_H

#include "raylib.h"   /* Traz os tipos do Raylib (ex.: Vector2, Color). */

/*
 * enum = "enumeracao", uma lista de valores nomeados.
 * Aqui representamos em qual "tela"/estado o jogo esta.
 * Usar nomes (em vez de numeros soltos como 0, 1, 2) deixa o codigo legivel.
 */
typedef enum {
    GAME_STATE_MENU,      /* Tela inicial com o titulo.            */
    GAME_STATE_PLAYING,   /* Jogo rodando de verdade (Fase 1).     */
    GAME_STATE_PAUSED,    /* Jogo congelado.                       */
    GAME_STATE_GAMEOVER,  /* Tela de fim de jogo.                  */
    GAME_STATE_CREDITS    /* Tela de creditos.                     */
} GameState;

/*
 * Prototipos das funcoes do modulo.
 * "(void)" = a funcao nao recebe parametros.
 * Declarar aqui permite que main.c chame essas funcoes sem precisar ver
 * a implementacao (que esta em game.c).
 */
void GameInit(void);          /* Inicializa janela e estado inicial. */
void GameRun(void);           /* Loop principal do jogo.            */
void GameClose(void);         /* Fecha a janela / libera recursos.  */
GameState GameGetState(void); /* Retorna o estado atual do jogo.    */

#endif /* Fecha o include guard GAME_H. */
