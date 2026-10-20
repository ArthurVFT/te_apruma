// entity.c - criacao, atualizacao e desenho das entidades.

#include "entity.h"
#include <stdlib.h>   // malloc / free
#include "textures.h"

#define DOG_FRAME_TIME  0.09f   // segundos por quadro
#define DOG_DRAW_SCALE  2.0f    // 32x32 aparece com 64x64

Entity* EntityCreate(EntityType type, Vector2 position) {
    Entity* entity = (Entity*)malloc(sizeof(Entity));
    if (entity == NULL) return NULL;

    entity->type = type;
    entity->position = position;
    entity->velocity = (Vector2){0, 0};
    entity->active = true;
    entity->frame = 0;
    entity->frameTimer = 0.0f;
    entity->facingLeft = false;

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

    // Multiplicar por deltaTime mantem o movimento igual em qualquer FPS.
    entity->position.x += entity->velocity.x * deltaTime;
    entity->position.y += entity->velocity.y * deltaTime;
    if (entity->type == ENTITY_DOG) {
        if (entity->velocity.x >  1.0f) entity->facingLeft = false;
        if (entity->velocity.x < -1.0f) entity->facingLeft = true;

        entity->frameTimer += deltaTime;
        if (entity->frameTimer >= DOG_FRAME_TIME) {
            entity->frameTimer -= DOG_FRAME_TIME;
            entity->frame = (entity->frame + 1) % DOG_FRAME_COUNT;
        }
    }    
}

void EntityDraw(Entity* entity) {
    if (entity == NULL || !entity->active) return;

    if (entity->type == ENTITY_DOG && TexturesDogLoaded()) {
        Rectangle source = {
            0.0f,
            (float)(entity->frame * DOG_FRAME_H),
            entity->facingLeft ? -(float)DOG_FRAME_W : (float)DOG_FRAME_W,
            (float)DOG_FRAME_H
        };

        float w = DOG_FRAME_W * DOG_DRAW_SCALE;
        float h = DOG_FRAME_H * DOG_DRAW_SCALE;
        Rectangle dest = {
            entity->position.x + entity->size.x * 0.5f - w * 0.5f,
            entity->position.y + entity->size.y * 0.5f - h * 0.5f,
            w, h
        };

        DrawTexturePro(TexturesGetDog(), source, dest, (Vector2){0, 0}, 0.0f, WHITE);
    } else if (entity->type == ENTITY_DOG) {
        // reserva: seus retangulos de antes, caso a imagem nao carregue
        DrawRectangleV(entity->position, entity->size, entity->color);
        DrawRectangle((int)entity->position.x, (int)(entity->position.y - 9), 16, 12, entity->color);
        DrawRectangle((int)entity->position.x + 10, (int)(entity->position.y - 6), 3, 3, BLACK);
        DrawRectangle((int)entity->position.x - 4, (int)(entity->position.y - 5), 5, 5, DARKBROWN);
    } else {
        DrawRectangleV(entity->position, entity->size, entity->color);
    }
    
}

// Toda entidade criada com EntityCreate precisa ser liberada aqui.
void EntityDestroy(Entity* entity) {
    if (entity == NULL) return;
    free(entity);
}
