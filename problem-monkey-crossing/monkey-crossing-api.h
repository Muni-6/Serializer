#pragma once

struct thread_data {
    int tid;
    int type;
    void (*body)(void *);
    void *body_arg;
};
typedef struct thread_data tdata_t;

void init_monkey_crossing(int rope_capacity);
void finish_monkey_crossing();
void *eastbound_thread(void *arg);
void *westbound_thread(void *arg);
