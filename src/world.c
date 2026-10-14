/*
 * world.c - Fase 1 - Rua Limpa (coleta de sacolas por tempo).
 *
 * Regras da fase:
 *   - As sacolas NAO caem: aparecem em posicoes aleatorias e ficam paradas.
 *   - O jogador coleta uma sacola encostando nela -> +1 ponto.
 *   - Os cachorros perseguem a sacola mais proxima; se um cachorro alcancar
 *     uma sacola antes do jogador, ela some e vale -1 ponto.
 *   - A fase tem um TEMPO. Ao acabar, o jogador vence se a pontuacao for > 0.
 *
 * Ponto pedagogico: usamos a lista encadeada generica + callbacks
 * (ListForEach / ListRemoveIf) para processar as entidades.
 */

#include "world.h"
#include "config.h"    /* SCREEN_W / SCREEN_H (tamanho da janela). */
#include "entity.h"    /* Entity, EntityCreate, EntityUpdate, EntityDraw, EntityDestroy */
#include "player.h"    /* PlayerGet, la de player.c                                     */
#include <math.h>      /* sqrtf - usada na direcao do cachorro.                          */

/* Ajustes de dificuldade - mexa aqui para balancear a fase. */
#define PHASE_DURATION   30.0f   /* Duracao da fase, em segundos.              */
#define BAG_INTERVAL      1.0f   /* A cada quantos segundos nasce uma sacola.  */
#define DOG_INTERVAL      6.0f   /* A cada quantos segundos nasce um cachorro. */

/* Margens para as sacolas nao nascerem coladas nas bordas / debaixo do HUD. */
#define BAG_MARGIN_X       30.0f
#define BAG_MARGIN_TOP     70.0f   /* abaixo do placar (Pontos/Tempo). */
#define BAG_MARGIN_BOTTOM  30.0f

/*
 * Instancia unica do mundo. "static" = privada deste arquivo.
 */
static World world = {0};

/* -------------------------------------------------------------------------
 * Funcoes auxiliares privadas (static)
 * ------------------------------------------------------------------------- */

/* Sorteia um float entre "min" e "max". */
static float RandomFloat(float min, float max) {
    float t = (float)GetRandomValue(0, 1000) / 1000.0f;   /* fator 0.0 .. 1.0 */
    return min + (max - min) * t;
}

/* Destructor de 1 argumento, para o ListRemoveIf. */
static void DestroyEntity(void* data) {
    EntityDestroy((Entity*)data);
}

/* Versao de 2 argumentos, para o ListForEach. */
static void DestroyEntityCallback(void* data, void* context) {
    (void)context;
    EntityDestroy((Entity*)data);
}

/* Desenha a entidade da vez. */
static void DrawEntityCallback(void* data, void* context) {
    (void)context;
    EntityDraw((Entity*)data);
}

/* Predicado do ListRemoveIf: "essa entidade deve sair da lista?" */
static bool IsEntityInactive(void* data, void* context) {
    (void)context;
    return !((Entity*)data)->active;
}

/* -------------------------------------------------------------------------
 * Busca da sacola mais proxima (usada pela IA do cachorro).
 * ------------------------------------------------------------------------- */

typedef struct {
    Vector2 origin;       /* De onde estamos medindo a distancia (o cachorro). */
    Entity* best;         /* Melhor candidata encontrada ate agora.            */
    float   bestDistSq;   /* Distancia AO QUADRADO da melhor (evita sqrt).     */
} NearestBagContext;

/*
 * Compara a distancia ao quadrado. Usar o quadrado e so uma otimizacao:
 * se A^2 < B^2 entao A < B (distancias nao sao negativas), e evitamos a raiz.
 */
static void FindNearestBagCallback(void* data, void* context) {
    Entity* e = (Entity*)data;
    NearestBagContext* ctx = (NearestBagContext*)context;

    if (!e->active || e->type != ENTITY_BAG) return;   /* So sacolas ativas contam. */

    float dx = e->position.x - ctx->origin.x;
    float dy = e->position.y - ctx->origin.y;
    float distSq = dx * dx + dy * dy;

    if (distSq < ctx->bestDistSq) {
        ctx->bestDistSq = distSq;
        ctx->best = e;
    }
}

/* Devolve a sacola ativa mais proxima de "origin", ou NULL se nao houver. */
static Entity* FindNearestBag(List* entities, Vector2 origin) {
    NearestBagContext ctx = { origin, NULL, 1.0e30f };   /* começa com distancia gigante */
    ListForEach(entities, FindNearestBagCallback, &ctx);
    return ctx.best;
}

