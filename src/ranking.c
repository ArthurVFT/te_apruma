// ranking.c - as melhores pontuacoes, guardadas numa matriz e salvas em arquivo.

#include "ranking.h"
#include "config.h"
#include <stdio.h>

// A matriz do ranking. "static" = privada deste arquivo.
static int ranking[RANKING_SIZE][RANKING_COLS];

// Quantas linhas ja foram preenchidas (0 ate RANKING_SIZE).
static int filled = 0;

// Le o arquivo e preenche a matriz. Sem arquivo, o ranking comeca vazio.
static void RankingLoad(void) {
    FILE* f = fopen(RANKING_FILE, "r");
    if (f == NULL) return;

    // fscanf devolve quantos valores leu: 2 = linha valida.
    while (filled < RANKING_SIZE &&
           fscanf(f, "%d %d", &ranking[filled][RANKING_POINTS],
                              &ranking[filled][RANKING_BAGS]) == 2) {
        filled++;
    }
    fclose(f);
}

// Regrava o arquivo inteiro com as linhas ocupadas da matriz.
static void RankingSave(void) {
    FILE* f = fopen(RANKING_FILE, "w");
    if (f == NULL) return;   // sem permissao de escrita: o jogo continua

    for (int i = 0; i < filled; i++) {
        fprintf(f, "%d %d\n", ranking[i][RANKING_POINTS], ranking[i][RANKING_BAGS]);
    }
    fclose(f);
}

void RankingInit(void) {
    RankingClear();
    RankingLoad();
}

// Zera a matriz inteira: percorre linhas e colunas.
void RankingClear(void) {
    for (int i = 0; i < RANKING_SIZE; i++) {
        for (int j = 0; j < RANKING_COLS; j++) {
            ranking[i][j] = 0;
        }
    }
    filled = 0;
}

//
// RankingAdd - insere (points, bags) na posicao correta, mantendo a matriz
// ordenada da MAIOR para a menor pontuacao.
//
// A ideia e a do "insertion sort": comece pela ultima linha e va empurrando
// as linhas menores uma casa para baixo, ate achar o lugar certo do novo valor.
//
void RankingAdd(int points, int bags) {
    if (points <= 0) return;   // so pontuacoes positivas entram no ranking

    // Se a matriz ja esta cheia e a pontuacao nao supera a pior, nao entra.
    if (filled == RANKING_SIZE &&
        points <= ranking[RANKING_SIZE - 1][RANKING_POINTS]) {
        return;
    }

    // Onde o novo valor comeca: ultima linha livre, ou a pior quando cheia.
    int pos = (filled < RANKING_SIZE) ? filled : RANKING_SIZE - 1;

    // Empurra para baixo as linhas com pontuacao menor, abrindo espaco.
    while (pos > 0 && ranking[pos - 1][RANKING_POINTS] < points) {
        ranking[pos][RANKING_POINTS] = ranking[pos - 1][RANKING_POINTS];
        ranking[pos][RANKING_BAGS]   = ranking[pos - 1][RANKING_BAGS];
        pos--;
    }

    ranking[pos][RANKING_POINTS] = points;
    ranking[pos][RANKING_BAGS]   = bags;

    if (filled < RANKING_SIZE) {
        filled++;
    }

    RankingSave();
}

int RankingGet(int position, int column) {
    if (position < 0 || position >= filled) return 0;
    if (column < 0 || column >= RANKING_COLS) return 0;
    return ranking[position][column];
}

int RankingCount(void) {
    return filled;
}
