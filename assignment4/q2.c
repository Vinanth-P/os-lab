#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    // 1. Input Validation
    // Check if the user provided an argument
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <positive integer>\n", argv[0]);
        return 1;
    }

    // Convert string argument to integer
    int n = atoi(argv[1]);

    // Ensure the integer is positive
    if (n <= 0) {
        fprintf(stderr, "Error: Please provide a positive integer.\n");
        return 1;
    }

    // 2. Create the child process
    pid_t pid = fork();

    // 3. Process Logic
    if (pid < 0) {
        // Fork failed
        fprintf(stderr, "Fork failed.\n");
        return 1;
    } else if (pid == 0) {
        // --- Child Process ---
        printf("Child Process (PID %d): Generating sequence...\n", getpid());
        printf("%d", n); // Print starting number

        while (n != 1) {
            if (n % 2 == 0) {
                // If n is even, divide by 2
                n = n / 2;
            } else {
                // If n is odd, multiply by 3 and add 1
                n = 3 * n + 1;
            }
            printf(", %d", n);
        }
        
        printf("\n");
        printf("Child Process: Done.\n");
        exit(0); // Exit child process to signal completion
    } else {
        // --- Parent Process ---
        printf("Parent Process (PID %d): Waiting for child...\n", getpid());
        
        // Wait for the child to complete
        wait(NULL);
        
        printf("Parent Process: Child has completed.\n");
    }

    return 0;
}