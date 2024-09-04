#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>

#include "monkey-crossing-api.h"
#include "monkey-crossing.h"

void init_monkey_crossing(int rope_capacity) {

}

void finish_monkey_crossing() {

}

void *eastbound_thread(void *arg) {

    pthread_exit(NULL);
}

void *westbound_thread(void *arg) {

    pthread_exit(NULL);
}
