#include <iostream>
#include <vector>
using namespace std;

int R, P;
vector<int> available;
vector<vector<int>> need;
vector<vector<int>> allocation;

bool isSafe(vector<int>& safeSeq) {
    vector<int> work = available;
    vector<bool> finish(P, false);
    safeSeq.clear();

    while (true) {
        bool found = false;

        for (int i = 0; i < P; i++) {
            if (!finish[i]) {
                bool canRun = true;

                for (int j = 0; j < R; j++) {
                    if (need[i][j] > work[j]) {
                        canRun = false;
                        break;
                    }
                }

                if (canRun) {
                    for (int j = 0; j < R; j++)
                        work[j] += allocation[i][j];

                    finish[i] = true;
                    safeSeq.push_back(i);
                    found = true;
                }
            }
        }

        if (!found) break;
    }

    for (bool f : finish)
        if (!f) return false;

    return true;
}

bool requestResources(int pid, vector<int> req) {
    for (int j = 0; j < R; j++)
        if (req[j] > need[pid][j] || req[j] > available[j]) {
            cout << "Request DENIED\n";
            return false;
        }

    // Try allocation
    for (int j = 0; j < R; j++) {
        available[j] -= req[j];
        allocation[pid][j] += req[j];
        need[pid][j] -= req[j];
    }

    vector<int> safeSeq;
    if (isSafe(safeSeq)) {
        cout << "Request GRANTED\nSafe Sequence: ";
        for (int x : safeSeq) cout << "P" << x << " ";
        cout << endl;

        // Check completion
        bool done = true;
        for (int j = 0; j < R; j++)
            if (need[pid][j] != 0) done = false;

        if (done) {
            cout << "Process P" << pid << " completed. Releasing resources.\n";
            for (int j = 0; j < R; j++) {
                available[j] += allocation[pid][j];
                allocation[pid][j] = 0;
            }
        }

        return true;
    } else {
        // Rollback
        for (int j = 0; j < R; j++) {
            available[j] += req[j];
            allocation[pid][j] -= req[j];
            need[pid][j] += req[j];
        }

        cout << "Request DENIED (Unsafe)\n";
        return false;
    }
}

void releaseResources(int pid, vector<int> rel) {
    for (int j = 0; j < R; j++) {
        allocation[pid][j] -= rel[j];
        available[j] += rel[j];
        need[pid][j] += rel[j];
    }
    cout << "Resources released\n";
}

int main() {
    cout << "Enter number of resources: ";
    cin >> R;

    available.resize(R);
    cout << "Enter instances of each resource:\n";
    for (int i = 0; i < R; i++)
        cin >> available[i];

    cout << "Enter number of processes: ";
    cin >> P;

    need.resize(P, vector<int>(R));
    allocation.resize(P, vector<int>(R, 0));

    cout << "Enter NEED matrix:\n";
    for (int i = 0; i < P; i++)
        for (int j = 0; j < R; j++)
            cin >> need[i][j];

    cout << "\nSystem Ready.\n";
    cout << "Enter: pid a/r r1 r2 r3 ...\n";

    while (true) {
        int pid;
        char type;
        vector<int> vec(R);

        cin >> pid >> type;

        for (int i = 0; i < R; i++)
            cin >> vec[i];

        if (type == 'a')
            requestResources(pid, vec);
        else if (type == 'r')
            releaseResources(pid, vec);

        // Check all done
        bool allDone = true;
        for (int i = 0; i < P; i++)
            for (int j = 0; j < R; j++)
                if (need[i][j] != 0)
                    allDone = false;

        if (allDone) {
            cout << "All processes finished.\n";
            break;
        }
    }

    return 0;
}
// Enter number of resources: 4
// Enter instances of each resource:
// 2 4 5 3
// Enter number of processes: 3
// Enter NEED matrix:
// 1 1 1 1
// 2 3 1 2
// 2 2 1 3

// System Ready.
// Enter: pid a/r r1 r2 r3 ...

// 0 a 1 0 1 1
// 1 a 1 1 0 0
// 2 r 0 0 0 1