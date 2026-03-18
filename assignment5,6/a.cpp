#include <bits/stdc++.h>
using namespace std;

struct Process {
    int pid;
    int arrival;
    int burst;
    int remaining;
    int priority;
    int deadline;

    int start_time = -1;
    int completion = 0;
};

// -----------------------------
// Random Generator
// -----------------------------
vector<Process> generate_processes(int n, int seed = 42) {
    mt19937 rng(seed);

    uniform_int_distribution<int> arrival(0, 10);
    uniform_int_distribution<int> burst(1, 10);
    uniform_int_distribution<int> priority(1, 5);

    vector<Process> v;

    for (int i = 0; i < n; ++i) {
        Process p;
        p.pid = i;
        p.arrival = arrival(rng);
        p.burst = burst(rng);
        p.remaining = p.burst;
        p.priority = priority(rng);
        p.deadline = p.arrival + p.burst + (rng() % 10);
        v.push_back(p);
    }
    return v;
}

// -----------------------------
// Metrics
// -----------------------------
void print_metrics(vector<Process>& p, int total_time, string title) {
    cout << "\n===== " << title << " =====\n";

    double w = 0, t = 0, r = 0;

    for (auto& x : p) {
        int turnaround = x.completion - x.arrival;
        int waiting = turnaround - x.burst;
        int response = x.start_time - x.arrival;

        w += waiting;
        t += turnaround;
        r += response;

        cout << "P" << x.pid
             << " Wait=" << waiting
             << " Resp=" << response
             << " Turn=" << turnaround << "\n";
    }

    int n = p.size();
    cout << "\nAverages:\n";
    cout << "Waiting: " << w / n << "\n";
    cout << "Response: " << r / n << "\n";
    cout << "Turnaround: " << t / n << "\n";

    int busy = 0;
    for (auto& x : p) busy += x.burst;

    cout << "CPU Utilization: " << (busy * 100.0 / total_time) << "%\n";
}

// -----------------------------
// FCFS
// -----------------------------
void fcfs(vector<Process> base) {
    auto p = base;
    sort(p.begin(), p.end(), [](auto& a, auto& b) {
        if (a.arrival == b.arrival) return a.pid < b.pid;
        return a.arrival < b.arrival;
    });

    int time = 0;

    for (auto& x : p) {
        if (time < x.arrival) time = x.arrival;
        x.start_time = time;
        time += x.burst;
        x.remaining = 0;
        x.completion = time;
    }

    print_metrics(p, time, "FCFS");
}

// -----------------------------
// SJF (arrival ignored)
// -----------------------------
void sjf(vector<Process> base) {
    auto p = base;
    for (auto& x : p) x.arrival = 0;

    sort(p.begin(), p.end(), [](auto& a, auto& b) {
        if (a.burst == b.burst) return a.pid < b.pid;
        return a.burst < b.burst;
    });

    int time = 0;
    for (auto& x : p) {
        x.start_time = time;
        time += x.burst;
        x.remaining = 0;
        x.completion = time;
    }

    print_metrics(p, time, "SJF (Non-preemptive)");
}

// -----------------------------
// SRTN
// -----------------------------
void srtn(vector<Process> base) {
    auto p = base;
    int n = p.size();
    int time = 0, done = 0;

    vector<bool> finished(n, false);

    while (done < n) {
        int idx = -1;
        int best = 1e9;

        for (int i = 0; i < n; ++i) {
            if (!finished[i] && p[i].arrival <= time && p[i].remaining > 0) {
                if (p[i].remaining < best) {
                    best = p[i].remaining;
                    idx = i;
                }
            }
        }

        if (idx == -1) {
            time++;
            continue;
        }

        if (p[idx].start_time == -1)
            p[idx].start_time = time;

        p[idx].remaining--;
        time++;

        if (p[idx].remaining == 0) {
            p[idx].completion = time;
            finished[idx] = true;
            done++;
        }
    }

    print_metrics(p, time, "SRTN (Preemptive)");
}

// -----------------------------
// Round Robin
// -----------------------------
void round_robin(vector<Process> base, int q) {
    auto p = base;
    int n = p.size();
    int time = 0, done = 0;

    queue<int> ready;
    vector<bool> inqueue(n, false);

    while (done < n) {
        for (int i = 0; i < n; ++i) {
            if (!inqueue[i] && p[i].arrival <= time && p[i].remaining > 0) {
                ready.push(i);
                inqueue[i] = true;
            }
        }

        if (ready.empty()) {
            time++;
            continue;
        }

        int i = ready.front();
        ready.pop();
        inqueue[i] = false;

        if (p[i].start_time == -1)
            p[i].start_time = time;

        int run = min(q, p[i].remaining);

        for (int k = 0; k < run; ++k) {
            p[i].remaining--;
            time++;

            for (int j = 0; j < n; ++j) {
                if (!inqueue[j] && p[j].arrival <= time && p[j].remaining > 0) {
                    ready.push(j);
                    inqueue[j] = true;
                }
            }

            if (p[i].remaining == 0) break;
        }

        if (p[i].remaining == 0) {
            p[i].completion = time;
            done++;
        } else {
            ready.push(i);
            inqueue[i] = true;
        }
    }

    print_metrics(p, time, "Round Robin (q=" + to_string(q) + ")");
}

// -----------------------------
// Priority (Non-preemptive)
// -----------------------------
void priority_np(vector<Process> base) {
    auto p = base;
    int n = p.size();
    int time = 0, done = 0;
    vector<bool> finished(n, false);

    while (done < n) {
        int idx = -1;
        int best = 1e9;

        for (int i = 0; i < n; ++i) {
            if (!finished[i] && p[i].arrival <= time) {
                if (p[i].priority < best) {
                    best = p[i].priority;
                    idx = i;
                }
            }
        }

        if (idx == -1) {
            time++;
            continue;
        }

        p[idx].start_time = time;
        time += p[idx].burst;
        p[idx].remaining = 0;
        p[idx].completion = time;

        finished[idx] = true;
        done++;
    }

    print_metrics(p, time, "Priority (Non-preemptive)");
}

// -----------------------------
// MAIN
// -----------------------------
int main() {
    int n;
    int choice;

    cout << "Enter number of processes: ";
    cin >> n;

    if (n <= 0) {
        cout << "Invalid number of processes.\n";
        return 0;
    }

    auto base = generate_processes(n);

    cout << "\nGenerated Processes:\n";
    for (auto& p : base) {
        cout << "P" << p.pid
             << " arrival=" << p.arrival
             << " burst=" << p.burst
             << " priority=" << p.priority
             << " deadline=" << p.deadline
             << "\n";
    }

    cout << "\nChoose Scheduling Algorithm:\n";
    cout << "1. FCFS\n";
    cout << "2. SJF (Non-preemptive)\n";
    cout << "3. SRTN (Preemptive)\n";
    cout << "4. Round Robin\n";
    cout << "5. Priority (Non-preemptive)\n";
    cout << "6. Run All\n";
    cout << "Enter choice: ";
    cin >> choice;

    switch (choice) {
        case 1:
            fcfs(base);
            break;

        case 2:
            sjf(base);
            break;

        case 3:
            srtn(base);
            break;

        case 4: {
            int q;
            cout << "Enter time quantum: ";
            cin >> q;
            round_robin(base, q);
            break;
        }

        case 5:
            priority_np(base);
            break;

        case 6:
            fcfs(base);
            sjf(base);
            srtn(base);
            round_robin(base, 2);
            round_robin(base, 4);
            round_robin(base, 6);
            priority_np(base);
            break;

        default:
            cout << "Invalid choice.\n";
    }

    return 0;
}
