//
// config.h - configuracoes globais. Unico lugar onde o tamanho da tela
// e definido; os outros modulos leem daqui.
//

#ifndef CONFIG_H
#define CONFIG_H

#define SCREEN_W 1280
#define SCREEN_H 720

// Faixa do "chao" na parte de baixo da tela.
#define GROUND_HEIGHT 40
#define GROUND_Y      (SCREEN_H - GROUND_HEIGHT)

// Arquivo onde o ranking e salvo (relativo a pasta de execucao).
#define RANKING_FILE  "ranking.txt"

#endif
