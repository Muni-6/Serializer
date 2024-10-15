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

    log_msg(rd->log + strlen(rd->log), "tid: %d, caregiver arrived \n", rd->tid);

    sleep(1);
}

// Caregiver departure function
void caregiver_departs_func(void *arg) {
    ldata_t *rd = (ldata_t *)arg;

    log_msg(rd->log + strlen(rd->log), "tid: %d, caregiver departure start\n", rd->tid);

    sleep(1);

    log_msg(rd->log + strlen(rd->log), "tid: %d, caregiver departed\n", rd->tid);

    sleep(1);
}

// Child arrival function
void child_arrives_func(void *arg) {
    ldata_t *rd = (ldata_t *)arg;

    log_msg(rd->log + strlen(rd->log), "tid: %d, child arrival start\n", rd->tid);

    sleep(1);

    log_msg(rd->log + strlen(rd->log), "tid: %d, child arrived\n", rd->tid);

    sleep(1);
}

// Child departure function
void child_departs_func(void *arg) {
    ldata_t *rd = (ldata_t *)arg;

    log_msg(rd->log + strlen(rd->log), "tid: %d, child departure start\n", rd->tid);

    sleep(1);

    log_msg(rd->log + strlen(rd->log), "tid: %d, child departed\n", rd->tid);

    sleep(1);
}


void generate_sequence(char *sequence, int c, int g, int C, int G) {
    int total = c + g + C + G;
    int index = 0;

    // Fill the sequence with each character based on the counts
    for (int i = 0; i < c; ++i) {
        sequence[index++] = 'c'; // Lowercase caregiver
    }
    for (int i = 0; i < g; ++i) {
        sequence[index++] = 'g'; // Lowercase child
    }
    for (int i = 0; i < C; ++i) {
        sequence[index++] = 'C'; // Uppercase caregiver
    }
    for (int i = 0; i < G; ++i) {
        sequence[index++] = 'G'; // Uppercase child
    }

    // Shuffle the sequence
    for (int i = 0; i < total; ++i) {
        int j = rand() % total;
        char temp = sequence[i];
        sequence[i] = sequence[j];
        sequence[j] = temp;
    }

    // Null-terminate the sequence
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

    // int total = 2*(NO_OF_CARETAKERS+NO_OF_CHILDREN);
    // char *sequence = (char *)malloc((total + 1) * sizeof(char));

    // srand(time(0)); // Seed the random number generator
    // generate_sequence(sequence, NO_OF_CHILDREN, NO_OF_CARETAKERS, NO_OF_CHILDREN, NO_OF_CARETAKERS);
    // printf("Combination of Arrival and departing threads sequence: %s\n", sequence);
   

    char sequence[] = "GCcg";
    int n_threads = strlen(sequence);

    pthread_t *threads = (pthread_t *)malloc(sizeof(pthread_t) * n_threads);
    tdata_t *threads_data = (tdata_t *)malloc(sizeof(tdata_t) * n_threads);

    init_child_care();

    printf("%s: launching threads...\n", __func__);
    for (int i = 0; i < n_threads; ++i) {
        ldata_t *temp = (ldata_t *)malloc(sizeof(ldata_t));
        memset(temp, 0, sizeof(ldata_t));
        temp->tid = i;

        memset(&threads_data[i], 0, sizeof(tdata_t));
        threads_data[i].tid = i;
        threads_data[i].body_arg = temp;

        if(sequence[i]=='G')
        {
            threads_data[i].type = 1 ;
            threads_data[i].body=caregiver_arrives_func;
            pthread_create(&threads[i], NULL, caregiver_arrive_thread, &threads_data[i]);
            sleep(1);
        }
        else if(sequence[i]=='C')
        {
            threads_data[i].type = 3 ;
            threads_data[i].body = child_arrives_func;
            pthread_create(&threads[i], NULL, child_arrive_thread, &threads_data[i]);      
            sleep(1);      
        }
        else if(sequence[i]=='g')
        {
            threads_data[i].type = 2 ;
            threads_data[i].body=caregiver_departs_func;
            pthread_create(&threads[i], NULL, caregiver_depart_thread, &threads_data[i]);
            sleep(1);
        }
        else if(sequence[i]=='c')
        {
            threads_data[i].type = 4 ;
            threads_data[i].body = child_departs_func;
            pthread_create(&threads[i], NULL, child_depart_thread, &threads_data[i]); 
            sleep(1);
        }

    }

    for (int i = 0; i < n_threads; ++i) {
        pthread_join(threads[i], NULL);
    }

    finish_child_care();

    printf("%s: tester done. Dumping data...\n", __func__);
    FILE *fp = fopen("tester-child-care-2/tester-child-care-2-dump.txt", "w");
    if (fp == NULL) {
        fprintf(stderr, "Error opening file for logging\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < n_threads; ++i) {
        char *log = ((ldata_t *)threads_data[i].body_arg)->log;
        int ret = fwrite(log, sizeof(char), strlen(log), fp);
        if (ret != strlen(log)) {
            fprintf(stderr, "fwrite() failed.\n");
            exit(EXIT_FAILURE);
        }
    }

    fprintf(fp, "\n");
    fclose(fp);

    // Sort log file
    system("sort tester-child-care-2/tester-child-care-2-dump.txt >tester-child-care-2/tester-child-care-2-dump-sorted.txt 2>/dev/null");

    free(threads);
    free(threads_data);

    printf("%s: bye!\n", __func__);
    return 0;

}