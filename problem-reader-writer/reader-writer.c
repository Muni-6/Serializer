#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>

#include "reader-writer-api.h"
#include "reader-writer.h"

void init_reader_writer() {
    ser = create_serializer();
    read_c = create_crowd(ser);
    write_c = create_crowd(ser);
    read_q = create_queue(ser);
    write_q = create_queue(ser);
}

void finish_reader_writer() {
    destroy_queue(ser, write_q);
    destroy_queue(ser, read_q);
    destroy_crowd(ser, write_c);
    destroy_crowd(ser, read_c);
    destroy_serializer(ser);
}

bool reader_cond(void *arg) {
    printf("entering the reader_cond function\n");
    return !crowd_count(ser, write_c);
}

bool writer_cond(void *arg) {
    return (crowd_count(ser, write_c) == 0) && (crowd_count(ser, read_c) == 0);
}

void *reader_thread(void *arg) {
    tdata_t *td = (tdata_t *)arg;

    enter_serializer(ser);

    enqueue(ser, read_q, reader_cond);
    join_crowd(ser, read_c, td->body, td->body_arg);

    exit_serializer(ser);

    pthread_exit(NULL);
}

void *writer_thread(void *arg) {
    tdata_t *td = (tdata_t *)arg;

    enter_serializer(ser);

    enqueue(ser, write_q, writer_cond);
    join_crowd(ser, write_c, td->body, td->body_arg);

    exit_serializer(ser);

    pthread_exit(NULL);
}
