#ifndef LIST_H
#define LIST_H

#include <stdlib.h>
#include <stdbool.h>

typedef struct Node {
    void* data;
    struct Node* next;
} Node;

typedef struct {
    Node* head;
    Node* tail;
    int count;
} List;

List* ListCreate(void);
void ListDestroy(List* list);
void ListPushBack(List* list, void* data);
void ListPushFront(List* list, void* data);
bool ListPopFront(List* list, void** data);
bool ListRemove(List* list, void* data);
void ListClear(List* list);
void ListForEach(List* list, void (*callback)(void* data, void* context), void* context);

#endif // LIST_H