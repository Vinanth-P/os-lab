#include <stdio.h>
#include <pthread.h>

#define SIZE 10

int arr[SIZE] = {12, 45, 2, 67, 34, 89, 23, 10, 5, 78};
int max_val, min_val;

// Thread function to find maximum
void* find_max(void* arg) {
    max_val = arr[0];
    for (int i = 1; i < SIZE; i++) {
        if (arr[i] > max_val) {
            max_val = arr[i];
        }
    }
    printf("Maximum value: %d\n", max_val);
    return NULL;
}

// Thread function to find minimum
void* find_min(void* arg) {
    min_val = arr[0];
    for (int i = 1; i < SIZE; i++) {
        if (arr[i] < min_val) {
            min_val = arr[i];
        }
    }
    printf("Minimum value: %d\n", min_val);
    return NULL;
}

int main() {
    pthread_t t1, t2;

    // Create threads
    pthread_create(&t1, NULL, find_max, NULL);
    pthread_create(&t2, NULL, find_min, NULL);

    // Wait for both threads
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    return 0;
}