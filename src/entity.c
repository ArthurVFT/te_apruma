#include "entity.h"
#include <stdlib.h>

Entity* EntityCreate(EntityType type, Vector2 position) {
    Entity* entity = (Entity*)malloc(sizeof(Entity));
    if (entity == NULL) return NULL;
    
    entity->type = type;
    entity->position = position;
    entity->active = true;
    entity->velocity = (Vector2){0, 0};
    
    switch (type) {
        case ENTITY_BAG:
            entity->size = (Vector2){15, 20};
            entity->velocity = (Vector2){0, 80.0f};
            entity->color = BROWN;
            entity->speed = 80.0f;
            break;
        case ENTITY_DOG:
            entity->size = (Vector2){30, 20};
            entity->velocity = (Vector2){-40.0f, 0};
            entity->color = YELLOW;
            entity->speed = 40.0f;
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
    entity->position.x += entity->velocity.x * deltaTime;
    entity->position.y += entity->velocity.y * deltaTime;
}

void EntityDraw(Entity* entity) {
    if (entity == NULL || !entity->active) return;
    DrawRectangleV(entity->position, entity->size, entity->color);
}

void EntityDestroy(Entity* entity) {
    if (entity == NULL) return;
    free(entity);
}