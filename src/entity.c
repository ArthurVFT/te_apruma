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
            entity->velocity = (Vector2){0, 80.0f};   /* cai: y positivo */
            entity->color = BROWN;
            entity->speed = 80.0f;
            break;

        case ENTITY_DOG:
            entity->size = (Vector2){30, 20};
            entity->velocity = (Vector2){-40.0f, 0};  /* anda para a esquerda (x negativo). */
            entity->color = YELLOW;
            entity->speed = 40.0f;
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
 * EntityDraw - desenha a entidade como um retangulo colorido.
 * DrawRectangleV recebe posicao (Vector2) e tamanho (Vector2).
 */
void EntityDraw(Entity* entity) {
    if (entity == NULL || !entity->active) return;
    DrawRectangleV(entity->position, entity->size, entity->color);
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
