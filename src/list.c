/*
 * list.c - Implementacao da lista encadeada generica.
 *
 * Como a lista guarda "void*", ela nao sabe (nem precisa saber) o que esta
 * guardando. Quem usa a lista e responsavel por criar e destruir os dados;
 * a lista cuida apenas de encadear e desencadear os nos.
 */

#include "list.h"

/*
 * ListCreate - aloca uma lista nova e vazia.
 * head = tail = NULL indicam lista vazia; count = 0.
 * Retorna um ponteiro para a lista (ou NULL se faltar memoria).
 */
List* ListCreate(void) {
    List* list = (List*)malloc(sizeof(List));   /* Reserva memoria para o cabecalho da lista. */
    if (list == NULL) return NULL;              /* Se malloc falhou, avisa quem chamou.        */
    list->head = NULL;                          /* Lista vazia: nao ha primeiro no.            */
    list->tail = NULL;                          /* Lista vazia: nao ha ultimo no.              */
    list->count = 0;                            /* Zero elementos.                             */
    return list;
}

/*
 * ListDestroy - apaga TODA a lista: primeiro os nos (ListClear) e depois
 * o proprio cabecalho. Depois disso, o ponteiro nao deve mais ser usado.
 */
void ListDestroy(List* list) {
    if (list == NULL) return;   /* Nada a fazer se a lista nao existe. */
    ListClear(list);            /* Libera todos os nos.                */
    free(list);                 /* Libera o cabecalho da lista.        */
}

/*
 * ListPushBack - insere um novo no no FIM da lista.
 * Gracias ao ponteiro "tail", isso e rapido (nao precisa percorrer a lista).
 */
void ListPushBack(List* list, void* data) {
    if (list == NULL) return;                                   /* Lista invalida.                 */
    Node* newNode = (Node*)malloc(sizeof(Node));                /* Cria um no novo.                */
    if (newNode == NULL) return;                                /* Sem memoria: desiste.           */
    newNode->data = data;                                       /* Guarda o dado (so o endereco).  */
    newNode->next = NULL;                                       /* Como vai pro fim, nao tem proximo. */

    if (list->head == NULL) {
        /* Lista estava vazia: o novo no e o primeiro E o ultimo. */
        list->head = newNode;
        list->tail = newNode;
    } else {
        /* Lista ja tinha itens: liga o antigo ultimo ao novo e atualiza o tail. */
        list->tail->next = newNode;
        list->tail = newNode;
    }
    list->count++;   /* Agora ha um elemento a mais. */
}

/*
 * ListPushFront - insere um novo no no INICIO da lista.
 */
void ListPushFront(List* list, void* data) {
    if (list == NULL) return;
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) return;
    newNode->data = data;
    newNode->next = list->head;   /* O novo no aponta para o antigo primeiro no. */

    list->head = newNode;         /* O novo no passa a ser o primeiro.           */
    if (list->tail == NULL) {
        /* Se a lista estava vazia, o novo no tambem e o ultimo. */
        list->tail = newNode;
    }
    list->count++;
}

/*
 * ListPopFront - remove o PRIMEIRO no e devolve o dado dele via "void** data".
 *
 * Por que ponteiro de ponteiro (void**)? Porque a funcao quer devolver DUAS
 * coisas: se deu certo (bool) e qual dado saiu. Entao escrevemos o dado no
 * endereco apontado por "data" (*data = ...). Quem chama passa &minhaVariavel.
 */
bool ListPopFront(List* list, void** data) {
    if (list == NULL || list->head == NULL) return false;   /* Lista inexistente ou vazia. */
    Node* temp = list->head;                /* Guarda o no que vamos remover.          */
    *data = temp->data;                     /* Devolve o dado para quem chamou.        */
    list->head = temp->next;                /* O segundo no passa a ser o primeiro.    */
    if (list->head == NULL) {
        list->tail = NULL;                  /* Se a lista ficou vazia, tail tambem zera. */
    }
    free(temp);                             /* Libera o no removido.                   */
    list->count--;                          /* Um elemento a menos.                    */
    return true;                            /* Avisou que deu certo.                   */
}

/*
 * ListRemove - procura um dado especifico (comparando ENDERECOS) e remove o no.
 * Retorna true se achou e removeu, false se nao encontrou.
 *
 * Paciencia: precisa tratar o caso do primeiro no separadamente, porque para
 * remover um no do meio precisamos do no ANTERIOR a ele.
 */
