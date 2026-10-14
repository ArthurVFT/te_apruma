/* entity.c - criacao, atualizacao e desenho das entidades. */

#include "entity.h"
#include <stdlib.h>   /* malloc / free */

Entity* EntityCreate(EntityType type, Vector2 position) {
    Entity* entity = (Entity*)malloc(sizeof(Entity));
    if (entity == NULL) return NULL;

    entity->position = position;
    entity->velocity = (Vector2){0, 0};
    entity->active = true;

    switch (type) {
        case ENTITY_BAG:
            entity->size = (Vector2){15, 20};
            entity->color = BROWN;
            entity->speed = 0.0f;
            break;

        case ENTITY_DOG:
            entity->size = (Vector2){44, 22};
            entity->color = ORANGE;
            entity->speed = 90.0f;
            break;

        case ENTITY_PERSON:
            entity->size = (Vector2){20, 40};
            entity->velocity = (Vector2){0, 40.0f};
            entity->color = PINK;
            entity->speed = 40.0f;
            break;
    }

    return entity;
}

void EntityUpdate(Entity* entity, float deltaTime) {
    if (entity == NULL || !entity->active) return;

    /* Multiplicar por deltaTime mantem o movimento igual em qualquer FPS. */
    entity->position.x += entity->velocity.x * deltaTime;
    entity->position.y += entity->velocity.y * deltaTime;
}

void EntityDraw(Entity* entity) {
    if (entity == NULL || !entity->active) return;

    if (entity->type == ENTITY_DOG) {
        /* Corpo + cabeca + olho + focinho, para o cachorro se destacar. */
        DrawRectangleV(entity->position, entity->size, entity->color);
        DrawRectangle((int)entity->position.x, (int)(entity->position.y - 9), 16, 12, entity->color);
        DrawRectangle((int)entity->position.x + 10, (int)(entity->position.y - 6), 3, 3, BLACK);
        DrawRectangle((int)entity->position.x - 4, (int)(entity->position.y - 5), 5, 5, DARKBROWN);
    } else {
        DrawRectangleV(entity->position, entity->size, entity->color);
    }
}

/* Toda entidade criada com EntityCreate precisa ser liberada aqui. */
void EntityDestroy(Entity* entity) {
    if (entity == NULL) return;
    free(entity);
}
