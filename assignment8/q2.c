#include <stdio.h>
#include <pthread.h>

// Function for even numbers
void* print_even(void* arg) {
    int n = *(int*)arg;

    for (int i = 0; i <= n; i++) {
        if (i % 2 == 0) {
            printf("Even: %d\n", i);
        }
    }
    return NULL;
}

// Function for odd numbers
void* print_odd(void* arg) {
    int n = *(int*)arg;

    for (int i = 0; i <= n; i++) {
        if (i % 2 != 0) {
            printf("Odd: %d\n", i);
        }
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    int limit = 10;

    // Create threads
    pthread_create(&t1, NULL, print_even, &limit);
    pthread_create(&t2, NULL, print_odd, &limit);

    // Wait for both threads to finish
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    return 0;
}