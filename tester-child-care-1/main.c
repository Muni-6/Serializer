#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdalign.h>
#include <stdarg.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "problem-child-care/child-care-api.h"

// Data used only in tester
struct log_data {
    // Per-thread data
    int tid;
    char log[8192];

};
typedef struct log_data ldata_t;

// Log a message to logbuf with time
void log_msg(char *logbuf, const char *format, ...) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    int64_t ns = (ts.tv_sec * 1000000000UL) + ts.tv_nsec;

    int ret = sprintf(logbuf, "%ld: ", ns);
    va_list args;
    va_start(args, format);
    ret = vsprintf(logbuf + ret, format, args);
    va_end(args);
}

// Caregiver arrival function
void caregiver_arrives_func(void *arg) {
    ldata_t *rd = (ldata_t *)arg;

    log_msg(rd->log + strlen(rd->log), "tid: %d, caregiver arrival start\n", rd->tid);

    sleep(1);

    log_msg(rd->log + strlen(rd->log), "tid: %d, caregiver arrival end\n", rd->tid);

    sleep(1);
}

// Caregiver departure function
void caregiver_departs_func(void *arg) {
    ldata_t *rd = (ldata_t *)arg;

    log_msg(rd->log + strlen(rd->log), "tid: %d, caregiver departure start\n", rd->tid);

    sleep(1);

    log_msg(rd->log + strlen(rd->log), "tid: %d, caregiver departure end\n", rd->tid);

    sleep(1);
}

// Child arrival function
void child_arrives_func(void *arg) {
    ldata_t *rd = (ldata_t *)arg;

    log_msg(rd->log + strlen(rd->log), "tid: %d, child arrival start\n", rd->tid);

    sleep(1);

    log_msg(rd->log + strlen(rd->log), "tid: %d, child arrival end\n", rd->tid);

    sleep(1);
}

// Child departure function
void child_departs_func(void *arg) {
    ldata_t *rd = (ldata_t *)arg;

    log_msg(rd->log + strlen(rd->log), "tid: %d, child departure start\n", rd->tid);

    sleep(1);

    log_msg(rd->log + strlen(rd->log), "tid: %d, child departure end\n", rd->tid);

    sleep(1);
}

void generate_sequence(char *sequence, int G, int C) {
    int total = G + C; 
    int index = 0;
    for (int i = 0; i < G; ++i) {
        sequence[index++] = 'G'; 
    }
    for (int i = 0; i < C; ++i) {
        sequence[index++] = 'C'; 
    }
    for (int i = 0; i < total; ++i) {
        int j = rand() % total;
        char temp = sequence[i];
        sequence[i] = sequence[j];
        sequence[j] = temp;
    }
    sequence[total] = '\0';
}


