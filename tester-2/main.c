#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdalign.h>
#include <stdarg.h>
#include <pthread.h>
#include <string.h>
#include <time.h>

#include "problem-reader-writer/reader-writer-api.h"

// Data used only in tester
struct resource_data {
    // Per-thread data
    int tid;
    char log[8192];

    // Pointer to shared data
    char *db_data;
    size_t db_size;
};
typedef struct resource_data rdata_t;

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

// Reader function
void read_func(void *arg) {
    // printf("%s: start.\n", __func__);
    rdata_t *rd = (rdata_t *)arg;

    // Add some additional delay
    struct timespec sleep_time_dummy = {0, 1000L};
    nanosleep(&sleep_time_dummy, NULL);

    log_msg(rd->log + strlen(rd->log), "tid: %d, read start\n", rd->tid);

    // Read from shared resource
    unsigned int seed = (unsigned int)time(NULL) + rd->tid + 1000;
    char read_data = rd->db_data[rand_r(&seed) % rd->db_size];
    (void)read_data;

    // Assume reading takes some random amount of time
    double random_number = rand_r(&seed) / RAND_MAX;
    struct timespec sleep_time = {1, 500000000L + (long)(random_number * 1000.0)};
    nanosleep(&sleep_time, NULL);

    log_msg(rd->log + strlen(rd->log), "tid: %d, read end\n", rd->tid);
}

// Writer function
void write_func(void *arg) {
    // printf("%s: start.\n", __func__);
    rdata_t *rd = (rdata_t *)arg;

    log_msg(rd->log + strlen(rd->log), "tid: %d, write start\n", rd->tid);

    // Write to shared resource
    unsigned int seed = (unsigned int)time(NULL) + rd->tid + 2000;
    char write_data = (char)rd->tid;
    rd->db_data[rand_r(&seed) % rd->db_size] = write_data;

    // Assume writing takes some random amount of time
    double random_number = rand_r(&seed) / RAND_MAX;
    struct timespec sleep_time = {1, 500000000L + (long)(random_number * 1000.0)};
    nanosleep(&sleep_time, NULL);

    log_msg(rd->log + strlen(rd->log), "tid: %d, write end\n", rd->tid);
}

int main(int argc, char **argv) {
    printf("%s: %s, start!\n", __func__, argv[0]);

    if (argc != 3) {
        fprintf(stderr, "Usage: ./out <n_readers> <n_writers>\n");
        exit(EXIT_FAILURE);
    }

    int n_readers = atoi(argv[1]);
    int n_writers = atoi(argv[2]);
    printf("%s: n_readers: %d, n_writers: %d\n", __func__, n_readers, n_writers);

    // Create shared resource
    size_t db_size = 64;
    char *db_data = (char *)malloc(db_size);
    memset(db_data, 0, db_size);

    pthread_t *readers = (pthread_t *)malloc(sizeof(pthread_t) * n_readers);
    pthread_t *writers = (pthread_t *)malloc(sizeof(pthread_t) * n_writers);
    tdata_t *readers_data = (tdata_t *)malloc(sizeof(tdata_t) * n_readers);
    tdata_t *writers_data = (tdata_t *)malloc(sizeof(tdata_t) * n_writers);

    init_reader_writer();

    // Launch threads
    printf("%s: launching threads...\n", __func__);
    int current_reader = 0;
    int current_writer = 0;
    while ((current_reader < n_readers) || (current_writer < n_writers)) {
        if (current_reader < n_readers) {
            int i = current_reader;
            rdata_t *temp = (rdata_t *)malloc(sizeof(rdata_t));
            memset(temp, 0, sizeof(rdata_t));
            temp->tid = i;
            temp->db_data = db_data;
            temp->db_size = db_size;

            memset(&readers_data[i], 0, sizeof(tdata_t));
            readers_data[i].tid = i;
            readers_data[i].type = 1;
            readers_data[i].body = read_func;
            readers_data[i].body_arg = temp;

            pthread_create(&readers[i], NULL, reader_thread, &readers_data[i]);
            ++current_reader;
        }
        if (current_writer < n_writers) {
            int i = current_writer;
            rdata_t *temp = (rdata_t *)malloc(sizeof(rdata_t));
            memset(temp, 0, sizeof(rdata_t));
            temp->tid = i;
            temp->db_data = db_data;
            temp->db_size = db_size;

            memset(&writers_data[i], 0, sizeof(tdata_t));
            writers_data[i].tid = i;
            writers_data[i].type = 2;
            writers_data[i].body = write_func;
            writers_data[i].body_arg = temp;

            pthread_create(&writers[i], NULL, writer_thread, &writers_data[i]);
            ++current_writer;
        }
    }

    // Join threads
    for (int i = 0; i < n_writers; ++i) {
        pthread_join(writers[i], NULL);
    }
    for (int i = 0; i < n_readers; ++i) {
        pthread_join(readers[i], NULL);
    }

    finish_reader_writer();

    // Dump log
    printf("%s: tester-2 done. Dumping data...\n", __func__);
    FILE *fp = fopen("tester-2-dump.txt", "w");
    for (int i = 0; i < n_readers; ++i) {
        char *log = ((rdata_t *)readers_data[i].body_arg)->log;
        int ret = fwrite(log, sizeof(char), strlen(log), fp);
        if (ret != strlen(log)) {
            fprintf(stderr, "fwrite() failed.\n");
            exit(EXIT_FAILURE);
        }
    }
    for (int i = 0; i < n_writers; ++i) {
        char *log = ((rdata_t *)writers_data[i].body_arg)->log;
        int ret = fwrite(log, sizeof(char), strlen(log), fp);
        if (ret != strlen(log)) {
            fprintf(stderr, "fwrite() failed.\n");
            exit(EXIT_FAILURE);
        }
    }
    fprintf(fp, "\n");
    fclose(fp);

    // Sort log by time
    system("sort tester-2-dump.txt >tester-2-dump-sorted.txt 2>/dev/null");

    // Dump raw data
    FILE *fp_db = fopen("tester-2.db", "w");
    int ret = fwrite(db_data, sizeof(char), db_size, fp_db);
    if (ret != db_size) {
        fprintf(stderr, "fwrite() for db failed.\n");
        exit(EXIT_FAILURE);
    }
    fprintf(fp_db, "\n");
    fclose(fp_db);

    // Free
    for (int i = 0; i < n_readers; ++i) {
        free(readers_data[i].body_arg);
    }
    for (int i = 0; i < n_writers; ++i) {
        free(writers_data[i].body_arg);
    }
    free(writers_data);
    free(readers_data);
    free(writers);
    free(readers);
    free(db_data);

    printf("%s: bye!\n", __func__);
    return 0;
}
