#include <stdio.h>
#include <pthread.h>

int counter = 0;  // Shared variable

// Thread function
void* increment(void* arg) {
    for (int i = 0; i < 100000; i++) {
        counter++;   // Critical section (not protected)
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;

    // Create two threads
    pthread_create(&t1, NULL, increment, NULL);
    pthread_create(&t2, NULL, increment, NULL);

    // Wait for both threads
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("Final counter value: %d\n", counter);

    return 0;
}