/*
 * world.c - Implementacao da Fase 1 - Rua Limpa.
 *
 * A ideia geral deste arquivo e:
 *   - ter UMA instancia global do World (static, privada);
 *   - a cada quadro (WorldUpdate): criar entidades novas, mover todas,
 *     checar colisao com o jogador e remover quem saiu de cena.
 *
 * O ponto pedagogico importante: usamos a lista encadeada generica + funcoes
 * de callback (ListForEach / ListRemoveIf) para processar as entidades sem
 * escrever um "for" cheio de detalhes de ponteiro aqui.
 */

#include "world.h"
#include "entity.h"    /* Entity, EntityCreate, EntityUpdate, EntityDraw, EntityDestroy */
#include "player.h"    /* PlayerGet, la de player.c                                     */

/* Dimensoes da janela (as mesmas definidas em game.c). */
#define SCREEN_W 800
#define SCREEN_H 450

/*
 * Instancia unica do mundo. "static" = privada deste arquivo.
 * O "{0}" zera todos os campos; a lista de verdade e criada no WorldInit.
 */
static World world = {0};

/* -------------------------------------------------------------------------
 * Funcoes auxiliares privadas (static): nao aparecem no world.h porque
 * ninguem de fora precisa chama-las.
 * ------------------------------------------------------------------------- */

/*
 * RandomFloat - sorteia um numero float entre "min" e "max".
 * GetRandomValue(0, 1000) da um inteiro de 0 a 1000; dividimos por 1000.0f
 * para virar um valor entre 0.0 e 1.0, e escalamos para o intervalo desejado.
 * O sufixo "f" indica que o numero e float (senao seria double).
 */
static float RandomFloat(float min, float max) {
    float t = (float)GetRandomValue(0, 1000) / 1000.0f;   /* fator de 0.0 a 1.0 */
    return min + (max - min) * t;
}

/*
 * DestroyEntity - "destructor" para uso com ListRemoveIf.
 * Recebe void* (generico) e converte para Entity* antes de destruir.
 * Precisa existir porque a lista nao sabe que tipo de dado ela guarda.
 */
static void DestroyEntity(void* data) {
    EntityDestroy((Entity*)data);
}

/* Versao de 2 argumentos, para usar no ListForEach (que passa um contexto). */
static void DestroyEntityCallback(void* data, void* context) {
    (void)context;
    EntityDestroy((Entity*)data);
}

/* Callback de desenho: so repassa para o EntityDraw. */
static void DrawEntityCallback(void* data, void* context) {
    (void)context;                 /* Nao usamos o contexto aqui; "(void)" evita aviso. */
    EntityDraw((Entity*)data);
}

/*
 * IsEntityInactive - PREDICADO usado pelo ListRemoveIf.
 * Retorna true para entidades que devem sair da lista (active == false).
 * Precisamos dessa funcao intermediaria porque o ListRemoveIf so entende
 * ponteiros genericos (void*).
 */
static bool IsEntityInactive(void* data, void* context) {
    (void)context;
    Entity* entity = (Entity*)data;
    return !entity->active;
}

/*
 * Contexto usado no callback de atualizacao.
 * E aqui que o parametro "void* context" do ListForEach brilha: ele permite
 * passar varias informacoes (delta, ponteiro para as vidas) sem variaveis globais.
 */
typedef struct {
    float deltaTime;   /* Segundos desde o ultimo quadro.   */
    int*  lives;       /* Ponteiro para world.lives.        */
} UpdateContext;

/*
 * UpdateEntityCallback - roda para cada entidade a cada quadro.
 * Move a entidade e, se ela saiu da tela, marca como inativa.
 * Uma sacola que caiu fora da tela custa 1 vida (o lixo "escapou").
 */
static void UpdateEntityCallback(void* data, void* context) {
    Entity* entity = (Entity*)data;
    UpdateContext* ctx = (UpdateContext*)context;

    if (!entity->active) return;

    EntityUpdate(entity, ctx->deltaTime);   /* Move conforme a velocidade. */

    /* A entidade saiu da tela? (caiu embaixo ou saiu pelas laterais) */
    bool leftScreen =
        (entity->position.y > SCREEN_H + entity->size.y) ||
        (entity->position.x + entity->size.x < 0) ||
        (entity->position.x > SCREEN_W);

    if (leftScreen) {
        if (entity->type == ENTITY_BAG) {
            (*ctx->lives)--;        /* Sacola escapou: perdeu uma vida. */
        }
        entity->active = false;     /* Sera removida no final do quadro. */
    }
}

/*
 * Contexto da colisao com o jogador.
 */
typedef struct {
    Rectangle playerBox;    /* Retangulo do jogador.            */
    int*  score;            /* Ponteiro para world.score.       */
    int*  lives;            /* Ponteiro para world.lives.       */
    float* invulnTimer;     /* Ponteiro para o tempo de invuln. */
} CollideContext;

/*
 * CollideEntityCallback - checa se a entidade encostou no jogador.
 *   - BAG  (sacola): +10 pontos e some.
 *   - DOG  (cachorro): -1 vida (se nao estiver invulneravel) e some.
 */
static void CollideEntityCallback(void* data, void* context) {
    Entity* entity = (Entity*)data;
    CollideContext* ctx = (CollideContext*)context;

    if (!entity->active) return;

    /* Monta o retangulo da entidade (posicao + tamanho). */
    Rectangle entityBox = { entity->position.x, entity->position.y,
                            entity->size.x, entity->size.y };

    /* CheckCollisionRecs = colisao AABB (retangulo x retangulo) do Raylib. */
    if (!CheckCollisionRecs(ctx->playerBox, entityBox)) return;

    if (entity->type == ENTITY_BAG) {
        *ctx->score += 10;          /* Coletou o lixo.          */
        entity->active = false;     /* Sacola coletada: some.   */
    } else if (entity->type == ENTITY_DOG) {
        if (*ctx->invulnTimer <= 0.0f) {
            (*ctx->lives)--;            /* Levou uma mordida.       */
            *ctx->invulnTimer = 1.0f;   /* Fica invulneravel 1s.    */
        }
        entity->active = false;     /* O cachorro vai embora apos o contato. */
    }
}

