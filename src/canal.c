#include "canal.h"
#include "config.h"
#include "entity.h"
#include "player.h"
#include <math.h>   

#define PHASE_DURATION   30.0f

static Canal canal = {0};


static float RandomFloat(float min, float max) {
    float t = (float)GetRandomValue(0, 1000) / 1000.0f;
    return min + (max - min) * t;
}


static float PhaseProgress(void) {
    float p = 1.0f - (canal.timeLeft / PHASE_DURATION);
    if (p < 0.0f) p = 0.0f;
    if (p > 1.0f) p = 1.0f;
    return p;
}

// --- callbacks genericos ---


static void DestroyEntity(void* data) {
    EntityDestroy((Entity*)data);
}

static void DestroyEntityCallback(void* data, void* context) {
    (void)context;
    EntityDestroy((Entity*)data);
}

static void DrawEntityCallback(void* data, void* context) {
    (void)context;
    EntityDraw((Entity*)data);
}

static bool IsEntityInactive(void* data, void* context) {
    (void)context;
    return !((Entity*)data)->active;
}

// --- atualizacao das entidades ---

typedef struct {
    float deltaTime;
    List* entities;
    int*  score;
    float progress;  
} UpdateContext;

static void UpdateEntityCallback(void* data, void* context) {
    Entity* entity = (Entity*)data;
    UpdateContext* ctx = (UpdateContext*)context;

    if (!entity->active) return;

    EntityUpdate(entity, ctx->deltaTime);
}

// --- colisao jogador x sacola ---
// (amanha: adaptar para jogador x pessoa)

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

    (*ctx->score)++;    // +1 ponto
    (*ctx->bags)++;     // +1 sacola coletada
    entity->active = false;
}



static void CountActiveDogsCallback(void* data, void* context) {
    Entity* e = (Entity*)data;
    if (e->active && e->type == ENTITY_DOG) {
        (*(int*)context)++;
    }
}

static int CountActiveDogs(List* entities) {
    int count = 0;
    ListForEach(entities, CountActiveDogsCallback, &count);
    return count;
}

// --- funcoes publicas ---

void CanalInit(void) {
    canal.entities = ListCreate();
    CanalReset();
}

void CanalReset(void) {
    if (canal.entities == NULL) return;
    
    ListForEach(canal.entities, DestroyEntityCallback, NULL);
    ListClear(canal.entities);

    canal.score = 0;
    canal.timeLeft = PHASE_DURATION;
    canal.finished = false;
    canal.won = false;
    canal.peopleRescued = 0;
    canal.peopleInWater = 0;
    canal.personTimer = 1.0f;

    
}

void CanalUpdate(void) {
    if (canal.finished) return;

    float delta = GetFrameTime();

    canal.timeLeft -= delta;
    if (canal.timeLeft <= 0.0f) {
        canal.timeLeft = 0.0f;
        canal.finished = true;
        canal.won = (canal.score > 0);
        return;
    }
    
    float progress = PhaseProgress();
    UpdateContext uctx = { delta, canal.entities, &canal.score, progress };
    ListForEach(canal.entities, UpdateEntityCallback, &uctx);
 
    ListRemoveIf(canal.entities, IsEntityInactive, NULL, DestroyEntity);
}

void CanalDraw(void) {

    DrawRectangle(0, 300, SCREEN_W, 120, BLUE);

    ListForEach(canal.entities, DrawEntityCallback, NULL);
    
}

void CanalClose(void) {
    if (canal.entities == NULL) return;
    ListForEach(canal.entities, DestroyEntityCallback, NULL);
    ListDestroy(canal.entities);
    canal.entities = NULL;
}

int   CanalGetScore(void)         { return canal.score; }
int   CanalGetPeopleRescued(void) { return canal.peopleRescued; }
float CanalGetTimeLeft(void)      { return canal.timeLeft; }
bool  CanalIsFinished(void)       { return canal.finished; }
bool  CanalDidWin(void)           { return canal.won; }