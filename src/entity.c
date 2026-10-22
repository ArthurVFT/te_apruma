// entity.c - criacao, atualizacao e desenho das entidades.

#include "entity.h"
#include <stdlib.h>   // malloc / free
#include "textures.h"

// Tamanho na tela = quadro (32) x escala. A hitbox usa o mesmo valor.
#define DOG_DRAW_SCALE     2.5f    // 80x80
#define BAG_DRAW_SCALE     1.5f    // 48x48

#define DOG_FRAME_TIME     0.09f   // segundos por quadro
#define BAG_FRAME_TIME     0.15f   // sacola balanca mais devagar

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
            // hitbox derivada da escala: 32 * 1.5 = 48x48
            entity->size = (Vector2){ BAG_FRAME_W * BAG_DRAW_SCALE,
                                      BAG_FRAME_H * BAG_DRAW_SCALE };
            entity->color = BROWN;
            entity->speed = 0.0f;
            break;

        case ENTITY_DOG:
            // hitbox derivada da escala: 32 * 2.5 = 80x80
            entity->size = (Vector2){ DOG_FRAME_W * DOG_DRAW_SCALE,
                                      DOG_FRAME_H * DOG_DRAW_SCALE };
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
        // Vira o sprite conforme a direcao horizontal do movimento.
        if (entity->velocity.x >  1.0f) entity->facingLeft = false;
        if (entity->velocity.x < -1.0f) entity->facingLeft = true;

        if (entity->velocity.x == 0.0f && entity->velocity.y == 0.0f) {
            // parado: nao avanca e volta pra pose inicial (quadro 0)
            entity->frameTimer = 0.0f;
            entity->frame = 0;
        } else {
            entity->frameTimer += deltaTime;
            if (entity->frameTimer >= DOG_FRAME_TIME) {
                entity->frameTimer -= DOG_FRAME_TIME;
                entity->frame = (entity->frame + 1) % DOG_FRAME_COUNT;
            }
        }
    } else if (entity->type == ENTITY_BAG) {
        // sacola: anima sozinha, sempre (ela balanca mesmo parada)
        entity->frameTimer += deltaTime;
        if (entity->frameTimer >= BAG_FRAME_TIME) {
            entity->frameTimer -= BAG_FRAME_TIME;
            entity->frame = (entity->frame + 1) % BAG_FRAME_COUNT;
        }
    }
}

void EntityDraw(Entity* entity) {
    if (entity == NULL || !entity->active) return;

    if (entity->type == ENTITY_DOG && TexturesDogLoaded()) {
        Rectangle source = {
            0.0f,
            (float)(entity->frame * DOG_FRAME_H),
            entity->facingLeft ? (float)DOG_FRAME_W : -(float)DOG_FRAME_W,
            (float)DOG_FRAME_H
        };

        float w = DOG_FRAME_W * DOG_DRAW_SCALE;
        float h = DOG_FRAME_H * DOG_DRAW_SCALE;
        Rectangle dest = { entity->position.x, entity->position.y, w, h };

        DrawTexturePro(TexturesGetDog(), source, dest, (Vector2){0, 0}, 0.0f, WHITE);
    } else if (entity->type == ENTITY_DOG) {
        // reserva: seus retangulos de antes, caso a imagem nao carregue
        DrawRectangleV(entity->position, entity->size, entity->color);
        DrawRectangle((int)entity->position.x, (int)(entity->position.y - 9), 16, 12, entity->color);
        DrawRectangle((int)entity->position.x + 10, (int)(entity->position.y - 6), 3, 3, BLACK);
        DrawRectangle((int)entity->position.x - 4, (int)(entity->position.y - 5), 5, 5, DARKBROWN);
    } else if (entity->type == ENTITY_BAG && TexturesBagLoaded()) {
        // sacola: quadro atual da folha, no tamanho derivado da escala
        Rectangle source = {
            0.0f,
            (float)(entity->frame * BAG_FRAME_H),
            (float)BAG_FRAME_W,
            (float)BAG_FRAME_H
        };
        Rectangle dest = { entity->position.x, entity->position.y,
                           BAG_FRAME_W * BAG_DRAW_SCALE,
                           BAG_FRAME_H * BAG_DRAW_SCALE };

        DrawTexturePro(TexturesGetBag(), source, dest, (Vector2){0, 0}, 0.0f, WHITE);
    } else {
        DrawRectangleV(entity->position, entity->size, entity->color);
    }
    
}

// Toda entidade criada com EntityCreate precisa ser liberada aqui.
void EntityDestroy(Entity* entity) {
    if (entity == NULL) return;
    free(entity);
}