/*
 * SpawnEntity - cria uma entidade aleatoria no topo/lateral da tela.
 * 70% de chance de sacola, 30% de cachorro.
 */
static void SpawnEntity(void) {
    int roll = GetRandomValue(0, 99);                 /* 0..99 */
    EntityType type = (roll < 70) ? ENTITY_BAG : ENTITY_DOG;

    float x, y;
    bool fromRight = false;

    if (type == ENTITY_BAG) {
        /* Sacola cai do topo, em um X aleatorio dentro da tela. */
        x = RandomFloat(20.0f, (float)(SCREEN_W - 40));
        y = -20.0f;
    } else {
        /* Cachorro entra pela esquerda OU pela direita, na altura do chao. */
        fromRight = (GetRandomValue(0, 1) == 0);
        x = fromRight ? (float)(SCREEN_W + 40) : -40.0f;
        y = 400.0f;   /* alinhado com o corpo do jogador (380..420) */
    }

    Entity* entity = EntityCreate(type, (Vector2){x, y});
    if (entity == NULL) return;   /* Sem memoria: simplesmente nao cria. */

    /* O cachorro ja nasce com velocidade.x = -40 (para a esquerda).
     * Se ele veio da esquerda, invertemos o sentido. */
    if (type == ENTITY_DOG && !fromRight) {
        entity->velocity.x = entity->speed;   /* agora anda para a direita */
    }

    ListPushBack(world.entities, entity);     /* Entra na lista encadeada. */
}

/* -------------------------------------------------------------------------
 * Funcoes publicas
 * ------------------------------------------------------------------------- */

/*
 * WorldInit - cria a lista e zera o estado. Chamada uma vez, no inicio.
 */
void WorldInit(void) {
    world.entities = ListCreate();   /* Lista vazia. */
    WorldReset();                    /* Zera pontos, vidas etc. */
}

/*
 * WorldReset - reinicia a fase: remove todas as entidades da lista e
 * restaura os valores iniciais. Usada ao (re)comecar um jogo.
 */
void WorldReset(void) {
    if (world.entities == NULL) return;

    /* ListClear libera os NOS da lista, mas NAO o dado de dentro (o Entity).
     * Por isso primeiro destruimos cada entidade, depois limpamos a lista. */
    ListForEach(world.entities, DestroyEntityCallback, NULL);
    ListClear(world.entities);

    world.score = 0;
    world.lives = 3;
    world.spawnTimer = 0.5f;      /* primeira entidade aparece rapido */
    world.spawnInterval = 1.0f;   /* depois, 1 por segundo */
    world.invulnTimer = 0.0f;
    world.gameOver = false;
}

/*
 * WorldUpdate - logica de um quadro. Ordem:
 *   1) baixar timers (spawn e invulnerabilidade);
 *   2) criar entidade se for hora;
 *   3) mover todas;
 *   4) checar colisao com o jogador;
 *   5) remover as inativas;
 *   6) verificar fim de jogo.
 */
void WorldUpdate(void) {
    if (world.gameOver) return;

    float delta = GetFrameTime();

    if (world.invulnTimer > 0.0f) {
        world.invulnTimer -= delta;
        if (world.invulnTimer < 0.0f) world.invulnTimer = 0.0f;
    }

    world.spawnTimer -= delta;
    if (world.spawnTimer <= 0.0f) {
        SpawnEntity();
        world.spawnTimer = world.spawnInterval;

        /* Dificuldade progressiva: intervalos menores (ate 0.4s). */
        if (world.spawnInterval > 0.4f) {
            world.spawnInterval -= 0.02f;
        }
    }

    /* 3) mover todas as entidades. */
    UpdateContext uctx = { delta, &world.lives };
    ListForEach(world.entities, UpdateEntityCallback, &uctx);

    /* 4) colisao com o jogador. */
    Player* p = PlayerGet();
    CollideContext cctx = {
        { p->position.x, p->position.y, p->size.x, p->size.y },
        &world.score,
        &world.lives,
        &world.invulnTimer
    };
    ListForEach(world.entities, CollideEntityCallback, &cctx);

    /* 5) remover tudo o que ficou inativo (coletado, saiu da tela ou mordeu).
     * ListRemoveIf percorre a lista, remove os nos cujo predicado for true
     * e chama DestroyEntity em cada dado removido (evita vazamento de memoria). */
    ListRemoveIf(world.entities, IsEntityInactive, NULL, DestroyEntity);

    /* 6) fim de jogo: acabaram as vidas. */
    if (world.lives <= 0) {
        world.lives = 0;
        world.gameOver = true;
    }
}

/*
 * WorldDraw - desenha todas as entidades da lista.
 */
void WorldDraw(void) {
    ListForEach(world.entities, DrawEntityCallback, NULL);
}

/*
 * WorldClose - libera todas as entidades e a lista. Chamada ao fechar o jogo.
 */
void WorldClose(void) {
    if (world.entities == NULL) return;
    ListForEach(world.entities, DestroyEntityCallback, NULL);   /* Destroi cada Entity. */
    ListDestroy(world.entities);                        /* Libera os nos e a lista. */
    world.entities = NULL;
}

int WorldGetScore(void) { return world.score; }
int WorldGetLives(void) { return world.lives; }
bool WorldIsGameOver(void) { return world.gameOver; }
