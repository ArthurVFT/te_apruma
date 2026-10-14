#ifndef ENTITY_H
#define ENTITY_H

#include "raylib.h"
#include <stdbool.h>

/* Tipos de entidade do jogo. */
typedef enum {
    ENTITY_BAG,
    ENTITY_DOG,
    ENTITY_PERSON
} EntityType;

/* Dados comuns a toda entidade. */
typedef struct {
    EntityType type;
    Vector2 position;
    Vector2 size;
    Vector2 velocity;
    Color color;
    bool active;      /* false = deve sair da lista na faxina */
    float speed;      /* magnitude da velocidade (pixels/s)   */
} Entity;

/* EntityCreate aloca na memoria dinamica; EntityDestroy libera. */
Entity* EntityCreate(EntityType type, Vector2 position);
void EntityUpdate(Entity* entity, float deltaTime);
void EntityDraw(Entity* entity);
void EntityDestroy(Entity* entity);

#endif
