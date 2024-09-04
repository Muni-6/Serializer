#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>

#include "child-care-api.h"
#include "child-care.h"

void init_child_care() {
    
}

void finish_child_care() {

}

void *caregiver_arrive_thread(void *arg) {

    pthread_exit(NULL);
}

void *caregiver_depart_thread(void *arg) {

    pthread_exit(NULL);
}

void *child_arrive_thread(void *arg) {

    pthread_exit(NULL);
}

void *child_depart_thread(void *arg) {

    pthread_exit(NULL);
}
