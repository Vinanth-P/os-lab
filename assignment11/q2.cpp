// Q2: Cycle Detection in RAG using DFS (Deadlock Detection)
// For single-instance resources: cycle in wait-for graph => deadlock
// For multi-instance resources: simulation-based detection algorithm
#include <iostream>
#include <vector>
using namespace std;

// ----------------------------------------------------------------
// Wait-For Graph: DFS cycle detection (single-instance resources)
// ----------------------------------------------------------------
struct WaitForGraph {
    int n;
    vector<vector<int>> adj;

    WaitForGraph(int n) : n(n), adj(n) {}

    void addWait(int pi, int pj) { adj[pi].push_back(pj); } // Pi waits for Pj

    bool dfs(int u, vector<bool>& visited, vector<bool>& recStack) {
        visited[u] = recStack[u] = true;
        for (int v : adj[u]) {
            if (!visited[v] && dfs(v, visited, recStack)) return true;
            if (recStack[v]) return true;
        }
        recStack[u] = false;
        return false;
    }

    bool hasCycle() {
        vector<bool> visited(n, false), recStack(n, false);
        for (int i = 0; i < n; i++)
            if (!visited[i] && dfs(i, visited, recStack)) return true;
        return false;
    }

    void display() {
        cout << "\nWait-For Graph edges (Pi -> Pj means Pi waits for Pj):\n";
        bool any = false;
        for (int i = 0; i < n; i++)
            for (int j : adj[i]) { cout << "  P" << i << " --> P" << j << "\n"; any = true; }
        if (!any) cout << "  (none)\n";
    }
};

// ----------------------------------------------------------------
// Multi-instance deadlock detection (simulation)
// ----------------------------------------------------------------
struct DeadlockDetector {
    int n, m;
    vector<int> total;
    vector<vector<int>> allocation, request;

    DeadlockDetector(int n, int m, vector<int> total)
        : n(n), m(m), total(total),
          allocation(n, vector<int>(m, 0)),
          request(n, vector<int>(m, 0)) {}

    void allocate(int pid, int rid, int amt) { allocation[pid][rid] += amt; }
    void addRequest(int pid, int rid, int amt) { request[pid][rid] += amt; }

    vector<int> available() {
        vector<int> avail = total;
        for (int i = 0; i < n; i++)
            for (int j = 0; j < m; j++)
                avail[j] -= allocation[i][j];
        return avail;
    }

    // Returns list of deadlocked process IDs
    vector<int> detect() {
        vector<int> work = available();
        vector<bool> finish(n, false);

        // Processes holding nothing are already "done"
        for (int i = 0; i < n; i++) {
            bool holdsNothing = true;
            for (int j = 0; j < m; j++) if (allocation[i][j]) { holdsNothing = false; break; }
            if (holdsNothing) finish[i] = true;
        }

        bool changed = true;
        while (changed) {
            changed = false;
            for (int i = 0; i < n; i++) {
                if (finish[i]) continue;
                bool canFinish = true;
                for (int j = 0; j < m; j++) if (request[i][j] > work[j]) { canFinish = false; break; }
                if (canFinish) {
                    for (int j = 0; j < m; j++) work[j] += allocation[i][j];
                    finish[i] = true;
                    changed = true;
                }
            }
        }

        vector<int> deadlocked;
        for (int i = 0; i < n; i++) if (!finish[i]) deadlocked.push_back(i);
        return deadlocked;
    }

    void displayState() {
        cout << "\n--- System State ---\n";
        cout << "Allocation:\n         ";
        for (int j = 0; j < m; j++) cout << "R" << j << " ";
        cout << "\n";
        for (int i = 0; i < n; i++) {
            cout << "  P" << i << "     ";
            for (int j = 0; j < m; j++) cout << allocation[i][j] << " ";
            cout << "\n";
        }
        cout << "Request:\n         ";
        for (int j = 0; j < m; j++) cout << "R" << j << " ";
        cout << "\n";
        for (int i = 0; i < n; i++) {
            cout << "  P" << i << "     ";
            for (int j = 0; j < m; j++) cout << request[i][j] << " ";
            cout << "\n";
        }
        auto avail = available();
        cout << "Available: ";
        for (int v : avail) cout << v << " ";
        cout << "\n" << string(40, '-') << "\n";
    }
};

// ----------------------------------------------------------------
// Demo scenarios
// ----------------------------------------------------------------
void scenarioWithCycle() {
    cout << "\n" << string(55, '=') << "\n";
    cout << "SCENARIO 1: System WITH a cycle (deadlock present)\n";
    cout << string(55, '=') << "\n";

    // P0 holds R0, wants R1
    // P1 holds R1, wants R2
    // P2 holds R2, wants R0  => circular wait
    DeadlockDetector det(3, 3, {1, 1, 1});
    det.allocate(0, 0, 1); det.allocate(1, 1, 1); det.allocate(2, 2, 1);
    det.addRequest(0, 1, 1); det.addRequest(1, 2, 1); det.addRequest(2, 0, 1);
    det.displayState();

    WaitForGraph wfg(3);
    wfg.addWait(0, 1); wfg.addWait(1, 2); wfg.addWait(2, 0);
    wfg.display();

    cout << "\nCycle in WFG (DFS): " << (wfg.hasCycle() ? "YES" : "NO") << "\n";

    auto deadlocked = det.detect();
    cout << "Deadlocked processes: ";
    if (deadlocked.empty()) cout << "none";
    else for (int p : deadlocked) cout << "P" << p << " ";
    cout << "\nConclusion: " << (deadlocked.empty() ? "No deadlock" : "DEADLOCK DETECTED") << "\n";
}

void scenarioWithoutCycle() {
    cout << "\n" << string(55, '=') << "\n";
    cout << "SCENARIO 2: System WITHOUT a cycle (no deadlock)\n";
    cout << string(55, '=') << "\n";

    // P0 holds R0, wants R1
    // P1 holds R1, no request  => P1 can finish -> releases R1 -> P0 can proceed
    // P2 holds R2, wants R0
    DeadlockDetector det(3, 3, {1, 1, 1});
    det.allocate(0, 0, 1); det.allocate(1, 1, 1); det.allocate(2, 2, 1);
    det.addRequest(0, 1, 1);
    det.addRequest(2, 0, 1);
    det.displayState();

    WaitForGraph wfg(3);
    wfg.addWait(0, 1);
    wfg.addWait(2, 0);
    wfg.display();

    cout << "\nCycle in WFG (DFS): " << (wfg.hasCycle() ? "YES" : "NO") << "\n";

    auto deadlocked = det.detect();
    cout << "Deadlocked processes: ";
    if (deadlocked.empty()) cout << "none";
    else for (int p : deadlocked) cout << "P" << p << " ";
    cout << "\nConclusion: " << (deadlocked.empty() ? "No deadlock system is safe" : "DEADLOCK DETECTED") << "\n";
}

int main() {
    cout << "=== Q2: Cycle Detection in RAG (DFS-based) ===\n";
    scenarioWithCycle();
    scenarioWithoutCycle();
    return 0;
}