int main(int argc, char **argv) {

    printf("%s: %s, start!\n", __func__, argv[0]);

    // if (argc != 3) {
    //     fprintf(stderr, "Usage: ./out <No of Caregivers> <No of children>\n");
    //     exit(EXIT_FAILURE);
    // }

    // int NO_OF_CARETAKERS = atoi(argv[1]);
    // int NO_OF_CHILDREN = atoi(argv[2]);
    // printf("%s: NO_OF_CARETAKERS: %d, NO_OF_CHILDREN: %d\n", __func__, NO_OF_CARETAKERS, NO_OF_CHILDREN);


    // char *arriving_sequence = (char *)malloc((NO_OF_CARETAKERS + NO_OF_CHILDREN + 1) * sizeof(char));
    // char *departing_sequence = (char *)malloc((NO_OF_CARETAKERS + NO_OF_CHILDREN + 1) * sizeof(char));


    // generate_sequence(arriving_sequence, NO_OF_CARETAKERS, NO_OF_CHILDREN);
    // generate_sequence(departing_sequence, NO_OF_CARETAKERS, NO_OF_CHILDREN);

    // // Print the sequences
    // printf("Arriving sequence: %s\n", arriving_sequence);
    // printf("Departing sequence: %s\n", departing_sequence);

    char arriving_sequence[] = "CCCGGGCCCGGCGCG";  
    char departing_sequence[] = "GCCCGCGGCCGCGCG";


    int n_threads = strlen(arriving_sequence);

    pthread_t *arriving_threads = (pthread_t *)malloc(sizeof(pthread_t) * n_threads);
    tdata_t *arriving_thread_data = (tdata_t *)malloc(sizeof(tdata_t) * n_threads);

    pthread_t *departing_threads = (pthread_t *)malloc(sizeof(pthread_t) * n_threads);
    tdata_t *departing_thread_data = (tdata_t *)malloc(sizeof(tdata_t) * n_threads);


    init_child_care();

    printf("%s: launching threads...\n", __func__);
    for (int i = 0; i < n_threads; ++i) {
        ldata_t *temp = (ldata_t *)malloc(sizeof(ldata_t));
        memset(temp, 0, sizeof(ldata_t));
        temp->tid = i;
        memset(&arriving_thread_data[i], 0, sizeof(tdata_t));
        arriving_thread_data[i].tid = i;
        arriving_thread_data[i].body_arg = temp;

        if (arriving_sequence[i] == 'G') {
        arriving_thread_data[i].body = caregiver_arrives_func;
        pthread_create(&arriving_threads[i], NULL, caregiver_arrive_thread, &arriving_thread_data[i]);
        }else if (arriving_sequence[i] == 'C') {
            arriving_thread_data[i].body = child_arrives_func;
            pthread_create(&arriving_threads[i], NULL, child_arrive_thread, &arriving_thread_data[i]);
        }
    }

    for (int i = 0; i < n_threads; ++i) {
        pthread_join(arriving_threads[i], NULL);
    }

    // After arriving threads have joined
    for (int i = 0; i < n_threads; ++i) {
        if (departing_sequence[i] == 'G') {
            departing_thread_data[i].tid = i;
            departing_thread_data[i].body_arg = arriving_thread_data[i].body_arg; // Use the same body_arg
            departing_thread_data[i].body = caregiver_departs_func;
            pthread_create(&departing_threads[i], NULL, caregiver_depart_thread, &departing_thread_data[i]);
        } else if (departing_sequence[i] == 'C') {
            departing_thread_data[i].tid = i;
            departing_thread_data[i].body_arg = arriving_thread_data[i].body_arg; // Use the same body_arg
            departing_thread_data[i].body = child_departs_func;
            pthread_create(&departing_threads[i], NULL, child_depart_thread, &departing_thread_data[i]);
        }
    }


    // Join departing threads
    for (int i = 0; i < n_threads; ++i) {
        pthread_join(departing_threads[i], NULL);
    }


    finish_child_care();

    printf("%s: tester done. Dumping data...\n", __func__);
    FILE *fp = fopen("tester-child-care-1/tester-dump-child-care-1.txt", "w");
    if (fp == NULL) {
        fprintf(stderr, "Error opening file for logging\n");
        exit(EXIT_FAILURE);
    }

    // Dump logs for both arriving and departing threads
    for (int i = 0; i < n_threads; ++i) {
        // Log arrival actions
        char *arrival_log = ((ldata_t *)arriving_thread_data[i].body_arg)->log;
        fwrite(arrival_log, sizeof(char), strlen(arrival_log), fp);

        // Log departure actions
        char *departure_log = ((ldata_t *)departing_thread_data[i].body_arg)->log;
        fwrite(departure_log, sizeof(char), strlen(departure_log), fp);
    }

    fprintf(fp, "\n");
    fclose(fp);

    // Sort log file
    system("sort tester-child-care-1/tester-dump-child-care-1.txt >tester-child-care-1/tester-dump-child-care-1-sorted.txt 2>/dev/null");

    
    free(arriving_thread_data);
    free(departing_thread_data);
    free(arriving_threads);
    free(departing_threads);

    printf("%s: bye!\n", __func__);
    return 0;

}
