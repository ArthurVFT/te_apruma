/*
 * list.h - Cabecalho de uma lista encadeada generica.
 *
 * "Generica" porque ela guarda "void*" (um ponteiro para qualquer coisa):
 * pode ser usada para armazenar entidades, pontuacoes, o que voce quiser.
 *
 * Uma lista encadeada e uma sequencia de nos (Node). Cada no guarda um dado
 * e um ponteiro para o proximo no. O ultimo aponta para NULL (fim da lista).
 */

#ifndef LIST_H
#define LIST_H

#include <stdlib.h>   /* malloc, free.            */
#include <stdbool.h>  /* Tipo bool (true / false). */

/*
 * struct Node - um "elo" da corrente.
 * "data" aponta para o dado guardado (nao copiamos o dado, so o endereco).
 * "next" aponta para o proximo no, ou NULL se for o ultimo.
 */
typedef struct Node {
    void* data;
    struct Node* next;
} Node;

/*
 * struct List - o "cabecalho" da lista, com metadados para facilitar:
 *  - head: primeiro no
 *  - tail: ultimo no  (permite inserir no fim sem percorrer a lista toda)
 *  - count: quantos itens existem (acesso rapido ao tamanho)
 */
typedef struct {
    Node* head;
    Node* tail;
    int count;
} List;

/* Funcoes publicas do modulo (implementadas em list.c). */
List* ListCreate(void);                                                              /* Cria a lista.                        */
void ListDestroy(List* list);                                                        /* Apaga a lista e todos os nos.        */
void ListPushBack(List* list, void* data);                                           /* Insere no fim.                       */
void ListPushFront(List* list, void* data);                                          /* Insere no inicio.                    */
bool ListPopFront(List* list, void** data);                                          /* Remove do inicio e devolve o dado.   */
bool ListRemove(List* list, void* data);                                             /* Remove um dado especifico.           */
void ListClear(List* list);                                                          /* Apaga os nos, mantem a lista vazia.  */
void ListForEach(List* list, void (*callback)(void* data, void* context), void* context); /* Percorre chamando uma funcao.   */
int  ListRemoveIf(List* list, bool (*predicate)(void* data, void* context), void* context, void (*destructor)(void* data)); /* Remove itens que passam no teste. */

#endif /* LIST_H */
