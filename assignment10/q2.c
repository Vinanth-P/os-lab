#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define N 5

sem_t fork_sem[N];

void *philosopher(void *arg) {
    int id = *(int *)arg;
    int left  = id;
    int right = (id + 1) % N;

    for (int i = 0; i < 3; i++) {
        printf("Philosopher %d is thinking\n", id);
        sleep(1);

        // Pick up forks (lower index first to avoid deadlock)
        if (left < right) {
            sem_wait(&fork_sem[left]);
            sem_wait(&fork_sem[right]);
        } else {
            sem_wait(&fork_sem[right]);
            sem_wait(&fork_sem[left]);
        }

        printf("Philosopher %d is eating\n", id);
        sleep(2);

        sem_post(&fork_sem[left]);
        sem_post(&fork_sem[right]);
        printf("Philosopher %d finished eating\n", id);
    }
    return NULL;
}

int main() {
    pthread_t threads[N];
    int ids[N];

    for (int i = 0; i < N; i++)
        sem_init(&fork_sem[i], 0, 1);

    for (int i = 0; i < N; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, philosopher, &ids[i]);
    }

    for (int i = 0; i < N; i++)
        pthread_join(threads[i], NULL);

    for (int i = 0; i < N; i++)
        sem_destroy(&fork_sem[i]);

    return 0;
}