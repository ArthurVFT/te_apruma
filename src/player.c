#include "player.h"

static Player player = {0};

void PlayerInit(void) {
    player.position = (Vector2){400, 380};
    player.size = (Vector2){30, 40};
    player.velocity = (Vector2){0, 0};
    player.color = BLUE;
    player.speed = 300.0f;
}

void PlayerUpdate(void) {
    float delta = GetFrameTime();
    
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
        player.position.x -= player.speed * delta;
    }
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
        player.position.x += player.speed * delta;
    }
    
    // Limitar nas bordas
    if (player.position.x < 0) player.position.x = 0;
    if (player.position.x + player.size.x > 800) player.position.x = 800 - player.size.x;
}

void PlayerDraw(void) {
    DrawRectangleV(player.position, player.size, player.color);
    DrawRectangle(player.position.x + 5, player.position.y - 10, 20, 10, DARKBLUE); // Cabeça
}

void PlayerClose(void) {
    // Nada para liberar por enquanto
}

Player* PlayerGet(void) {
    return &player;
}