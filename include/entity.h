#ifndef ENTITY_H
#define ENTITY_H

#include "raylib.h"
#include <stdbool.h>

typedef enum {
    ENTITY_BAG,
    ENTITY_DOG,
    ENTITY_PERSON
} EntityType;

typedef struct {
    EntityType type;
    Vector2 position;
    Vector2 size;
    Vector2 velocity;
    Color color;
    bool active;
    float speed;
} Entity;

Entity* EntityCreate(EntityType type, Vector2 position);
void EntityUpdate(Entity* entity, float deltaTime);
void EntityDraw(Entity* entity);
void EntityDestroy(Entity* entity);

#endif // ENTITY_H