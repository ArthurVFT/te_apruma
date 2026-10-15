// main.c - ponto de entrada do jogo: iniciar, rodar e fechar.

#include "game.h"

int main(void) {
    GameInit();    // abre a janela e prepara tudo
    GameRun();     // loop principal
    GameClose();   // libera recursos

    return 0;
}
