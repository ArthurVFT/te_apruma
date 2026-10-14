/*
 * world.c - Fase 1 - Rua Limpa.
 *
 * Regras:
 *   - Sacolas aparecem em posicoes aleatorias e ficam paradas.
 *   - Jogador encosta na sacola -> +1 ponto.
 *   - O cachorro persegue a sacola mais proxima; se pegar antes, ela vale -1.
 *   - Ha um tempo limite; ao fim, vence quem terminar com score > 0.
 */

#include "world.h"
#include "config.h"
#include "entity.h"
#include "player.h"
#include <math.h>   /* sqrtf */

/* Ajustes de dificuldade. */
#define PHASE_DURATION   30.0f
#define BAG_INTERVAL      1.0f
#define DOG_INTERVAL      6.0f

/* Margens para as sacolas nao nascerem nas bordas nem sob o HUD. */
#define BAG_MARGIN_X       30.0f
#define BAG_MARGIN_TOP     70.0f
#define BAG_MARGIN_BOTTOM  30.0f

static World world = {0};

/* Sorteia um float entre min e max. */
static float RandomFloat(float min, float max) {
    float t = (float)GetRandomValue(0, 1000) / 1000.0f;
    return min + (max - min) * t;
}

/* --- callbacks genericos --- */

/* ListRemoveIf: destroi o dado removido (1 argumento). */
static void DestroyEntity(void* data) {
    EntityDestroy((Entity*)data);
}

/* ListForEach: destroi todos os dados de uma vez (2 argumentos). */
static void DestroyEntityCallback(void* data, void* context) {
    (void)context;
    EntityDestroy((Entity*)data);
}

static void DrawEntityCallback(void* data, void* context) {
    (void)context;
    EntityDraw((Entity*)data);
}

/* Predicado: "essa entidade deve sair da lista?" */
static bool IsEntityInactive(void* data, void* context) {
    (void)context;
    return !((Entity*)data)->active;
}

/* --- IA do cachorro: achar a sacola mais proxima --- */

typedef struct {
    Vector2 origin;
    Entity* best;
    float   bestDistSq;
} NearestBagContext;

/* Comparamos distancia AO QUADRADO para evitar a raiz quadrada. */
static void FindNearestBagCallback(void* data, void* context) {
    Entity* e = (Entity*)data;
    NearestBagContext* ctx = (NearestBagContext*)context;

    if (!e->active || e->type != ENTITY_BAG) return;

    float dx = e->position.x - ctx->origin.x;
    float dy = e->position.y - ctx->origin.y;
    float distSq = dx * dx + dy * dy;

    if (distSq < ctx->bestDistSq) {
        ctx->bestDistSq = distSq;
        ctx->best = e;
    }
}

static Entity* FindNearestBag(List* entities, Vector2 origin) {
    NearestBagContext ctx = { origin, NULL, 1.0e30f };
    ListForEach(entities, FindNearestBagCallback, &ctx);
    return ctx.best;
}

/* --- atualizacao das entidades --- */

typedef struct {
    float deltaTime;
    List* entities;
    int*  score;
} UpdateContext;

/* O cachorro persegue a sacola mais proxima e a "come" se encostar (-1 ponto). */
static void UpdateDog(Entity* dog, float delta, List* entities, int* score) {
    Entity* target = FindNearestBag(entities, dog->position);
    if (target == NULL) {
        dog->velocity = (Vector2){0, 0};
        return;
    }

    /* Direcao do centro do cachorro ate o centro da sacola. */
    Vector2 dogCenter = { dog->position.x + dog->size.x * 0.5f,
                          dog->position.y + dog->size.y * 0.5f };
    Vector2 bagCenter = { target->position.x + target->size.x * 0.5f,
                          target->position.y + target->size.y * 0.5f };

    float dx = bagCenter.x - dogCenter.x;
    float dy = bagCenter.y - dogCenter.y;
    float len = sqrtf(dx * dx + dy * dy);

    if (len > 0.0f) {
        dog->velocity.x = dx / len * dog->speed;
        dog->velocity.y = dy / len * dog->speed;
    }

    EntityUpdate(dog, delta);

    /* Colisao cachorro x sacola (AABB). Marca inativa; a faxina remove. */
    Rectangle dogBox = { dog->position.x, dog->position.y, dog->size.x, dog->size.y };
    Rectangle bagBox = { target->position.x, target->position.y,
                         target->size.x, target->size.y };

    if (CheckCollisionRecs(dogBox, bagBox)) {
        target->active = false;
        if (score != NULL) (*score)--;
    }
}

static void UpdateEntityCallback(void* data, void* context) {
    Entity* entity = (Entity*)data;
    UpdateContext* ctx = (UpdateContext*)context;

    if (!entity->active) return;

    if (entity->type == ENTITY_DOG) {
        UpdateDog(entity, ctx->deltaTime, ctx->entities, ctx->score);
    } else {
        EntityUpdate(entity, ctx->deltaTime);
    }
}

