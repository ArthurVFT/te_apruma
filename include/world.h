/*
 * world.h - Cabecalho do modulo "mundo" (a logica da fase em si).
 *
 * O World e o "gerente" da Fase 1 - Rua Limpa. Ele:
 *   - guarda uma LISTA ENCADEADA de entidades (sacolas e cachorros);
 *   - cria sacolas em posicoes ALEATORIAS (elas ficam paradas);
 *   - cria cachorros que perseguem a sacola mais proxima;
 *   - controla a colisao (jogador pega sacola / cachorro come sacola);
 *   - controla o TEMPO da fase e decide se o jogador venceu.
 *
 * Repare que o World NAO le teclado do jogador: isso continua sendo
 * responsabilidade de game.c/player.c. Aqui cuidamos do "mundo".
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
    int   score;         /* Pontuacao: +1 por sacola, -1 se o cachorro comer. */
    float timeLeft;      /* Segundos restantes da fase (contagem regressiva). */
    float bagTimer;      /* Segundos ate criar a proxima sacola.            */
    float dogTimer;      /* Segundos ate criar o proximo cachorro.          */
    bool  finished;      /* true quando o tempo acabou.                     */
    bool  won;           /* Resultado (valido quando finished): score > 0?  */
} World;

/* Funcoes do modulo (implementadas em world.c). */
void WorldInit(void);          /* Cria a lista e zera tudo (chamar uma vez). */
void WorldReset(void);         /* Reinicia a fase (limpa entidades e stats). */
void WorldUpdate(void);        /* Spawn + movimento + colisao + tempo.        */
void WorldDraw(void);          /* Desenha todas as entidades.                */
void WorldClose(void);         /* Libera a lista e as entidades.             */

int   WorldGetScore(void);     /* Leitura da pontuacao atual.                */
float WorldGetTimeLeft(void);  /* Segundos restantes da fase.                */
bool  WorldIsFinished(void);   /* true se o tempo acabou.                    */
bool  WorldDidWin(void);       /* true se o jogador venceu a fase.           */

#endif /* WORLD_H */
