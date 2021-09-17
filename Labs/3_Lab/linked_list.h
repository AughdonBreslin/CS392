/*******************************************************************************
 * Name        : linked_list.h (Lab 3)
 * Author      : Matthew Oyales & Audie Breslin
 * Date        : 4 June 2021
 * Description : Practice with pointers and DLLs.
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/

#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

#include <stdio.h>
#include <string.h>
#include "node.h"

typedef struct linked_list {
    node *head;
    node *tail;
    size_t num_nodes;
} linked_list;

linked_list* create_linked_list() {
    return (linked_list *)calloc(1, sizeof(linked_list));
}

/**
 * Inserts a non-NULL node into a non-NULL linked list.
 * The node is inserted so that the data in the linked list is in
 * non-decreasing order.
 * A node with a value already in the list is inserted AFTER the node(s)
 * already in the list.
 */
void insert_in_order(linked_list *list, node *n,
                     int (*cmp)(const void*, const void*)) {
    // TODO
    //Zero elements in list
    list->numnodes++;
    node *p = list->head, *q = NULL;
    while (p != NULL && cmp(p->data, n->data) <= 0) {
        q = p;
        p = p->next;
    }
    n->next = p;
    n->prev = q;
    if (p != NULL) {
        p->prev = n;
    } else {
        list->tail = n;
    }
    if (q != NULL) {
        q->next = n;
    } else {
        list->head = n;
    }
}

void print_list(linked_list *list, void (*print_function)(void*)) {
    putchar('[');
    node *cur = list->head;
    if (cur != NULL) {
        print_function(cur->data);
        cur = cur->next;
    }
    for ( ; cur != NULL; cur = cur->next) {
        printf(", ");
        print_function(cur->data);
    }
    printf("]\n{length: %lu, head->data: ", list->num_nodes);
    list->head != NULL ? print_function(list->head->data) :
                         (void)printf("NULL");
    printf(", tail->data: ");
    list->tail != NULL ? print_function(list->tail->data) :
                         (void)printf("NULL");
    printf("}\n\n");
}

/**
 * Frees a list starting from the tail.
 * This will check if your previous pointers have been set up correctly.
 */
void free_list(linked_list *list, void (*free_data)(void *)) {
    while (list->tail != NULL) {
        node *prev = list->tail->prev;
        free_node(list->tail, free_data);
        list->tail = prev;
    }
}

#endif
