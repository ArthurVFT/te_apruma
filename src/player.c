// player.c - logica e desenho do jogador.

#include "player.h"
#include "config.h"
#include <math.h>   // sqrtf, usado na normalizacao da diagonal

// O jogador e unico e privado deste arquivo.
static Player player = {0};

void PlayerInit(void) {
    player.size = (Vector2){30, 40};
    player.position = (Vector2){
        (SCREEN_W - player.size.x) / 2.0f,   // centralizado na largura
        GROUND_Y - player.size.y             // em pe sobre a calcada
    };
    player.velocity = (Vector2){0, 0};
    player.color = BLUE;
    player.speed = 300.0f;                   // pixels por segundo
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
    }

    // Limita o jogador dentro da tela.
    if (player.position.x < 0) player.position.x = 0;
    if (player.position.x + player.size.x > SCREEN_W) player.position.x = SCREEN_W - player.size.x;

    if (player.position.y < 0) player.position.y = 0;
    if (player.position.y + player.size.y > SCREEN_H) player.position.y = SCREEN_H - player.size.y;
}

void PlayerDraw(void) {
    DrawRectangleV(player.position, player.size, player.color);                       // corpo
    DrawRectangle(player.position.x + 5, player.position.y - 10, 20, 10, DARKBLUE);   // cabeca
}

void PlayerClose(void) {
    // nada alocado por enquanto
}

Player* PlayerGet(void) {
    return &player;
}
