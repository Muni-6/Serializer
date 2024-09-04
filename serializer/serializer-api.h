#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include <string.h>

struct psu_serializer {
    // add necessary members
};
typedef struct psu_serializer serializer_t;

struct psu_crowd {
    // add necessary members
};
typedef struct psu_crowd crowd_t;

struct psu_queue {
    // add necessary members
};
typedef struct psu_queue queue_t;

serializer_t *create_serializer();
void destroy_serializer(serializer_t *serial);
void enter_serializer(serializer_t *serial);
void exit_serializer(serializer_t *serial);

crowd_t *create_crowd(serializer_t *crowd);
void destroy_crowd(serializer_t *serial, crowd_t *crowd);
void join_crowd(serializer_t *serial, crowd_t *crowd, void (*body)(void *), void *body_args);
int crowd_count(serializer_t *serial, crowd_t *crowd);

queue_t *create_queue(serializer_t *serial);
void destroy_queue(serializer_t *serial, queue_t *queue);
void enqueue(serializer_t *serial, queue_t *queue, bool (*cond)(void *));
