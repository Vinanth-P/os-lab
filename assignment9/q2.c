#include <stdio.h>

#define MAX 10

int main() {
    int n, m;
    int allocation[MAX][MAX], max[MAX][MAX], need[MAX][MAX];
    int available[MAX], work[MAX];
    int finish[MAX], safeSequence[MAX];

    printf("Enter number of processes: ");
    scanf("%d", &n);

    printf("Enter number of resources: ");
    scanf("%d", &m);

    // Allocation matrix
    printf("Enter Allocation Matrix:\n");
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            scanf("%d", &allocation[i][j]);

    // Max matrix
    printf("Enter Max Matrix:\n");
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            scanf("%d", &max[i][j]);

    // Available resources
    printf("Enter Available Resources:\n");
    for (int j = 0; j < m; j++)
        scanf("%d", &available[j]);

    // Compute Need matrix
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            need[i][j] = max[i][j] - allocation[i][j];

    // 🔹 Take request
    int process;
    int request[MAX];

    printf("Enter process number making request: ");
    scanf("%d", &process);

    printf("Enter request vector:\n");
    for (int j = 0; j < m; j++)
        scanf("%d", &request[j]);

    //  Step 1: Check Request <= Need
    for (int j = 0; j < m; j++) {
        if (request[j] > need[process][j]) {
            printf("Error: Request exceeds maximum need.\n");
            return 0;
        }
    }

    //  Step 2: Check Request <= Available
    for (int j = 0; j < m; j++) {
        if (request[j] > available[j]) {
            printf("Resources not available. Process must wait.\n");
            return 0;
        }
    }

    // Step 3: Temporarily allocate
    for (int j = 0; j < m; j++) {
        available[j] -= request[j];
        allocation[process][j] += request[j];
        need[process][j] -= request[j];
    }

    //  Step 4: Safety Algorithm
    for (int i = 0; i < m; i++)
        work[i] = available[i];

    for (int i = 0; i < n; i++)
        finish[i] = 0;

    int count = 0;

    while (count < n) {
        int found = 0;

        for (int i = 0; i < n; i++) {
            if (finish[i] == 0) {
                int j;
                for (j = 0; j < m; j++) {
                    if (need[i][j] > work[j])
                        break;
                }

                if (j == m) {
                    for (int k = 0; k < m; k++)
                        work[k] += allocation[i][k];

                    safeSequence[count++] = i;
                    finish[i] = 1;
                    found = 1;
                }
            }
        }

        if (!found) {
            printf("System is NOT in safe state.\nRequest DENIED.\n");
            return 0;
        }
    }

    // If safe
    printf("System is in SAFE state.\nRequest GRANTED.\nSafe sequence: ");
    for (int i = 0; i < n; i++)
        printf("P%d ", safeSequence[i]);

    printf("\n");

    return 0;
}