/*
 * ranking.h - guarda as melhores pontuacoes.
 *
 * O ranking e uma MATRIZ (vetor bidimensional):
 *
 *      int ranking[RANKING_SIZE][RANKING_COLS];
 *
 * Cada LINHA e uma posicao do ranking (1o lugar, 2o lugar, ...).
 * Cada COLUNA guarda um dado daquela posicao:
 *      coluna 0 -> pontos
 *      coluna 1 -> sacolas coletadas
 *
 * Exemplo (linha = posicao):
 *      ranking[0] = { 12, 5 }   -> 1o lugar: 12 pontos, 5 sacolas
 *      ranking[1] = {  8, 3 }   -> 2o lugar:  8 pontos, 3 sacolas
 */

#ifndef RANKING_H
#define RANKING_H

#define RANKING_SIZE 5   /* quantas posicoes (linhas) o ranking tem */
#define RANKING_COLS 2   /* quantas colunas por posicao             */

/* Indices das colunas (em vez de usar 0 e 1 soltos no codigo). */
#define RANKING_POINTS 0
#define RANKING_BAGS   1

void RankingInit(void);
void RankingClear(void);
void RankingAdd(int points, int bags);          /* insere mantendo a ordem */
int  RankingGet(int position, int column);      /* le uma celula da matriz */
int  RankingCount(void);                        /* quantas posicoes ocupadas */

#endif
