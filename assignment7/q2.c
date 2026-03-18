#include <stdio.h>
#include <limits.h>

#define MAX 20

typedef struct {
    int pid, at, bt, ct, tat, wt, rt;
    int completed;
} Process;

/* -------- INPUT -------- */
void input(Process p[], int n) {
    for(int i = 0; i < n; i++) {
        printf("\nProcess %d\n", i+1);
        printf("Arrival Time: ");
        scanf("%d", &p[i].at);
        printf("Burst Time: ");
        scanf("%d", &p[i].bt);

        p[i].pid = i+1;
        p[i].rt = p[i].bt;
        p[i].completed = 0;
    }
}

/* -------- DISPLAY -------- */
void display(Process p[], int n) {
    float avg_tat = 0, avg_wt = 0;

    printf("\nPID\tAT\tBT\tCT\tTAT\tWT\n");

    for(int i = 0; i < n; i++) {
        p[i].tat = p[i].ct - p[i].at;
        p[i].wt = p[i].tat - p[i].bt;

        avg_tat += p[i].tat;
        avg_wt += p[i].wt;

        printf("%d\t%d\t%d\t%d\t%d\t%d\n",
               p[i].pid, p[i].at, p[i].bt,
               p[i].ct, p[i].tat, p[i].wt);
    }

    printf("\nAverage TAT = %.2f", avg_tat/n);
    printf("\nAverage WT = %.2f\n", avg_wt/n);
}

/* -------- ROUND ROBIN -------- */
void round_robin(Process p[], int n, int tq) {
    int time = 0, completed = 0;
    int queue[MAX], front = 0, rear = 0;
    int visited[MAX] = {0};

    // Add first arriving process
    for(int i = 0; i < n; i++) {
        if(p[i].at == 0) {
            queue[rear++] = i;
            visited[i] = 1;
        }
    }

    while(front < rear) {
        int i = queue[front++];

        if(p[i].rt > tq) {
            time += tq;
            p[i].rt -= tq;
        } else {
            time += p[i].rt;
            p[i].rt = 0;
            p[i].ct = time;
            p[i].completed = 1;
            completed++;
        }

        // Add newly arrived processes
        for(int j = 0; j < n; j++) {
            if(p[j].at <= time && !visited[j]) {
                queue[rear++] = j;
                visited[j] = 1;
            }
        }

        // If process not finished, re-add to queue
        if(p[i].rt > 0)
            queue[rear++] = i;
    }

    display(p, n);
}

/* -------- LJF (Non-Preemptive) -------- */
void ljf(Process p[], int n) {
    int time = 0, completed = 0;

    while(completed < n) {
        int idx = -1, max_bt = -1;

        for(int i = 0; i < n; i++) {
            if(p[i].at <= time && !p[i].completed && p[i].bt > max_bt) {
                max_bt = p[i].bt;
                idx = i;
            }
        }

        if(idx != -1) {
            time += p[idx].bt;
            p[idx].ct = time;
            p[idx].completed = 1;
            completed++;
        } else {
            time++;
        }
    }

    display(p, n);
}

/* -------- LRTF (Preemptive) -------- */
void lrtf(Process p[], int n) {
    int time = 0, completed = 0;

    while(completed < n) {
        int idx = -1, max_rt = -1;

        for(int i = 0; i < n; i++) {
            if(p[i].at <= time && !p[i].completed && p[i].rt > max_rt) {
                max_rt = p[i].rt;
                idx = i;
            }
        }

        if(idx != -1) {
            p[idx].rt--;
            time++;

            if(p[idx].rt == 0) {
                p[idx].ct = time;
                p[idx].completed = 1;
                completed++;
            }
        } else {
            time++;
        }
    }

    display(p, n);
}

/* -------- MAIN -------- */
int main() {
    Process p[MAX], temp[MAX];
    int n, choice, tq;

    printf("Enter number of processes: ");
    scanf("%d", &n);

    input(p, n);

    do {
        for(int i = 0; i < n; i++)
            temp[i] = p[i];

        printf("\n\n--- Scheduling Algorithms ---\n");
        printf("1. Round Robin\n2. Longest Job First\n3. Longest Remaining Time First\n0. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch(choice) {
            case 1:
                printf("Enter Time Quantum: ");
                scanf("%d", &tq);
                round_robin(temp, n, tq);
                break;

            case 2:
                ljf(temp, n);
                break;

            case 3:
                lrtf(temp, n);
                break;
        }

    } while(choice != 0);

    return 0;
}