/* --- colisao jogador x sacola --- */

typedef struct {
    Rectangle playerBox;
    int*      score;
    int*      bags;
} PlayerCollideContext;

static void PlayerBagCollideCallback(void* data, void* context) {
    Entity* entity = (Entity*)data;
    PlayerCollideContext* ctx = (PlayerCollideContext*)context;

    if (!entity->active || entity->type != ENTITY_BAG) return;

    Rectangle bagBox = { entity->position.x, entity->position.y,
                         entity->size.x, entity->size.y };

    if (!CheckCollisionRecs(ctx->playerBox, bagBox)) return;

    (*ctx->score)++;    /* +1 ponto            */
    (*ctx->bags)++;     /* +1 sacola coletada  */
    entity->active = false;
}

/* --- spawn --- */

static void SpawnBag(void) {
    float x = RandomFloat(BAG_MARGIN_X, SCREEN_W - BAG_MARGIN_X);
    float y = RandomFloat(BAG_MARGIN_TOP, SCREEN_H - BAG_MARGIN_BOTTOM);

    Entity* entity = EntityCreate(ENTITY_BAG, (Vector2){x, y});
    if (entity == NULL) return;

    entity->velocity = (Vector2){0, 0};   /* sacola parada */
    ListPushBack(world.entities, entity);
}

static void SpawnDog(void) {
    bool fromRight = (GetRandomValue(0, 1) == 0);
    float x = fromRight ? (float)(SCREEN_W + 40) : -40.0f;
    float y = RandomFloat(80.0f, SCREEN_H - 40.0f);

    Entity* entity = EntityCreate(ENTITY_DOG, (Vector2){x, y});
    if (entity == NULL) return;

    ListPushBack(world.entities, entity);
}

static void CountActiveDogsCallback(void* data, void* context) {
    Entity* e = (Entity*)data;
    if (e->active && e->type == ENTITY_DOG) {
        (*(int*)context)++;
    }
}

/* Ha algum cachorro em cena? (limite de 1 por vez) */
static bool HasActiveDog(List* entities) {
    int count = 0;
    ListForEach(entities, CountActiveDogsCallback, &count);
    return count > 0;
}

/* --- funcoes publicas --- */

void WorldInit(void) {
    world.entities = ListCreate();
    WorldReset();
}

void WorldReset(void) {
    if (world.entities == NULL) return;

    /* ListClear libera os NOS, entao destruimos os dados (Entity) antes. */
    ListForEach(world.entities, DestroyEntityCallback, NULL);
    ListClear(world.entities);

    world.score = 0;
    world.bagsCollected = 0;
    world.timeLeft = PHASE_DURATION;
    world.bagTimer = 0.3f;   /* primeira sacola no comeco */
    world.dogTimer = 2.0f;   /* primeiro cachorro apos 2s */
    world.finished = false;
    world.won = false;
}

void WorldUpdate(void) {
    if (world.finished) return;

    float delta = GetFrameTime();

    /* 1) tempo */
    world.timeLeft -= delta;
    if (world.timeLeft <= 0.0f) {
        world.timeLeft = 0.0f;
        world.finished = true;
        world.won = (world.score > 0);
        return;
    }

    /* 2) spawns */
    world.bagTimer -= delta;
    if (world.bagTimer <= 0.0f) {
        SpawnBag();
        world.bagTimer = BAG_INTERVAL;
    }

    world.dogTimer -= delta;
    if (world.dogTimer <= 0.0f) {
        if (!HasActiveDog(world.entities)) {
            SpawnDog();
        }
        world.dogTimer = DOG_INTERVAL;
    }

    /* 3) move/IA */
    UpdateContext uctx = { delta, world.entities, &world.score };
    ListForEach(world.entities, UpdateEntityCallback, &uctx);

    /* 4) colisao jogador x sacola */
    Player* p = PlayerGet();
    PlayerCollideContext pctx = {
        { p->position.x, p->position.y, p->size.x, p->size.y },
        &world.score,
        &world.bagsCollected
    };
    ListForEach(world.entities, PlayerBagCollideCallback, &pctx);

    /* 5) faxina: remove inativas (coletadas ou comidas) */
    ListRemoveIf(world.entities, IsEntityInactive, NULL, DestroyEntity);
}

void WorldDraw(void) {
    ListForEach(world.entities, DrawEntityCallback, NULL);
}

void WorldClose(void) {
    if (world.entities == NULL) return;
    ListForEach(world.entities, DestroyEntityCallback, NULL);
    ListDestroy(world.entities);
    world.entities = NULL;
}

int   WorldGetScore(void)         { return world.score; }
int   WorldGetBagsCollected(void) { return world.bagsCollected; }
float WorldGetTimeLeft(void)      { return world.timeLeft; }
bool  WorldIsFinished(void)       { return world.finished; }
bool  WorldDidWin(void)           { return world.won; }
