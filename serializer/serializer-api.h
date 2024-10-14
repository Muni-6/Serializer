#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include <string.h>

typedef struct queue_node_t {
    bool (*condition)(void*);
    struct queue_node_t* next; 
    pthread_cond_t node_condV; 
    int id;
} Queue_Node;

struct psu_queue {
    Queue_Node* front;
    Queue_Node* rear;
    // pthread_cond_t queue_cond; 
    int nodes;
    int size;
    // add necessary members
};
struct psu_crowd {
     int count;  
    //  pthread_cond_t crowd_cond; 
    // add necessary members
};
typedef struct psu_crowd crowd_t;

typedef struct psu_queue queue_t;
struct psu_serializer {
    queue_t** queues;             
    crowd_t** crowds; 
    int queue_size;
    int crowd_size;   
    // bool isPossessed;     
    // bool isPossessedCrowd; 
    // bool dequeue;     
    pthread_mutex_t lock;        
    // pthread_cond_t serializer_cond; 
    // add necessary members
};
typedef struct psu_serializer serializer_t;


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