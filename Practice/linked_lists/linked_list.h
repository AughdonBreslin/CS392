#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "node.h"

typedef struct linked_list {
    node *head;
    node *tail;
    size_t num_nodes;
} linked_list;

linked_list* create_linked_list() {
    return (linked_list *)calloc(1, sizeof(linked_list));
}

void push_back(linked_list *list, node *node) {
    // Both pointers will point to this node, as the only elem. in the list.
    if (list->head == NULL) {
        list->head = list->tail = node;
    } else {
        list->tail->next = node;
        node->prev = list->tail;
        list->tail = node;
    }
    list->num_nodes++;
}

void push_front(linked_list *list, node *node) {
    if (list->head == NULL) {
        list->head = list->tail = node;
    } else {
        list->head->prev = node;
        node->next = list->head;
        list->head = node;
    }
    list->num_nodes++;
}

node *pop_front(linked_list *list) {
    node *cur = list->head;
    if (cur == NULL) {
        return NULL;
    }
    if (cur->next == NULL) {
        list->head = NULL;
        list->tail = NULL;
    } else {
        list->head = list->head->next;
        list->head->prev = NULL;
        cur->next = NULL;
    }
    list->num_nodes--;
    return cur;
}

node *pop_back(linked_list *list) {
    node *cur = list->tail;
    if (cur == NULL) {
        return NULL;
    }
    if (cur->prev == NULL) {
        list->head = NULL;
        list->tail = NULL;
    } else {
        list->tail = list->tail->prev;
        list->tail->next = NULL;
        cur->prev = NULL;
    }
    list->num_nodes--;
    return cur;
}

size_t count_nodes(linked_list *list) {
    // We cannot have a negative count, so best to use size_t
    size_t count = 0;
    node *cur = list->head;

    while (cur) {
        count++;
        cur = cur->next;
    }

    return count;
}

node *nth_from_end(linked_list *list, int n) {
    if (n > list->num_nodes) {
        return NULL;
    }
    node *cur = list->tail;
    for (int i = 0; i < n; i++) {
        cur = cur->prev;
    }
    return cur;
}

void print_list(linked_list *list, void (*print_function)(void *)) {
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
    puts("]");
}

/** 
 * Returns a new linked list with all the elements
 * from the original list that are less than the supplied key
*/
linked_list *find_less(linked_list *list, const void *key,
                        int (*cmp)(const void *, const void *)) {
    linked_list *results = NULL;
    for (node *cur = list->head; cur != NULL; cur= cur->next) {
        if(cmp(&cur->data,&key) < 0) {
            if (!results) {
                results = create_linked_list();
            }
            push_back(results,create_node((void *)strdup(cur->data)));
        }
    }
    if (results) {
        char **array = malloc(results->num_nodes * sizeof(char *)),
             **p;
        p = array;
        for (node *cur = results->head; cur != NULL; cur= cur->next) {
            *p = cur->data;
            p++;
        }
        qsort(array, results->num_nodes, sizeof(char *), cmp);
        p = array;
        for (node *cur = results->head; cur != NULL; cur= cur->next) {
            cur->data = *p;
            p++;
        }
        free(array);
        
    }
    return results;
}

#endif
