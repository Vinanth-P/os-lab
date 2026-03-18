#include <stdio.h>
#include <limits.h>

#define MAX 20

typedef struct {
    int pid, at, bt, ct, tat, wt, rt, priority;
    int completed;
} Process;

void input(Process p[], int n) {
    for(int i = 0; i < n; i++) {
        printf("\nProcess %d\n", i+1);
        printf("Enter Arrival Time: ");
        scanf("%d", &p[i].at);
        printf("Enter Burst Time: ");
        scanf("%d", &p[i].bt);
        printf("Enter Priority: ");
        scanf("%d", &p[i].priority);

        p[i].pid = i+1;
        p[i].rt = p[i].bt;
        p[i].completed = 0;
    }
}

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

    printf("\nAverage Turnaround Time = %.2f", avg_tat/n);
    printf("\nAverage Waiting Time = %.2f\n", avg_wt/n);
}

/* ---------------- FCFS ---------------- */
void fcfs(Process p[], int n) {
    int time = 0;

    for(int i = 0; i < n-1; i++) {
        for(int j = i+1; j < n; j++) {
            if(p[i].at > p[j].at) {
                Process temp = p[i];
                p[i] = p[j];
                p[j] = temp;
            }
        }
    }

    for(int i = 0; i < n; i++) {
        if(time < p[i].at)
            time = p[i].at;

        time += p[i].bt;
        p[i].ct = time;
    }

    display(p, n);
}

/* ---------------- SJF (Non-Preemptive) ---------------- */
void sjf(Process p[], int n) {
    int time = 0, completed = 0;

    while(completed < n) {
        int idx = -1, min_bt = INT_MAX;

        for(int i = 0; i < n; i++) {
            if(p[i].at <= time && !p[i].completed && p[i].bt < min_bt) {
                min_bt = p[i].bt;
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

/* ---------------- SRTF ---------------- */
void srtf(Process p[], int n) {
    int time = 0, completed = 0;

    while(completed < n) {
        int idx = -1, min_rt = INT_MAX;

        for(int i = 0; i < n; i++) {
            if(p[i].at <= time && !p[i].completed && p[i].rt < min_rt) {
                min_rt = p[i].rt;
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

/* ---------------- Priority (Non-Preemptive) ---------------- */
void priority_np(Process p[], int n) {
    int time = 0, completed = 0;

    while(completed < n) {
        int idx = -1, high = INT_MAX;

        for(int i = 0; i < n; i++) {
            if(p[i].at <= time && !p[i].completed && p[i].priority < high) {
                high = p[i].priority;
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

/* ---------------- Priority (Preemptive) ---------------- */
void priority_p(Process p[], int n) {
    int time = 0, completed = 0;

    while(completed < n) {
        int idx = -1, high = INT_MAX;

        for(int i = 0; i < n; i++) {
            if(p[i].at <= time && !p[i].completed && p[i].priority < high) {
                high = p[i].priority;
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

/* ---------------- MAIN ---------------- */
int main() {
    Process p[MAX], temp[MAX];
    int n, choice;

    printf("Enter number of processes: ");
    scanf("%d", &n);

    input(p, n);

    do {
        // Copy original data
        for(int i = 0; i < n; i++)
            temp[i] = p[i];

        printf("\n\n--- CPU Scheduling ---\n");
        printf("1. FCFS\n2. SJF\n3. SRTF\n4. Priority (Non-Preemptive)\n5. Priority (Preemptive)\n0. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch(choice) {
            case 1: fcfs(temp, n); break;
            case 2: sjf(temp, n); break;
            case 3: srtf(temp, n); break;
            case 4: priority_np(temp, n); break;
            case 5: priority_p(temp, n); break;
        }

    } while(choice != 0);

    return 0;
}