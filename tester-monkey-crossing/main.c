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

#include "problem-monkey-crossing/monkey-crossing-api.h"


struct log_data {
    
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

// Eastbound monkey function
void east_func(void *arg) {
    ldata_t *rd = (ldata_t *)arg;

    log_msg(rd->log + strlen(rd->log), "tid: %d, east monkey start\n", rd->tid);

    sleep(1);

    log_msg(rd->log + strlen(rd->log), "tid: %d, east monkey end\n", rd->tid);
}

// Westbound monkey function
void west_func(void *arg) {
    ldata_t *rd = (ldata_t *)arg;

    log_msg(rd->log + strlen(rd->log), "tid: %d, west monkey start\n", rd->tid);

    sleep(1);

    log_msg(rd->log + strlen(rd->log), "tid: %d, west monkey end\n", rd->tid);
}

void generate_crossing_sequence(char *sequence, int length) {
    for (int i = 0; i < length; ++i) {
        sequence[i] = (rand() % 2 == 0) ? 'E' : 'W';
    }
    sequence[length] = '\0';
}


int main(int argc, char **argv) {
    printf("%s: %s, start!\n", __func__, argv[0]);

    if (argc != 3) {
        fprintf(stderr, "Usage: ./out <ROPE_CAPACITY> <SEQUENCE_LENGTH>\n");
        exit(EXIT_FAILURE);
    }

    int ROPE_CAPACITY = atoi(argv[1]);
    int SEQUENCE_LENGTH = atoi(argv[2]);
    printf("%s: ROPE_CAPACITY: %d, SEQUENCE_LENGTH: %d\n", __func__, ROPE_CAPACITY, SEQUENCE_LENGTH);

    
    char *crossing_sequence = (char *)malloc(SEQUENCE_LENGTH + 1);
    generate_crossing_sequence(crossing_sequence, SEQUENCE_LENGTH);
    printf("Generated crossing sequence: %s\n", crossing_sequence);


    int n_monkeys = strlen(crossing_sequence);

    pthread_t *monkey_threads = (pthread_t *)malloc(sizeof(pthread_t) * n_monkeys);
    tdata_t *monkey_data = (tdata_t *)malloc(sizeof(tdata_t) * n_monkeys);

    init_monkey_crossing(ROPE_CAPACITY);

    // Launch threads based on the specified crossing sequence
    printf("%s: launching threads...\n", __func__);
    for (int i = 0; i < n_monkeys; ++i) {
        ldata_t *temp = (ldata_t *)malloc(sizeof(ldata_t));
        memset(temp, 0, sizeof(ldata_t));
        temp->tid = i;

        memset(&monkey_data[i], 0, sizeof(tdata_t));
        monkey_data[i].tid = i;
        monkey_data[i].body_arg = temp;

        // Launch the appropriate thread based on the crossing sequence
        if (crossing_sequence[i] == 'E') {
            monkey_data[i].type = 1;
            monkey_data[i].body = east_func;
            pthread_create(&monkey_threads[i], NULL, eastbound_thread, &monkey_data[i]);
        } else if (crossing_sequence[i] == 'W') {
            monkey_data[i].type = 2;
            monkey_data[i].body = west_func;
            pthread_create(&monkey_threads[i], NULL, westbound_thread, &monkey_data[i]);
        }
    }
    // Join threads
    for (int i = 0; i < n_monkeys; ++i) {
        pthread_join(monkey_threads[i], NULL);
    }

    finish_monkey_crossing();

    // Dump log
    printf("%s: tester-monkey-crossing done. Dumping data...\n", __func__);
    FILE *fp = fopen("tester-monkey-crossing/tester-monkey-crossing-dump.txt", "w");
    if (!fp) {
        perror("Failed to open log file");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < n_monkeys; ++i) {
        char *log = ((ldata_t *)monkey_data[i].body_arg)->log;
        int ret = fwrite(log, sizeof(char), strlen(log), fp);
        if (ret != strlen(log)) {
            fprintf(stderr, "fwrite() failed for monkey %d\n", i);
            fclose(fp);
            exit(EXIT_FAILURE);
        }
    }
    fprintf(fp, "\n");
    fclose(fp);

    // Sort log by time
    if (system("sort tester-monkey-crossing/tester-monkey-crossing-dump.txt >tester-monkey-crossing/tester-monkey-crossing-dump-sorted.txt 2>/dev/null") == -1) {
        fprintf(stderr, "Error sorting log file.\n");
        exit(EXIT_FAILURE);
    }
    
    // Free resources
    for (int i = 0; i < n_monkeys; ++i) {
        free(monkey_data[i].body_arg);
    }
    free(monkey_data);
    free(monkey_threads);

    printf("%s: bye!\n", __func__);
    return 0;

}