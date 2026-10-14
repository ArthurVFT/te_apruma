/*
 * entity.h - Cabecalho do modulo de entidades do jogo.
 *
 * "Entidade" e qualquer objeto que aparece em jogo e tem posicao:
 * sacola de lixo, cachorro caramelo e pessoa. Todos usam a MESMA struct,
 * mudando apenas o "type".
 */

#ifndef ENTITY_H
#define ENTITY_H

#include "raylib.h"     /* Vector2, Color.        */
#include <stdbool.h>    /* Define o tipo booleano "bool" (true/false). */

/*
 * enum EntityType - os tipos possiveis de entidade.
 */
typedef enum {
    ENTITY_BAG,      /* Sacola de lixo. */
    ENTITY_DOG,      /* Cachorro caramelo. */
    ENTITY_PERSON    /* Pessoa (ex.: entrando no canal). */
} EntityType;

/*
 * struct Entity - dados comuns a toda entidade.
 * Assim como Player, e um pacote de campos acessados com ponto ou seta (->).
 */
typedef struct {
    EntityType type;    /* Qual tipo esta entidade e.            */
    Vector2 position;   /* Posicao (x, y) na tela.               */
    Vector2 size;       /* Tamanho do retangulo.                 */
    Vector2 velocity;   /* Velocidade em pixels por segundo.     */
    Color color;        /* Cor usada para desenhar.              */
    bool active;        /* Marca se a entidade ainda esta valida/distinta. */
    float speed;        /* Guarda a magnitude da velocidade.     */
} Entity;

/*
 * Funcoes do modulo (implementadas em entity.c).
 * Note que EntityCreate devolve "Entity*" (um PONTEIRO para a entidade
 * criada na memoria dinamica) e EntityDestroy libera essa memoria.
 */
Entity* EntityCreate(EntityType type, Vector2 position); /* Cria uma entidade (aloca memoria). */
void EntityUpdate(Entity* entity, float deltaTime);      /* Move a entidade.                   */
void EntityDraw(Entity* entity);                         /* Desenha a entidade.                */
void EntityDestroy(Entity* entity);                      /* Libera a memoria da entidade.      */

#endif /* ENTITY_H */
