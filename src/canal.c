//
//Fase 2 - Canal.
//
// Regras:
// - Pessoas surgem em cima ou embaixo da tela e correm em direcao ao canal.
// - O agente encosta nelas antes de chegarem na agua -> +1 ponto.
// - Cada pessoa que chega no canal conta como "na agua" (acumulado).
// - Com 6 pessoas na agua, a fase e perdida na hora.
// - Se o tempo acabar com menos de 6, a fase e vencida.
// - Com o tempo, as pessoas aparecem mais seguido e mais rapidas.
//


#include "canal.h"
#include "config.h"
#include "entity.h"
#include "player.h"

#define PHASE_DURATION   30.0f

#define CANAL_Y  300
#define CANAL_HEIGHT 120

#define PERSON_BASE_SPEED 40.0f
#define PERSON_SPEED_GAIN 80.0f
#define PERSON_SPAWN_BASE 2.0f
#define PERSON_SPAWN_FAST 0.7f

#define WATER_LIMIT 6


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

static void SpawnPerson(float speed){
    bool fromTop = (GetRandomValue(0, 1) == 0);
    float x = RandomFloat(30.0f, SCREEN_W - 50.0f);
    float y  = fromTop ? - 40.0f : (float)SCREEN_H;

    Entity* person = EntityCreate(ENTITY_PERSON, (Vector2){ x, y });
    if (person == NULL) return;

    person->speed = speed;
    person->velocity = (Vector2){0,fromTop?speed:-speed};
    ListPushBack(canal.entities, person);
}

// --- atualizacao das entidades ---

typedef struct {
    float deltaTime;
    List* entities;
    int*  score;
    float progress; 
    int *water; 
} UpdateContext;

static void UpdateEntityCallback(void* data, void* context) {
    Entity* entity = (Entity*)data;
    UpdateContext* ctx = (UpdateContext*)context;

    if (!entity->active) return;

    EntityUpdate(entity, ctx->deltaTime);

    // pessoa entrou no canal
    Rectangle personBox = { entity->position.x, entity->position.y,
                            entity->size.x, entity->size.y };
    Rectangle canalBox  = { 0, CANAL_Y, SCREEN_W, CANAL_HEIGHT };

    if (CheckCollisionRecs(personBox, canalBox)) {
        entity->active = false;   
        (*ctx->water)++;          
    }
}

// colisão agente x pessoa
typedef struct {
    Rectangle playerBox;
    int*      score;
    int*      rescued;
} PlayerCollideContext;


static void PlayerPersonCollideCallback(void* data, void* context) {
    Entity* entity = (Entity*)data;
    PlayerCollideContext* ctx = (PlayerCollideContext*)context;

    if (!entity->active || entity->type != ENTITY_PERSON) return;

    Rectangle personBox = { entity->position.x, entity->position.y,
                            entity->size.x, entity->size.y };

    if (!CheckCollisionRecs(ctx->playerBox, personBox)) return;

    (*ctx->score)++;      // +1 ponto
    (*ctx->rescued)++;    // +1 pessoa resgatada
    entity->active = false;
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
        canal.won = (canal.peopleInWater < WATER_LIMIT);
        return;
    }

    float progress = PhaseProgress();
    canal.personTimer -= delta;
    if (canal.personTimer <= 0.0f) {
        float speed = PERSON_BASE_SPEED + PERSON_SPEED_GAIN * progress;
        SpawnPerson(speed);
        canal.personTimer = PERSON_SPAWN_BASE + (PERSON_SPAWN_FAST - PERSON_SPAWN_BASE) * progress;
    }

    UpdateContext uctx = { delta, canal.entities, &canal.score, progress, &canal.peopleInWater };
    ListForEach(canal.entities, UpdateEntityCallback, &uctx);

    if (canal.peopleInWater >= WATER_LIMIT) {
        canal.finished = true;
        canal.won = false;
        return;
    }

    Player* p = PlayerGet();
    PlayerCollideContext pctx = {
        { p->position.x, p->position.y, p->size.x, p->size.y },
        &canal.score,
        &canal.peopleRescued
    };
    ListForEach(canal.entities, PlayerPersonCollideCallback, &pctx);

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
int CanalGetPeopleInWater(void)   { return canal.peopleInWater; }