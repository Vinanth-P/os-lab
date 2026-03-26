#include <stdio.h>
#include <string.h>

#define MAX 10

typedef struct {
    int pid, arrival, burst;
    int remaining, finish, waiting, turnaround;
} Process;

void input(Process p[], int n) {
    for (int i = 0; i < n; i++) {
        p[i].pid = i + 1;
        printf("Process %d - Arrival Time: ", i + 1);
        scanf("%d", &p[i].arrival);
        printf("Process %d - Burst Time: ", i + 1);
        scanf("%d", &p[i].burst);
        p[i].remaining = p[i].burst;
        p[i].finish = p[i].waiting = p[i].turnaround = 0;
    }
}

void printResults(Process p[], int n) {
    printf("\n%-5s %-8s %-8s %-10s %-12s %-10s\n",
           "PID", "Arrival", "Burst", "Finish", "Turnaround", "Waiting");
    float avgTAT = 0, avgWT = 0;
    for (int i = 0; i < n; i++) {
        p[i].turnaround = p[i].finish - p[i].arrival;
        p[i].waiting    = p[i].turnaround - p[i].burst;
        printf("%-5d %-8d %-8d %-10d %-12d %-10d\n",
               p[i].pid, p[i].arrival, p[i].burst,
               p[i].finish, p[i].turnaround, p[i].waiting);
        avgTAT += p[i].turnaround;
        avgWT  += p[i].waiting;
    }
    printf("Average Turnaround Time: %.2f\n", avgTAT / n);
    printf("Average Waiting Time   : %.2f\n", avgWT  / n);
}

/* Round Robin */
void roundRobin(Process p[], int n, int quantum) {
    int time = 0, done = 0, completed[MAX] = {0};
    for (int i = 0; i < n; i++) p[i].remaining = p[i].burst;

    while (done < n) {
        int found = 0;
        for (int i = 0; i < n; i++) {
            if (!completed[i] && p[i].arrival <= time && p[i].remaining > 0) {
                found = 1;
                int exec = (p[i].remaining < quantum) ? p[i].remaining : quantum;
                p[i].remaining -= exec;
                time += exec;
                if (p[i].remaining == 0) {
                    p[i].finish = time;
                    completed[i] = 1;
                    done++;
                }
            }
        }
        if (!found) time++;
    }
    printf("\n=== Round Robin (Quantum = %d) ===", quantum);
    printResults(p, n);
}

/* Longest Job First - Non-Preemptive */
void ljf(Process p[], int n) {
    int done = 0, time = 0, completed[MAX] = {0};
    while (done < n) {
        int idx = -1, maxB = -1;
        for (int i = 0; i < n; i++)
            if (!completed[i] && p[i].arrival <= time && p[i].burst > maxB) {
                maxB = p[i].burst; idx = i;
            }
        if (idx == -1) { time++; continue; }
        time += p[idx].burst;
        p[idx].finish = time;
        completed[idx] = 1;
        done++;
    }
    printf("\n=== Longest Job First (Non-Preemptive) ===");
    printResults(p, n);
}

/* Longest Remaining Time First - Preemptive */
void lrtf(Process p[], int n) {
    int done = 0, time = 0, completed[MAX] = {0};
    for (int i = 0; i < n; i++) p[i].remaining = p[i].burst;

    while (done < n) {
        int idx = -1, maxR = -1;
        for (int i = 0; i < n; i++)
            if (!completed[i] && p[i].arrival <= time && p[i].remaining > maxR) {
                maxR = p[i].remaining; idx = i;
            }
        if (idx == -1) { time++; continue; }
        p[idx].remaining--;
        time++;
        if (p[idx].remaining == 0) {
            p[idx].finish = time;
            completed[idx] = 1;
            done++;
        }
    }
    printf("\n=== Longest Remaining Time First (Preemptive) ===");
    printResults(p, n);
}

int main() {
    int n, choice, quantum;
    Process original[MAX], p[MAX];

    printf("Enter number of processes (max %d): ", MAX);
    scanf("%d", &n);

    while (1) {
        printf("\n====== Q2: CPU Scheduling Menu ======\n");
        printf("1. Round Robin\n");
        printf("2. Longest Job First (Non-Preemptive)\n");
        printf("3. Longest Remaining Time First (Preemptive)\n");
        printf("0. Exit\n");
        printf("Choice: ");
        scanf("%d", &choice);

        if (choice == 0) break;

        printf("\nEnter process details:\n");
        input(original, n);
        memcpy(p, original, sizeof(Process) * n);

        switch (choice) {
            case 1:
                printf("Enter time quantum: ");
                scanf("%d", &quantum);
                roundRobin(p, n, quantum);
                break;
            case 2: ljf(p, n); break;
            case 3: lrtf(p, n); break;
            default: printf("Invalid choice.\n");
        }
    }
    return 0;
}