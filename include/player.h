/*
 * player.h - Cabecalho do modulo do jogador (o agente comunitario).
 */

#ifndef PLAYER_H   /* Include guard: evita incluir este arquivo duas vezes. */
#define PLAYER_H

#include "raylib.h"   /* Vector2 e Color vem daqui. */

/*
 * struct Player - agrupa todos os dados do jogador em um unico "pacote".
 * Uma struct e como um formulario com varios campos; voce acessa cada campo
 * com o operador ponto: player.position, player.speed etc.
 */
typedef struct {
    Vector2 position;   /* Posicao (x, y) em pixels no canto superior esquerdo. */
    Vector2 size;       /* Largura e altura do retangulo que representa o jogador. */
    Vector2 velocity;   /* Velocidade atual (reservado para usos futuros).       */
    Color color;        /* Cor do retangulo do jogador.                          */
    float speed;        /* Quantos pixels o jogador anda por segundo.            */
} Player;

/* Funcoes do modulo (implementadas em player.c). */
void PlayerInit(void);      /* Define os valores iniciais do jogador.      */
void PlayerUpdate(void);    /* Le o teclado e move o jogador.              */
void PlayerDraw(void);      /* Desenha o jogador na tela.                  */
void PlayerClose(void);     /* Libera recursos do jogador.                 */
Player* PlayerGet(void);    /* Devolve um PONTEIRO para o jogador (acesso direto). */

#endif /* PLAYER_H */
