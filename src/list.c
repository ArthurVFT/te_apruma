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
        list->head = newNode;
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
        list->tail = newNode;
    }
    list->count++;
}

bool ListPopFront(List* list, void** data) {
    if (list == NULL || list->head == NULL) return false;
    Node* temp = list->head;
    *data = temp->data;
    list->head = temp->next;
    if (list->head == NULL) {
        list->tail = NULL;
    }
    free(temp);
    list->count--;
    return true;
}

bool ListRemove(List* list, void* data) {
    if (list == NULL || list->head == NULL) return false;
    
    if (list->head->data == data) {
        Node* temp = list->head;
        list->head = temp->next;
        if (list->head == NULL) list->tail = NULL;
        free(temp);
        list->count--;
        return true;
    }
    
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
        current = current->next;
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