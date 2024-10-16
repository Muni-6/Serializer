#pragma once

// Serializer data structures
typedef struct queue_node_t {
    bool (*condition)(void*);
    struct queue_node_t* next; 
    pthread_cond_t node_condV; 
    int id;
} Queue_Node;