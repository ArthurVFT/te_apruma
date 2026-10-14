/*
 * world.h - Cabecalho do modulo "mundo" (a logica da fase em si).
 *
 * O World e o "gerente" da Fase 1 - Rua Limpa. Ele:
 *   - guarda uma LISTA ENCADEADA de entidades (sacolas e cachorros);
 *   - cria (spawn) entidades de tempo em tempo;
 *   - move todas e checa colisao com o jogador;
 *   - controla pontuacao, vidas e fim de jogo.
 *
 * Repare que o World NAO desenha nem le teclado do jogador: isso continua
 * sendo responsabilidade de game.c/player.c. Aqui cuidamos do "mundo".
 */

#ifndef WORLD_H
#define WORLD_H

#include "raylib.h"    /* Vector2, Color, Rectangle... */
#include "list.h"      /* O tipo List.                 */
#include <stdbool.h>   /* bool (true/false).           */

/*
 * struct World - o estado completo da fase.
 */
typedef struct {
    List* entities;      /* Lista encadeada de Entity* (as coisas em cena). */
    int   score;         /* Pontuacao do jogador.                          */
    int   lives;         /* Vidas restantes.                                */
    float spawnTimer;    /* Segundos ate criar a proxima entidade.          */
    float spawnInterval; /* Intervalo atual entre criacoes (diminui = dificil). */
    float invulnTimer;   /* Segundos de invulnerabilidade apos tomar dano.  */
    bool  gameOver;      /* true quando acabaram as vidas.                  */
} World;

/* Funcoes do modulo (implementadas em world.c). */
void WorldInit(void);          /* Cria a lista e zera tudo (chamar uma vez). */
void WorldReset(void);         /* Reinicia a fase (limpa entidades e stats). */
void WorldUpdate(void);        /* Spawn + movimento + colisao (rodar por quadro). */
void WorldDraw(void);          /* Desenha todas as entidades.                */
void WorldClose(void);         /* Libera a lista e as entidades.             */

int  WorldGetScore(void);      /* Leitura da pontuacao atual.                */
int  WorldGetLives(void);      /* Leitura das vidas atuais.                  */
bool WorldIsGameOver(void);    /* true se o jogo acabou.                     */

#endif /* WORLD_H */
