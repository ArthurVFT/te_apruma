/*
 * entity.c - Implementacao das entidades (sacola, cachorro, pessoa).
 */

#include "entity.h"
#include <stdlib.h>   /* malloc e free vem daqui. */

/*
 * EntityCreate - cria uma entidade NOVA na memoria (heap) e devolve o
 * endereco dela. Usar malloc aqui atende o requisito de alocacao dinamica.
 *
 * Passos:
 *  1) Reservar memoria do tamanho de uma struct Entity.
 *  2) Verificar se o malloc deu certo (pode falhar por falta de memoria).
 *  3) Preencher os campos comuns e, depois, os especificos de cada tipo.
 */
Entity* EntityCreate(EntityType type, Vector2 position) {
    Entity* entity = (Entity*)malloc(sizeof(Entity));   /* sizeof(Entity) = quantos bytes a struct ocupa. */
    if (entity == NULL) return NULL;                    /* Se faltou memoria, devolve NULL.              */

    entity->type = type;                 /* O operador "->" acessa campos atraves de um PONTEIRO. */
    entity->position = position;
    entity->active = true;               /* Comeca ativa.                                         */
    entity->velocity = (Vector2){0, 0};  /* Zera a velocidade antes de definir por tipo.          */

    /*
     * Cada tipo ganha tamanho, velocidade e cor proprios.
     * Ex.: a sacola cai para baixo (velocity.y positivo = para baixo na tela),
     * o cachorro anda para a esquerda, a pessoa desce (entra no canal).
     */
    switch (type) {
        case ENTITY_BAG:
            entity->size = (Vector2){15, 20};
            entity->velocity = (Vector2){0, 0};       /* sacola e ESTATICA (nao cai) */
            entity->color = BROWN;
            entity->speed = 0.0f;
            break;

        case ENTITY_DOG:
            entity->size = (Vector2){44, 22};
            entity->velocity = (Vector2){0, 0};       /* a velocidade e calculada pelo world */
            entity->color = ORANGE;                   /* cor "caramelo" para dar destaque */
            entity->speed = 90.0f;                    /* pixels por segundo ao perseguir sacola */
            break;

        case ENTITY_PERSON:
            entity->size = (Vector2){20, 40};
            entity->velocity = (Vector2){0, 40.0f};   /* desce (entra no canal). */
            entity->color = PINK;
            entity->speed = 40.0f;
            break;
    }

    return entity;   /* Devolve o ponteiro para quem chamou. */
}

/*
 * EntityUpdate - move a entidade de acordo com sua velocidade.
 * Multiplica por deltaTime para o movimento ser igual em qualquer FPS.
 * Se a entidade nao existe (NULL) ou ja foi desativada, nao faz nada.
 */
void EntityUpdate(Entity* entity, float deltaTime) {
    if (entity == NULL || !entity->active) return;
    entity->position.x += entity->velocity.x * deltaTime;
    entity->position.y += entity->velocity.y * deltaTime;
}

/*
 * EntityDraw - desenha a entidade.
 * Sacolas e pessoas sao retangulos simples; o cachorro ganha um detalhe
 * (cabeca + olho) para ficar facil de identificar em jogo.
 */
void EntityDraw(Entity* entity) {
    if (entity == NULL || !entity->active) return;

    if (entity->type == ENTITY_DOG) {
        /* Corpo. */
        DrawRectangleV(entity->position, entity->size, entity->color);
        /* Cabeca: quadradinho acima da frente do corpo. */
        DrawRectangle((int)entity->position.x, (int)(entity->position.y - 9), 16, 12, entity->color);
        /* Olho: ponto preto para dar "vida". */
        DrawRectangle((int)entity->position.x + 10, (int)(entity->position.y - 6), 3, 3, BLACK);
        /* Focinho. */
        DrawRectangle((int)entity->position.x - 4, (int)(entity->position.y - 5), 5, 5, DARKBROWN);
    } else {
        DrawRectangleV(entity->position, entity->size, entity->color);
    }
}

/*
 * EntityDestroy - libera a memoria que foi reservada com malloc.
 * IMPORTANTE: toda entidade criada com EntityCreate precisa ser destruida
 * com EntityDestroy, senao a memoria fica "presa" (memory leak).
 */
void EntityDestroy(Entity* entity) {
    if (entity == NULL) return;
    free(entity);
}
