/* ranking.c - as melhores pontuacoes, guardadas numa matriz. */

#include "ranking.h"

/* A matriz do ranking. "static" = privada deste arquivo. */
static int ranking[RANKING_SIZE][RANKING_COLS];

/* Quantas linhas ja foram preenchidas (0 ate RANKING_SIZE). */
static int filled = 0;

void RankingInit(void) {
    RankingClear();
}

/* Zera a matriz inteira: percorre linhas e colunas. */
void RankingClear(void) {
    for (int i = 0; i < RANKING_SIZE; i++) {
        for (int j = 0; j < RANKING_COLS; j++) {
            ranking[i][j] = 0;
        }
    }
    filled = 0;
}

/*
 * RankingAdd - insere (points, bags) na posicao correta, mantendo a matriz
 * ordenada da MAIOR para a menor pontuacao.
 *
 * A ideia e a do "insertion sort": comece pela ultima linha e va empurrando
 * as linhas menores uma casa para baixo, ate achar o lugar certo do novo valor.
 */
void RankingAdd(int points, int bags) {
    if (points <= 0) return;   /* so pontuacoes positivas entram no ranking */

    /* Se a matriz ja esta cheia e a pontuacao nao supera a pior, nao entra. */
    if (filled == RANKING_SIZE &&
        points <= ranking[RANKING_SIZE - 1][RANKING_POINTS]) {
        return;
    }

    /* Onde o novo valor comeca: ultima linha livre, ou a pior quando cheia. */
    int pos = (filled < RANKING_SIZE) ? filled : RANKING_SIZE - 1;

    /* Empurra para baixo as linhas com pontuacao menor, abrindo espaco. */
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
}

int RankingGet(int position, int column) {
    if (position < 0 || position >= filled) return 0;
    if (column < 0 || column >= RANKING_COLS) return 0;
    return ranking[position][column];
}

int RankingCount(void) {
    return filled;
}
