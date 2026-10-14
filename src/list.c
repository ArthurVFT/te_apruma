/* list.c - lista encadeada generica (guarda void*). */

#include "list.h"

List* ListCreate(void) {
    List* list = (List*)malloc(sizeof(List));
    if (list == NULL) return NULL;
    list->head = NULL;
    list->tail = NULL;
    list->count = 0;
    return list;
}

void ListDestroy(List* list) {
    if (list == NULL) return;
    ListClear(list);
    free(list);
}

void ListPushBack(List* list, void* data) {
    if (list == NULL) return;

    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) return;
    newNode->data = data;
    newNode->next = NULL;

    if (list->head == NULL) {
        list->head = newNode;   /* lista estava vazia */
        list->tail = newNode;
    } else {
        list->tail->next = newNode;
        list->tail = newNode;
    }
    list->count++;
}

void ListPushFront(List* list, void* data) {
    if (list == NULL) return;

    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) return;
    newNode->data = data;
    newNode->next = list->head;

    list->head = newNode;
    if (list->tail == NULL) {
        list->tail = newNode;   /* lista estava vazia */
    }
    list->count++;
}

bool ListPopFront(List* list, void** data) {
    if (list == NULL || list->head == NULL) return false;

    Node* temp = list->head;
    *data = temp->data;
    list->head = temp->next;
    if (list->head == NULL) list->tail = NULL;
    free(temp);
    list->count--;
    return true;
}

bool ListRemove(List* list, void* data) {
    if (list == NULL || list->head == NULL) return false;

    /* Caso 1: o dado esta no primeiro no. */
    if (list->head->data == data) {
        Node* temp = list->head;
        list->head = temp->next;
        if (list->head == NULL) list->tail = NULL;
        free(temp);
        list->count--;
        return true;
    }

    /* Caso 2: esta no meio/fim. Guardamos o ANTERIOR para "pular" o removido. */
    Node* current = list->head;
    while (current->next != NULL) {
        if (current->next->data == data) {
            Node* temp = current->next;
            current->next = temp->next;
            if (current->next == NULL) list->tail = current;
            free(temp);
            list->count--;
            return true;
        }
        current = current->next;
    }
    return false;
}

void ListClear(List* list) {
    if (list == NULL) return;

    Node* current = list->head;
    while (current != NULL) {
        Node* temp = current;
        current = current->next;   /* avanca ANTES de liberar o no atual */
        free(temp);
    }
    list->head = NULL;
    list->tail = NULL;
    list->count = 0;
}

void ListForEach(List* list, void (*callback)(void* data, void* context), void* context) {
    if (list == NULL || callback == NULL) return;

    Node* current = list->head;
    while (current != NULL) {
        callback(current->data, context);
        current = current->next;
    }
}

int ListRemoveIf(List* list, bool (*predicate)(void* data, void* context),
                 void* context, void (*destructor)(void* data)) {
    if (list == NULL || predicate == NULL) return 0;

    int removed = 0;
    Node* prev = NULL;
    Node* current = list->head;

    while (current != NULL) {
        Node* next = current->next;   /* guarda o proximo antes de mexer */

        if (predicate(current->data, context)) {
            if (prev == NULL) {
                list->head = next;        /* removeu o primeiro */
            } else {
                prev->next = next;        /* pula o no removido */
            }
            if (list->tail == current) {
                list->tail = prev;
            }
            if (destructor != NULL) {
                destructor(current->data);   /* libera o DADO */
            }
            free(current);                   /* libera o NO   */
            list->count--;
            removed++;
        } else {
            prev = current;
        }

        current = next;
    }

    return removed;
}
