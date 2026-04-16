#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM_READERS 4
#define NUM_WRITERS 2

int shared_data = 0;
int reader_count = 0;

sem_t rw_mutex;   // exclusive access for writers
sem_t mutex;      // protect reader_count

void *reader(void *arg) {
    int id = *(int *)arg;

    for (int i = 0; i < 3; i++) {
        sem_wait(&mutex);
        reader_count++;
        if (reader_count == 1)
            sem_wait(&rw_mutex);   // first reader blocks writers
        sem_post(&mutex);

        printf("Reader %d reads: %d\n", id, shared_data);
        sleep(1);

        sem_wait(&mutex);
        reader_count--;
        if (reader_count == 0)
            sem_post(&rw_mutex);   // last reader unblocks writers
        sem_post(&mutex);

        sleep(1);
    }
    return NULL;
}

void *writer(void *arg) {
    int id = *(int *)arg;

    for (int i = 0; i < 3; i++) {
        sem_wait(&rw_mutex);

        shared_data++;
        printf("Writer %d wrote: %d\n", id, shared_data);
        sleep(2);

        sem_post(&rw_mutex);
        sleep(1);
    }
    return NULL;
}

int main() {
    pthread_t r_threads[NUM_READERS], w_threads[NUM_WRITERS];
    int r_ids[NUM_READERS], w_ids[NUM_WRITERS];

    sem_init(&rw_mutex, 0, 1);
    sem_init(&mutex,    0, 1);

    for (int i = 0; i < NUM_WRITERS; i++) {
        w_ids[i] = i;
        pthread_create(&w_threads[i], NULL, writer, &w_ids[i]);
    }
    for (int i = 0; i < NUM_READERS; i++) {
        r_ids[i] = i;
        pthread_create(&r_threads[i], NULL, reader, &r_ids[i]);
    }

    for (int i = 0; i < NUM_WRITERS; i++) pthread_join(w_threads[i], NULL);
    for (int i = 0; i < NUM_READERS; i++) pthread_join(r_threads[i], NULL);

    sem_destroy(&rw_mutex);
    sem_destroy(&mutex);

    return 0;
}