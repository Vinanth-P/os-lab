#include <stdio.h>
#include <pthread.h>

#define SIZE 3

int A[SIZE][SIZE] = {
    {1, 2, 3},
    {4, 5, 6},
    {7, 8, 9}
};

int B[SIZE][SIZE] = {
    {9, 8, 7},
    {6, 5, 4},
    {3, 2, 1}
};

int C[SIZE][SIZE];  // Result matrix

// Thread function to compute one row
void* add_row(void* arg) {
    int row = *(int*)arg;

    for (int j = 0; j < SIZE; j++) {
        C[row][j] = A[row][j] + B[row][j];
    }

    return NULL;
}

int main() {
    pthread_t threads[SIZE];
    int row_index[SIZE];

    // Create one thread per row
    for (int i = 0; i < SIZE; i++) {
        row_index[i] = i;
        pthread_create(&threads[i], NULL, add_row, &row_index[i]);
    }

    // Wait for all threads
    for (int i = 0; i < SIZE; i++) {
        pthread_join(threads[i], NULL);
    }

    // Print result matrix
    printf("Resultant Matrix:\n");
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            printf("%d ", C[i][j]);
        }
        printf("\n");
    }

    return 0;
}