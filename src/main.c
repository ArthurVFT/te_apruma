/*
 * main.c - Ponto de entrada do jogo "Te Apruma!"
 *
 * Este arquivo e o mais simples do projeto: ele so chama as tres funcoes
 * principais do modulo de jogo, na ordem correta (iniciar -> rodar -> fechar).
 * Toda a logica de verdade fica em game.c, player.c, entity.c e list.c.
 */

/*
 * Bloco de inclusao do raylib.
 * Como cada pessoa do grupo pode ter instalado o raylib em um lugar diferente,
 * usamos __has_include (recurso do compilador) para tentar varios caminhos ate
 * achar o raylib.h. Se achar no caminho padrao "raylib.h" (porque o include
 * esta configurado no Makefile), usa esse. Caso contrario tenta caminhos fixos.
 */
#if __has_include("raylib.h")
    #include "raylib.h"
#elif __has_include("C:/raylib/src/raylib.h")
    #include "C:/raylib/src/raylib.h"
#else
    #include "raylib.h"
#endif

#include "game.h"   /* Declaracoes de GameInit, GameRun e GameClose. */

/*
 * main - funcao que o sistema operacional chama quando o programa inicia.
 * O tipo "int" e o retorno que o SO espera (0 = encerrou sem erro).
 * O "void" entre parenteses significa "nao recebe nenhum argumento".
 */
int main(void) {
    GameInit();    /* Abre a janela do Raylib e prepara o jogador. */
    GameRun();     /* Entra no loop principal do jogo (roda ate fechar a janela). */
    GameClose();   /* Libera recursos (Raylib) ao sair. */

    return 0;      /* Avisa o sistema que terminou com sucesso. */
}
