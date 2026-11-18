// ranking.c - as melhores pontuacoes, guardadas numa matriz e salvas em arquivo.

#include "ranking.h"
#include "config.h"
#include <stdio.h>

// A matriz do ranking. "static" = privada deste arquivo.
static int ranking[RANKING_SIZE][RANKING_COLS];

// Quantas linhas ja foram preenchidas (0 ate RANKING_SIZE).
static int filled = 0;

// Ordena a matriz da MAIOR para a menor pontuacao (insertion sort).
// Troca as linhas inteiras, para as colunas ficarem juntas.
static void RankingSort(void) {
    for (int i = 1; i < filled; i++) {
        int j = i;
        while (j > 0 && ranking[j - 1][RANKING_POINTS] < ranking[j][RANKING_POINTS]) {
            for (int c = 0; c < RANKING_COLS; c++) {
                int temp = ranking[j - 1][c];
                ranking[j - 1][c] = ranking[j][c];
                ranking[j][c] = temp;
            }
            j--;
        }
    }
}

// Le o arquivo linha a linha e preenche a matriz. Sem arquivo, o ranking comeca vazio.
// Para na primeira linha sem as 3 colunas (ex.: arquivo antigo de 2 colunas).
static void RankingLoad(void) {
    FILE* f = fopen(RANKING_FILE, "r");
    if (f == NULL) return;

    char line[64];
    while (filled < RANKING_SIZE && fgets(line, sizeof line, f) != NULL) {
        // sscanf devolve quantos valores leu: 3 = linha valida.
        if (sscanf(line, "%d %d %d", &ranking[filled][RANKING_POINTS],
                                     &ranking[filled][RANKING_BAGS],
                                     &ranking[filled][RANKING_RESCUED]) != 3) {
            break;
        }
        filled++;
    }
    fclose(f);
    RankingSort();   // ordena o que acabou de ser lido
}

// Regrava o arquivo inteiro com as linhas ocupadas da matriz.
static void RankingSave(void) {
    FILE* f = fopen(RANKING_FILE, "w");
    if (f == NULL) return;   // sem permissao de escrita: o jogo continua

    for (int i = 0; i < filled; i++) {
        fprintf(f, "%d %d %d\n", ranking[i][RANKING_POINTS],
                                 ranking[i][RANKING_BAGS],
                                 ranking[i][RANKING_RESCUED]);
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


void RankingAdd(int points, int bags, int rescued) {
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
        ranking[pos][RANKING_RESCUED] = ranking[pos - 1][RANKING_RESCUED];
        pos--;
    }

    ranking[pos][RANKING_POINTS]  = points;
    ranking[pos][RANKING_BAGS]    = bags;
    ranking[pos][RANKING_RESCUED] = rescued;

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
