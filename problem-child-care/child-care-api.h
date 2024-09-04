#pragma once

struct thread_data {
    int tid;
    int type;
    void (*body)(void *);
    void *body_arg;
};
typedef struct thread_data tdata_t;

void init_child_care();
void finish_child_care();
void *caregiver_arrive_thread(void *arg);
void *caregiver_depart_thread(void *arg);
void *child_arrive_thread(void *arg);
void *child_depart_thread(void *arg);
