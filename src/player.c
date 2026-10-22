// player.c - logica e desenho do jogador.

#include "player.h"
#include "config.h"
#include "textures.h"
#include <math.h>   // sqrtf, usado na normalizacao da diagonal

// O jogador e unico e privado deste arquivo.
static Player player = {0};

#define PLAYER_FRAME_TIME  0.10f   // segundos por quadro
#define PLAYER_DRAW_SCALE  3.0f    // 32 * 3 = 96x96 (maior que o cachorro)

void PlayerInit(void) {
    player.size = (Vector2){ PLAYER_FRAME_W * PLAYER_DRAW_SCALE,
                             PLAYER_FRAME_H * PLAYER_DRAW_SCALE };
    player.position = (Vector2){
        (SCREEN_W - player.size.x) / 2.0f,   // centralizado na largura
        GROUND_Y - player.size.y             // em pe sobre a calcada
    };
    player.velocity = (Vector2){0, 0};
    player.color = BLUE;
    player.speed = 300.0f;                   // pixels por segundo
    player.frame = 0;
    player.frameTimer = 0.0f;
    player.facingLeft = false;
}

void PlayerUpdate(void) {
    float delta = GetFrameTime();

    // Le as teclas e monta a direcao. No Raylib, y negativo sobe.
    Vector2 dir = {0, 0};
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))  dir.x -= 1;
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) dir.x += 1;
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))    dir.y -= 1;
    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))  dir.y += 1;

    if (dir.x != 0 || dir.y != 0) {
        // Normaliza: sem isso a diagonal (1,1) seria ~41% mais rapida.
        // Dividir pelo comprimento faz o vetor ter tamanho 1.
        float length = sqrtf(dir.x * dir.x + dir.y * dir.y);
        dir.x /= length;
        dir.y /= length;

        // Multiplicar por delta deixa a velocidade independente do FPS.
        player.position.x += dir.x * player.speed * delta;
        player.position.y += dir.y * player.speed * delta;

        if (dir.x >  0) player.facingLeft = false;
        if (dir.x <  0) player.facingLeft = true;

        // Anima so enquanto anda.
        player.frameTimer += delta;
        if (player.frameTimer >= PLAYER_FRAME_TIME) {
            player.frameTimer -= PLAYER_FRAME_TIME;
            player.frame = (player.frame + 1) % PLAYER_FRAME_COUNT;
        }
    } else {
        // parado: volta pra pose inicial (quadro 0)
        player.frameTimer = 0.0f;
        player.frame = 0;
    }

    // Limita o jogador dentro da tela.
    if (player.position.x < 0) player.position.x = 0;
    if (player.position.x + player.size.x > SCREEN_W) player.position.x = SCREEN_W - player.size.x;

    if (player.position.y < 0) player.position.y = 0;
    if (player.position.y + player.size.y > SCREEN_H) player.position.y = SCREEN_H - player.size.y;
}

void PlayerDraw(void) {
    if (TexturesPlayerLoaded()) {
        // quadro atual da folha, esticado para caber na hitbox
        Rectangle source = {
            player.facingLeft ? (float)PLAYER_FRAME_W : -(float)PLAYER_FRAME_W,
            (float)(player.frame * PLAYER_FRAME_H),
            (float)PLAYER_FRAME_W,
            (float)PLAYER_FRAME_H
        };
        Rectangle dest = { player.position.x, player.position.y,
                           player.size.x, player.size.y };

        DrawTexturePro(TexturesGetPlayer(), source, dest, (Vector2){0, 0}, 0.0f, WHITE);
        return;
    }

    // reserva: retangulos, caso a imagem nao carregue
    DrawRectangleV(player.position, player.size, player.color);                       // corpo
    DrawRectangle(player.position.x + 5, player.position.y - 10, 20, 10, DARKBLUE);   // cabeca
}

void PlayerClose(void) {
    // nada alocado por enquanto
}

Player* PlayerGet(void) {
    return &player;
}
