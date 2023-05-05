#include <string.h>
#include <stdio.h>
#include "queue.h"
#include "lib.h"

char pop(char * queue) {
    if (empty(queue)) {
        perror("Empty queue");
        return '\0';
    }
    char item = queue[0];
    memcpy(queue, queue + 1, strlen(queue) + 1);
    return item;
}

void push(char *queue, char item) {
    if (full(queue)) {
        perror("Full queue");
        return;
    }
    int n = strlen(queue);
    queue[n] = item;
    queue[n+1] = '\0';
}

int full(char *queue) {
    return (strlen(queue) + 1 == QUEUE_SIZE);
}

int empty(char *queue) {
    return (strlen(queue) == 0);
}
