#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int global = 10;

int main() {
    int local = 20;
    static int stat = 30;

    pid_t pid = fork();

    if (pid == 0) {
        global++;
        local++;
        stat++;
        printf("Child: %d %d %d\n", global, local, stat);
    } else {
        sleep(1);
        printf("Parent: %d %d %d\n", global, local, stat);
    }
}
// After fork → memory is copied, not shared.

// Changing in child ≠ change in parent.