/*
 * player.c - Implementacao do jogador.
 */

#include "player.h"
#include "config.h"  /* SCREEN_W / SCREEN_H / GROUND_Y. */
#include <math.h>    /* sqrtf (raiz quadrada) - usada na normalizacao da diagonal. */

/*
 * Unica instancia do jogador no jogo.
 * "static" deixa a variavel visivel so dentro deste arquivo.
 * O "{0}" zera todos os campos (posicao 0,0, speed 0.0 etc.) por seguranca.
 */
static Player player = {0};

/*
 * PlayerInit - define os valores iniciais.
 * (Vector2){400, 380} e uma forma de "montar" um Vector2 na hora,
 * chamada de "compound literal": cria um valor temporario e copia pra variavel.
 */
void PlayerInit(void) {
    player.size = (Vector2){30, 40};         /* 30 pixels de largura, 40 de altura.       */
    player.position = (Vector2){             /* Comeca centralizado e em cima da calcada. */
        (SCREEN_W - player.size.x) / 2.0f,   /* centro horizontal                         */
        GROUND_Y - player.size.y             /* em pe sobre o chao                        */
    };
    player.velocity = (Vector2){0, 0};       /* Parado.                                   */
    player.color = BLUE;                     /* Cor azul.                                 */
    player.speed = 300.0f;                   /* 300 pixels por segundo. O "f" indica float. */
}

/*
 * PlayerUpdate - chamada a cada quadro enquanto o jogo esta em PLAYING.
 * Move o jogador nos dois eixos (horizontal e vertical) conforme as teclas.
 *
 * Truque importante do Raylib: o eixo Y cresce PARA BAIXO.
 *   - SUBIR  = diminuir y (KEY_UP / W)
 *   - DESCER = aumentar y (KEY_DOWN / S)
 */
void PlayerUpdate(void) {
    float delta = GetFrameTime();   /* Segundos desde o quadro anterior (ex.: ~0.0166 a 60 FPS). */

    /*
     * Montamos um "vetor de direcao" lido das teclas. Ele comeca em (0,0)
     * (parado) e cada tecla soma/subtrai 1 em um dos eixos.
     *
     * IsKeyDown = tecla segurada (movimento continuo).
     * Horizontal: setas ou A/D.  Vertical: setas ou W/S.
     */
    Vector2 dir = {0, 0};
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))  dir.x -= 1;
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) dir.x += 1;
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))    dir.y -= 1;   /* y negativo = sobe */
    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))  dir.y += 1;   /* y positivo = desce */

    /* So mexe se alguma tecla de movimento estiver pressionada. */
    if (dir.x != 0 || dir.y != 0) {
        /*
         * NORMALIZACAO: se o jogador aperta duas teclas (ex.: direita + baixo),
         * "dir" vira (1,1), cujo comprimento e raiz(2) ~= 1.414. Usando assim,
         * a diagonal seria ~41% mais rapida que andar reto. Dividindo pelo
         * comprimento, "dir" vira um vetor UNITARIO (comprimento 1), entao a
         * velocidade fica IGUAL na reta e na diagonal.
         *
         * Multiplicar por "delta" mantem a velocidade independente do FPS.
         */
        float length = sqrtf(dir.x * dir.x + dir.y * dir.y);
        dir.x /= length;
        dir.y /= length;

        player.position.x += dir.x * player.speed * delta;
        player.position.y += dir.y * player.speed * delta;
    }

    /* Limitar nas bordas: usa SCREEN_W x SCREEN_H do config.h.
     * O canto superior esquerdo e "position"; o fundo/direita e "position + size". */
    if (player.position.x < 0) player.position.x = 0;
    if (player.position.x + player.size.x > SCREEN_W) player.position.x = SCREEN_W - player.size.x;

    if (player.position.y < 0) player.position.y = 0;
    if (player.position.y + player.size.y > SCREEN_H) player.position.y = SCREEN_H - player.size.y;
}

/*
 * PlayerDraw - desenha o jogador.
 * Desenhamos um retangulo (corpo) e outro menor em cima (cabeca).
 */
void PlayerDraw(void) {
    DrawRectangleV(player.position, player.size, player.color);   /* Corpo. */
    DrawRectangle(player.position.x + 5, player.position.y - 10, 20, 10, DARKBLUE); /* Cabeca. */
}

/*
 * PlayerClose - por enquanto nao ha nada alocado dinamicamente para liberar.
 * Deixamos a funcao pronta para o futuro (quando houver malloc no jogador).
 */
void PlayerClose(void) {
    /* Nada para liberar por enquanto */
}

/*
 * PlayerGet - devolve o ENDERECO do jogador (Player*).
 * Com o ponteiro, outro modulo pode ler/alterar o jogador sem copia.
 * O "&" pega o endereco da variavel.
 */
Player* PlayerGet(void) {
    return &player;
}
