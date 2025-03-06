#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include "monkey-crossing-api.h"
#include "monkey-crossing.h"

void init_monkey_crossing(int rope_capacity) {
    ser = create_serializer();
    east_c = create_crowd(ser);
    west_c = create_crowd(ser);
    queue_common = create_queue(ser);
    capacity = rope_capacity;
}

void finish_monkey_crossing() {
    destroy_queue(ser, queue_common);
    destroy_crowd(ser, west_c);
    destroy_crowd(ser, east_c);
    destroy_serializer(ser);
}

bool east_cond(void *arg) {
    // printf("entering the reader_cond function\n");
    // return !crowd_count(ser, write_c);
    //  printf("The function pointer of east cond is %p\n", east_cond);
     return (crowd_count(ser,east_c)<capacity && (crowd_count(ser, west_c) == 0));
     //return true;
}

bool west_cond(void *arg) {
    // printf("The function pointer of west cond is %p\n", west_cond);
    return (crowd_count(ser,west_c)<capacity && (crowd_count(ser,east_c) == 0));
    // return true;
}

void *eastbound_thread(void *arg) {
    tdata_t *td = (tdata_t *)arg;

    enter_serializer(ser);
    // printf("Eastbound entered\n");

    enqueue(ser, queue_common, east_cond);
    join_crowd(ser, east_c, td->body, td->body_arg);

    exit_serializer(ser);

    pthread_exit(NULL);
    // pthread_exit(NULL);
}

void *westbound_thread(void *arg)
{
    tdata_t *td = (tdata_t *)arg;

    enter_serializer(ser);

    enqueue(ser, queue_common, west_cond);

    join_crowd(ser, west_c, td->body, td->body_arg);

    exit_serializer(ser);

    pthread_exit(NULL);
}