bool ListRemove(List* list, void* data) {
    if (list == NULL || list->head == NULL) return false;   /* Nada para remover. */

    /* Caso 1: o dado esta no primeiro no. */
    if (list->head->data == data) {
        Node* temp = list->head;
        list->head = temp->next;
        if (list->head == NULL) list->tail = NULL;   /* Lista ficou vazia. */
        free(temp);
        list->count--;
        return true;
    }

    /* Caso 2: o dado esta no meio ou no fim. Percorremos olhando sempre o PROXIMO. */
    Node* current = list->head;
    while (current->next != NULL) {
        if (current->next->data == data) {
            Node* temp = current->next;         /* No a remover.                        */
            current->next = temp->next;         /* Pula por cima dele.                  */
            if (current->next == NULL) list->tail = current; /* Removeu o ultimo: atualiza tail. */
            free(temp);
            list->count--;
            return true;
        }
        current = current->next;
    }
    return false;   /* Percorreu tudo e nao achou. */
}

/*
 * ListClear - libera todos os nos, deixando a lista VAZIA mas ainda usavel.
 * Diferente de ListDestroy, a lista em si (cabecalho) permanece.
 *
 * Detalhe importante: guardamos o proximo (current = current->next) ANTES de
 * dar free, porque depois do free o campo next nao pode mais ser lido.
 */
void ListClear(List* list) {
    if (list == NULL) return;
    Node* current = list->head;
    while (current != NULL) {
        Node* temp = current;           /* Guarda o no atual.        */
        current = current->next;        /* Avanca ANTES de liberar.  */
        free(temp);                    /* Libera o no atual.        */
    }
    list->head = NULL;                 /* Lista volta a ficar vazia. */
    list->tail = NULL;
    list->count = 0;
}

/*
 * ListForEach - percorre a lista do inicio ao fim, chamando uma funcao
 * (callback) para cada dado.
 *
 * O callback recebe o dado e um "context" (qualquer coisa que voce queira
 * passar, util para acumular resultados). Isso e o que a linguagem C chama
 * de "ponteiro para funcao": voce passa o NOME de uma funcao como argumento.
 */
void ListForEach(List* list, void (*callback)(void* data, void* context), void* context) {
    if (list == NULL || callback == NULL) return;   /* Precisa da lista e da funcao. */
    Node* current = list->head;
    while (current != NULL) {
        callback(current->data, context);   /* Chama a funcao do usuario para este dado. */
        current = current->next;            /* Vai para o proximo no.                     */
    }
}

/*
 * ListRemoveIf - percorre a lista e remove TODOS os itens que passam no teste.
 *
 * Parametros:
 *   - predicate:   funcao que recebe (data, context) e devolve true se o item
 *                  deve ser removido. Ex.: "e uma entidade inativa?"
 *   - context:     valor extra repassado para o predicate (pode ser NULL).
 *   - destructor:  funcao chamada no dado removido para liberar memoria
 *                  (ex.: EntityDestroy). Pode ser NULL se nao houver o que liberar.
 *
 * Retorna quantos itens foram removidos.
 *
 * Detalhe de lista encadeada: para remover um no do MEIO precisamos do no
 * ANTERIOR ("prev"), para liga-lo ao no seguinte, "pulando" o removido.
 * Guardamos "next" ANTES de dar free, porque depois do free nao podemos mais
 * acessar current->next.
 */
int ListRemoveIf(List* list, bool (*predicate)(void* data, void* context), void* context, void (*destructor)(void* data)) {
    if (list == NULL || predicate == NULL) return 0;   /* Sem lista ou sem teste, nao ha o que fazer. */

    int removed = 0;
    Node* previous = NULL;          /* No anterior ao que estamos olhando.        */
    Node* current = list->head;     /* Comeca pelo primeiro.                      */

    while (current != NULL) {
        Node* next = current->next;                 /* Guarda o proximo ANTES de mexer. */

        if (predicate(current->data, context)) {    /* Esse item deve sair?             */
            /* Desencadeia o no: liga o anterior direto no proximo. */
            if (previous == NULL) {
                list->head = next;                  /* Removendo o primeiro.            */
            } else {
                previous->next = next;              /* Pulando o no removido.           */
            }
            if (list->tail == current) {
                list->tail = previous;              /* Removeu o ultimo: tail recua.    */
            }

            if (destructor != NULL) {
                destructor(current->data);          /* Libera o DADO (ex.: EntityDestroy). */
            }
            free(current);                          /* Libera o NO.                     */
            list->count--;
            removed++;
        } else {
            previous = current;                     /* Esse fica; ele vira o "anterior". */
        }

        current = next;                             /* Avanca para o proximo no.         */
    }

    return removed;
}
