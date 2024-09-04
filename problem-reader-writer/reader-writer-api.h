#pragma once

struct thread_data {
    int tid;
    int type;
    void (*body)(void *);
    void *body_arg;
};
typedef struct thread_data tdata_t;

void init_reader_writer();
void finish_reader_writer();
void *reader_thread(void *arg);
void *writer_thread(void *arg);
