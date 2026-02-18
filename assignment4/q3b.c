#include <unistd.h>
int main() {
    int i;
    for (i = 0; i < 10; i++)
        if (i % 2 == 0) fork();
    return 0;
}
//31
//2^5-1