/* -------------------------------------------------------------------------
 * Atualizacao das entidades
 * ------------------------------------------------------------------------- */

typedef struct {
    float deltaTime;   /* Segundos desde o quadro anterior. */
    List* entities;    /* A lista, para o cachorro achar a sacola. */
    int*  score;       /* Ponteiro para world.score. */
} UpdateContext;

/*
 * UpdateDog - a IA do cachorro:
 *   1) acha a sacola mais proxima;
 *   2) aponta a velocidade na direcao dela e se move;
 *   3) se encostar na sacola, o cachorro a "come": -1 ponto e ela some.
 */
static void UpdateDog(Entity* dog, float delta, List* entities, int* score) {
    Entity* target = FindNearestBag(entities, dog->position);

    if (target == NULL) {
        dog->velocity = (Vector2){0, 0};   /* Sem sacola: fica parado. */
        return;
    }

    /* Centro do cachorro e centro da sacola (colisao/movimento pelo centro). */
    Vector2 dogCenter = { dog->position.x + dog->size.x * 0.5f,
                          dog->position.y + dog->size.y * 0.5f };
    Vector2 bagCenter = { target->position.x + target->size.x * 0.5f,
                          target->position.y + target->size.y * 0.5f };

    float dx = bagCenter.x - dogCenter.x;
    float dy = bagCenter.y - dogCenter.y;
    float len = sqrtf(dx * dx + dy * dy);

    /* Vetor unitario (dx/len, dy/len) * velocidade = anda na direcao da sacola. */
    if (len > 0.001f) {
        dog->velocity.x = (dx / len) * dog->speed;
        dog->velocity.y = (dy / len) * dog->speed;
    }

    EntityUpdate(dog, delta);   /* Aplica a velocidade (move). */

    /* ==================== === COLISAO === (cachorro x sacola) =============
     * Usa o retangulo do cachorro contra o da sacola-alvo. Encostou = comeu.
     * Nota: aqui so MARCAMOS a sacola como inativa; quem remove da lista e o
     * ListRemoveIf no fim do WorldUpdate (nao se pode remover durante o for). */
    Rectangle dogBox = { dog->position.x, dog->position.y, dog->size.x, dog->size.y };
    Rectangle bagBox = { target->position.x, target->position.y,
                         target->size.x, target->size.y };

    if (CheckCollisionRecs(dogBox, bagBox)) {
        target->active = false;              /* sacola "pega" pelo cachorro */
        if (score != NULL) (*score)--;       /* -1 ponto                    */
    }
}

/*
 * UpdateEntityCallback - roda para cada entidade a cada quadro.
 * Cachorro usa IA propria; os demais apenas se movem pela velocidade.
 */
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

/* -------------------------------------------------------------------------
 * Colisao jogador x sacola
 * ------------------------------------------------------------------------- */

typedef struct {
    Rectangle playerBox;   /* Retangulo do jogador.      */
    int*      score;       /* Ponteiro para world.score. */
} PlayerCollideContext;

/* === COLISAO (jogador x sacola) ===
 * Se o jogador encostar numa sacola: +1 ponto e a sacola some. */
static void PlayerBagCollideCallback(void* data, void* context) {
    Entity* entity = (Entity*)data;
    PlayerCollideContext* ctx = (PlayerCollideContext*)context;

    if (!entity->active || entity->type != ENTITY_BAG) return;

    Rectangle bagBox = { entity->position.x, entity->position.y,
                         entity->size.x, entity->size.y };

    /* CheckCollisionRecs = colisao AABB (retangulo x retangulo) do Raylib. */
    if (!CheckCollisionRecs(ctx->playerBox, bagBox)) return;

    *ctx->score += 1;         /* Coletou a sacola: +1 ponto. */
    entity->active = false;   /* Sera removida na faxina do fim do quadro. */
}

/* -------------------------------------------------------------------------
 * Criacao de entidades (spawn)
 * ------------------------------------------------------------------------- */

/* Nasce uma sacola numa posicao aleatoria da area de jogo (ela nao cai). */
static void SpawnBag(void) {
    float x = RandomFloat(BAG_MARGIN_X, SCREEN_W - BAG_MARGIN_X);
    float y = RandomFloat(BAG_MARGIN_TOP, SCREEN_H - BAG_MARGIN_BOTTOM);

    Entity* entity = EntityCreate(ENTITY_BAG, (Vector2){x, y});
    if (entity == NULL) return;

    entity->velocity = (Vector2){0, 0};   /* Sacola PARADA (nao cai). */
    ListPushBack(world.entities, entity);
}

