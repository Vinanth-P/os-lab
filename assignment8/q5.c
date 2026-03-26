#include <stdio.h>
#include <pthread.h>

int n;
long long factorial = 1;
int sum = 0;

// Thread function to compute factorial
void* compute_factorial(void* arg) {
    for (int i = 1; i <= n; i++) {
        factorial *= i;
    }
    printf("Factorial of %d = %lld\n", n, factorial);
    return NULL;
}

// Thread function to compute sum
void* compute_sum(void* arg) {
    for (int i = 1; i <= n; i++) {
        sum += i;
    }
    printf("Sum of first %d natural numbers = %d\n", n, sum);
    return NULL;
}

int main() {
    pthread_t t1, t2;

    printf("Enter a number: ");
    scanf("%d", &n);

    // Create threads
    pthread_create(&t1, NULL, compute_factorial, NULL);
    pthread_create(&t2, NULL, compute_sum, NULL);

    // Wait for both threads to complete
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    return 0;
}