#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>

#include "child-care-api.h"
#include "child-care.h"


void init_child_care() {
    ser = create_serializer();
    
    caregiver_arrive_queue = create_queue(ser);
    caregiver_depart_queue = create_queue(ser);
    child_arrive_queue = create_queue(ser);
    child_depart_queue = create_queue(ser);

    // Create only the necessary crowds
    caregiver_arrive_crowd = create_crowd(ser);
    child_arrive_crowd = create_crowd(ser);
}

void finish_child_care() {
    // Destroy only the initialized crowds and queues
    destroy_crowd(ser, caregiver_arrive_crowd);
    destroy_crowd(ser, child_arrive_crowd);
    
    destroy_queue(ser, caregiver_arrive_queue);
    destroy_queue(ser, caregiver_depart_queue);
    destroy_queue(ser, child_arrive_queue);
    destroy_queue(ser, child_depart_queue);
    
    destroy_serializer(ser);
}

bool caregiver_arrive_cond(void *arg) {
    return true;
}

bool child_depart_cond(void *arg) {
    return true;
}

bool caregiver_depart_cond(void *arg) {
    bool condition = (((crowd_count(ser, child_arrive_crowd) > 0))
                      && (crowd_count(ser, caregiver_arrive_crowd) > 0))
                     || (crowd_count(ser, child_arrive_crowd) == 0);
    return condition;
}

bool child_arrive_cond(void *arg) {
    return crowd_count(ser, caregiver_arrive_crowd) > 0;
}

void *caregiver_arrive_thread(void *arg) {
    tdata_t *td = (tdata_t *)arg;

    enter_serializer(ser);
    enqueue(ser, caregiver_arrive_queue, caregiver_arrive_cond);
    join_crowd(ser, caregiver_arrive_crowd, td->body, td->body_arg);
    exit_serializer(ser);

    pthread_exit(NULL);
}

void *caregiver_depart_thread(void *arg) {
    tdata_t *td = (tdata_t *)arg;
    enter_serializer(ser);
    enqueue(ser, caregiver_depart_queue, caregiver_depart_cond);
    exit_serializer(ser);

    pthread_exit(NULL);
}

void *child_arrive_thread(void *arg) {
    tdata_t *td = (tdata_t *)arg;

    enter_serializer(ser);
    enqueue(ser, child_arrive_queue, child_arrive_cond);
    join_crowd(ser, child_arrive_crowd, td->body, td->body_arg);
    exit_serializer(ser);

    pthread_exit(NULL);
}

void *child_depart_thread(void *arg) {
    tdata_t *td = (tdata_t *)arg;

    enter_serializer(ser);
    enqueue(ser, child_depart_queue, child_depart_cond);
    exit_serializer(ser);

    pthread_exit(NULL);
}