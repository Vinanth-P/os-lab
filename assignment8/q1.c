#include <stdio.h>
#include <pthread.h>

// Function to be executed by the child thread
void* thread_function(void* arg) {
    printf("Hello from the child thread!\n");
    return NULL;
}

int main() {
    pthread_t thread_id;

    // Create a new thread
    if (pthread_create(&thread_id, NULL, thread_function, NULL) != 0) {
        printf("Failed to create thread\n");
        return 1;
    }

    // Message from main thread
    printf("Hello from the main thread!\n");

    // Wait for the child thread to finish
    pthread_join(thread_id, NULL);

    return 0;
}
// thread_create(thread, attr, function, argument)
// &thread_id → where the thread ID will be stored
// NULL → default thread attributes
// thread_function → function the thread will run
// NULL → no argument passed to the function