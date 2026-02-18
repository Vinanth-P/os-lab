#include <stdio.h>
#include <unistd.h>

int main() {
    if (fork() == 0) {
        printf("Child before exec\n");
        execl("/bin/ls", "ls", NULL);
        printf("This will NOT print\n");
    }
}
