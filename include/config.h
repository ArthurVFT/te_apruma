/*
 * config.h - Configuracoes globais do jogo.
 *
 * Este e o UNICO lugar onde o tamanho da tela e definido. game.c, player.c
 * e world.c todos incluem este arquivo, entao mudar os valores aqui ajusta
 * o jogo inteiro de uma vez so.
 */

#ifndef CONFIG_H
#define CONFIG_H

/* Tamanho da janela do jogo, em pixels. */
#define SCREEN_W 1280
#define SCREEN_H 720

/* Altura e posicao Y da "calcada" (faixa cinza na parte de baixo). */
#define GROUND_HEIGHT 40
#define GROUND_Y      (SCREEN_H - GROUND_HEIGHT)

#endif /* CONFIG_H */
