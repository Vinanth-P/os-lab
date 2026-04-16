// Q1: Resource Allocation Graph (RAG)
#include <iostream>
#include <vector>
#include <string>
using namespace std;

struct RAG {
    int n, m; // processes, resources
    vector<vector<int>> allocation; // allocation[i][j] = instances of Rj held by Pi
    vector<vector<int>> request;   // request[i][j] = instances of Rj requested by Pi

    RAG(int n, int m) : n(n), m(m),
        allocation(n, vector<int>(m, 0)),
        request(n, vector<int>(m, 0)) {}

    void allocate(int pid, int rid, int amt) { allocation[pid][rid] += amt; }
    void addRequest(int pid, int rid, int amt) { request[pid][rid] += amt; }

    void display() {
        cout << "\n========== Resource Allocation Graph ==========\n";

        cout << "\nAllocation edges (Resource -> Process):\n";
        bool any = false;
        for (int i = 0; i < n; i++)
            for (int j = 0; j < m; j++)
                if (allocation[i][j]) {
                    cout << "  R" << j << " --(" << allocation[i][j] << ")--> P" << i << "\n";
                    any = true;
                }
        if (!any) cout << "  (none)\n";

        cout << "\nRequest edges (Process -> Resource):\n";
        any = false;
        for (int i = 0; i < n; i++)
            for (int j = 0; j < m; j++)
                if (request[i][j]) {
                    cout << "  P" << i << " --(" << request[i][j] << ")--> R" << j << "\n";
                    any = true;
                }
        if (!any) cout << "  (none)\n";

        cout << "\nMatrix view (A=Allocated, Req=Requested):\n";
        cout << "         ";
        for (int j = 0; j < m; j++) cout << "  R" << j << "  ";
        cout << "\n";
        for (int i = 0; i < n; i++) {
            cout << "  P" << i << " Alloc:";
            for (int j = 0; j < m; j++) cout << "  " << allocation[i][j] << "    ";
            cout << "\n";
            cout << "     Req  :";
            for (int j = 0; j < m; j++) cout << "  " << request[i][j] << "    ";
            cout << "\n";
        }
        cout << "================================================\n";
    }
};

int main() {
    cout << "=== Q1: Resource Allocation Graph ===\n";
    // 3 processes, 3 resources
    RAG rag(3, 3);

    rag.allocate(0, 0, 1);   // P0 holds R0
    rag.allocate(1, 1, 1);   // P1 holds R1
    rag.allocate(2, 2, 1);   // P2 holds R2

    rag.addRequest(0, 1, 1); // P0 wants R1
    rag.addRequest(1, 2, 1); // P1 wants R2
    rag.addRequest(2, 0, 1); // P2 wants R0

    rag.display();
    return 0;
}