import random
import sys
from collections import deque
from copy import deepcopy

# -----------------------------
# Process Definition
# -----------------------------
class Process:
    def __init__(self, pid, arrival, burst, priority, deadline):
        self.pid = pid
        self.arrival = arrival
        self.burst = burst
        self.remaining = burst
        self.priority = priority
        self.deadline = deadline

        self.start_time = None
        self.completion = None


# -----------------------------
# Random Data Generator
# -----------------------------
def generate_processes(n, seed=42):
    random.seed(seed)
    processes = []
    for i in range(n):
        arrival = random.randint(0, 10)
        burst = random.randint(1, 10)
        priority = random.randint(1, 5)
        deadline = random.randint(arrival + burst, arrival + burst + 10)
        processes.append(Process(i, arrival, burst, priority, deadline))
    return processes


# -----------------------------
# Metric Calculator
# -----------------------------
def print_metrics(processes, total_time, title):
    print(f"\n===== {title} =====")

    total_wait = 0
    total_turn = 0
    total_resp = 0

    for p in processes:
        turnaround = p.completion - p.arrival
        waiting = turnaround - p.burst
        response = p.start_time - p.arrival

        total_wait += waiting
        total_turn += turnaround
        total_resp += response

        print(
            f"P{p.pid}: Wait={waiting}, Resp={response}, Turn={turnaround}"
        )

    n = len(processes)
    print("\nAverages:")
    print("Waiting:", total_wait / n)
    print("Response:", total_resp / n)
    print("Turnaround:", total_turn / n)

    cpu_busy = sum(p.burst for p in processes)
    print("CPU Utilization:", (cpu_busy / total_time) * 100, "%")


# -----------------------------
# FCFS
# -----------------------------
def fcfs(proc_list):
    processes = deepcopy(proc_list)
    time = 0

    processes.sort(key=lambda x: (x.arrival, x.pid))

    for p in processes:
        if time < p.arrival:
            time = p.arrival

        p.start_time = time
        time += p.burst
        p.remaining = 0
        p.completion = time

    print_metrics(processes, time, "FCFS")


# -----------------------------
# SJF (Non-preemptive)
# Arrival ignored as requested
# -----------------------------
def sjf(proc_list):
    processes = deepcopy(proc_list)
    time = 0

    for p in processes:
        p.arrival = 0

    completed = []

    while processes:
        processes.sort(key=lambda x: (x.burst, x.pid))
        p = processes.pop(0)

        p.start_time = time
        time += p.burst
        p.remaining = 0
        p.completion = time

        completed.append(p)

    print_metrics(completed, time, "SJF (Non-preemptive)")


# -----------------------------
# SRTN (Preemptive SJF)
# -----------------------------
def srtn(proc_list):
    processes = deepcopy(proc_list)
    time = 0
    completed = []
    ready = []

    while len(completed) < len(processes):
        for p in processes:
            if p.arrival == time:
                ready.append(p)

        if ready:
            ready.sort(key=lambda x: (x.remaining, x.pid))
            p = ready[0]

            if p.start_time is None:
                p.start_time = time

            p.remaining -= 1

            if p.remaining == 0:
                p.completion = time + 1
                completed.append(p)
                ready.remove(p)

        time += 1

    print_metrics(completed, time, "SRTN (Preemptive)")


# -----------------------------
# Round Robin
# -----------------------------
def round_robin(proc_list, quantum):
    processes = deepcopy(proc_list)
    time = 0
    completed = []
    ready = deque()

    while len(completed) < len(processes):
        for p in processes:
            if p.arrival == time:
                ready.append(p)

        if ready:
            p = ready.popleft()

            if p.start_time is None:
                p.start_time = time

            run = min(quantum, p.remaining)
            for _ in range(run):
                time += 1
                p.remaining -= 1

                for x in processes:
                    if x.arrival == time:
                        ready.append(x)

                if p.remaining == 0:
                    break

            if p.remaining == 0:
                p.completion = time
                completed.append(p)
            else:
                ready.append(p)
        else:
            time += 1

    print_metrics(completed, time, f"Round Robin (q={quantum})")


# -----------------------------
# Priority (Non-preemptive)
# -----------------------------
def priority_non_preemptive(proc_list):
    processes = deepcopy(proc_list)
    time = 0
    completed = []
    ready = []

    while len(completed) < len(processes):
        for p in processes:
            if p.arrival == time:
                ready.append(p)

        if ready:
            ready.sort(key=lambda x: (x.priority, x.pid))
            p = ready.pop(0)

            if p.start_time is None:
                p.start_time = time

            time += p.burst
            p.remaining = 0
            p.completion = time
            completed.append(p)
        else:
            time += 1

    print_metrics(completed, time, "Priority (Non-preemptive)")


# -----------------------------
# MAIN
# -----------------------------
def main():
    if len(sys.argv) < 3:
        print("Usage: python sim.py <algo|all> <num_processes>")
        return

    algo = sys.argv[1].lower()
    n = int(sys.argv[2])

    base_processes = generate_processes(n)

    print("\nGenerated Processes:")
    for p in base_processes:
        print(
            f"P{p.pid}: arrival={p.arrival}, burst={p.burst}, priority={p.priority}, deadline={p.deadline}"
        )

    if algo == "fcfs":
        fcfs(base_processes)

    elif algo == "sjf":
        sjf(base_processes)

    elif algo == "srtn":
        srtn(base_processes)

    elif algo == "rr":
        for q in [2, 4, 6]:
            round_robin(base_processes, q)

    elif algo == "priority":
        priority_non_preemptive(base_processes)

    elif algo == "all":
        fcfs(base_processes)
        sjf(base_processes)
        srtn(base_processes)
        for q in [2, 4, 6]:
            round_robin(base_processes, q)
        priority_non_preemptive(base_processes)


if __name__ == "__main__":
    main()
