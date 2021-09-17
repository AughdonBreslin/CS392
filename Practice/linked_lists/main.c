#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "linked_list.h"

void print_str(void *str) {
    printf("\"%s\"", (char *)str);
}

void insert_word(linked_list *list, char *word, bool at_end) {
    if (at_end) {
        push_back(list, create_node((void *)strdup(word)));
    } else {
        push_front(list, create_node((void *)strdup(word)));
    }
}

int str_cmp(const void *a, const void *b) {
    return strcmp(*(const char **)a,*(const char **)b);
}

int main() {
    linked_list *list = create_linked_list();
    insert_word(list, "hello", true);
    insert_word(list, "CS392", true);
    insert_word(list, "Dr. B", false);
    insert_word(list, "Dr. Evil", true);
    print_list(list, print_str);
    linked_list *less = find_less(list, "dah", str_cmp);
    printf("Num nodes: %ld\n", count_nodes(list));
    while (list->num_nodes > 0) {
        node *front = pop_back(list);
        printf("Popped value: %s\n", (char *)front->data);
        free_node(front, free);
    }
   
    free(list);
    if(less){
        while (less->num_nodes > 0){
            free_node(pop_front(less),free);
        }
        free(less);
    }

    return 0;
}