/* Nasce um cachorro numa das laterais; ele mesmo vai correr atras das sacolas. */
static void SpawnDog(void) {
    bool fromRight = (GetRandomValue(0, 1) == 0);
    float x = fromRight ? (float)(SCREEN_W + 40) : -40.0f;
    float y = RandomFloat(80.0f, SCREEN_H - 40.0f);

    Entity* entity = EntityCreate(ENTITY_DOG, (Vector2){x, y});
    if (entity == NULL) return;

    ListPushBack(world.entities, entity);
}

/*
 * Conta quantos cachorros ATIVOS existem na lista.
 * Usa um ponteiro para int como contexto (o "lugar" onde somamos).
 */
static void CountActiveDogsCallback(void* data, void* context) {
    Entity* e = (Entity*)data;
    if (e->active && e->type == ENTITY_DOG) {
        (*(int*)context)++;
    }
}

/* Ha algum cachorro em cena? (limite de 1 cachorro por vez) */
static bool HasActiveDog(List* entities) {
    int count = 0;
    ListForEach(entities, CountActiveDogsCallback, &count);
    return count > 0;
}

/* -------------------------------------------------------------------------
 * Funcoes publicas
 * ------------------------------------------------------------------------- */

void WorldInit(void) {
    world.entities = ListCreate();   /* Lista vazia. */
    WorldReset();                    /* Zera pontos, tempo etc. */
}

/*
 * WorldReset - reinicia a fase: limpa entidades e restaura os valores iniciais.
 */
void WorldReset(void) {
    if (world.entities == NULL) return;

    /* ListClear libera os NOS, mas nao o dado (o Entity). Por isso destruimos
     * cada entidade antes de limpar a lista. */
    ListForEach(world.entities, DestroyEntityCallback, NULL);
    ListClear(world.entities);

    world.score = 0;
    world.timeLeft = PHASE_DURATION;
    world.bagTimer = 0.3f;    /* primeira sacola logo no comeco */
    world.dogTimer = 2.0f;    /* primeiro cachorro depois de 2s */
    world.finished = false;
    world.won = false;
}

/*
 * WorldUpdate - logica de um quadro:
 *   1) desconta o tempo; se acabou, decide vitoria/derrota;
 *   2) cria sacolas e cachorros nos intervalos;
 *   3) move/IA de todas as entidades;
 *   4) colisao jogador x sacola;
 *   5) remove as inativas.
 */
void WorldUpdate(void) {
    if (world.finished) return;

    float delta = GetFrameTime();

    /* 1) Tempo da fase. */
    world.timeLeft -= delta;
    if (world.timeLeft <= 0.0f) {
        world.timeLeft = 0.0f;
        world.finished = true;
        world.won = (world.score > 0);   /* Vence com score POSITIVO. */
        return;
    }

    /* 2) Spawns. */
    world.bagTimer -= delta;
    if (world.bagTimer <= 0.0f) {
        SpawnBag();
        world.bagTimer = BAG_INTERVAL;
    }

    world.dogTimer -= delta;
    if (world.dogTimer <= 0.0f) {
        /* Limite: no maximo 1 cachorro em cena por vez. */
        if (!HasActiveDog(world.entities)) {
            SpawnDog();
        }
        world.dogTimer = DOG_INTERVAL;
    }

    /* 3) atualiza entidades (o cachorro caça a sacola mais proxima). */
    UpdateContext uctx = { delta, world.entities, &world.score };
    ListForEach(world.entities, UpdateEntityCallback, &uctx);

    /* 4) === COLISAO (jogador x sacola) === */
    Player* p = PlayerGet();
    PlayerCollideContext pctx = {
        { p->position.x, p->position.y, p->size.x, p->size.y },
        &world.score
    };
    ListForEach(world.entities, PlayerBagCollideCallback, &pctx);

    /* 5) faxina: remove tudo que ficou inativo (sacola coletada/comida). */
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

int   WorldGetScore(void)    { return world.score; }
float WorldGetTimeLeft(void) { return world.timeLeft; }
bool  WorldIsFinished(void)  { return world.finished; }
bool  WorldDidWin(void)      { return world.won; }
