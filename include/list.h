#ifndef LIST_H
#define LIST_H

#include <stdlib.h>   /* malloc, free, NULL */
#include <stdbool.h>

/*
 * Lista encadeada generica: guarda void* (nao sabe o que esta guardando).
 * Quem usa a lista e responsavel por criar e destruir os dados.
 */

typedef struct Node {
    void* data;
    struct Node* next;
} Node;

typedef struct {
    Node* head;   /* primeiro node                        */
    Node* tail;   /* ultimo no (insercao rapida no fim) */
    int   count;  /* quantidade de itens                */
} List;

List* ListCreate(void);
void ListDestroy(List* list);
void ListPushBack(List* list, void* data);
void ListPushFront(List* list, void* data);
bool ListPopFront(List* list, void** data);
bool ListRemove(List* list, void* data);
void ListClear(List* list);
void ListForEach(List* list, void (*callback)(void* data, void* context), void* context);
int  ListRemoveIf(List* list,
                  bool (*predicate)(void* data, void* context),
                  void* context,
                  void (*destructor)(void* data));

#endif
