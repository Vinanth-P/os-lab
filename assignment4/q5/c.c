#include <stdio.h>
#include <unistd.h>

int main() {
    printf("Me: %d\n", getpid());
    printf("Parent: %d\n", getppid());

    // look up parent in ps to find its parent
    sleep(10);
}
