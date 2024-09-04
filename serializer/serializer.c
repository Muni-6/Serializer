#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include <string.h>

#include "serializer-api.h"
#include "serializer.h"

serializer_t *create_serializer() {
    serializer_t *temp = NULL;

    // Allocate necessary structures using malloc...

    return temp;
}

void destroy_serializer(serializer_t *serial) {

    // Free memory...
    
}

void enter_serializer(serializer_t *serial) {

    // Gain possession of the serializer
    
}

void exit_serializer(serializer_t *serial) {

    // Release

}

crowd_t *create_crowd(serializer_t *crowd) {
    crowd_t *temp = NULL;

    // Allocate necessary structures using malloc...

    return temp;
}

void destroy_crowd(serializer_t *serial, crowd_t *crowd) {

    // Free memory...

}

void join_crowd(serializer_t *serial, crowd_t *crowd, void (*body)(void *), void *body_args) {
    // Put the executing thread into crowd

    // Release possession of the serializer

    // Execute list of statements
    body(body_args);

    // Perform leave_crowd operation

}

int crowd_count(serializer_t *serial, crowd_t *crowd) {
    int ret = 0;

    return ret;
}

queue_t *create_queue(serializer_t *serial) {
    queue_t *temp = NULL;

    // Allocate necessary structures using malloc...

    return temp;
}

void destroy_queue(serializer_t *serial, queue_t *queue) {

    // Free memory...

}

void enqueue(serializer_t *serial, queue_t *queue, bool (*cond)(void *)) {
    // Place the executing thread into end of queue
    // Check condition when head of queue is reached

    // Perform dequeue operation

}
