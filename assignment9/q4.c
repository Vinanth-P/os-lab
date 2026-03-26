#include <stdio.h>

#define MAX 10

int n, m;
int allocation[MAX][MAX], max[MAX][MAX], need[MAX][MAX];
int available[MAX];

// 🔹 Function to calculate Need matrix
void calculateNeed() {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            need[i][j] = max[i][j] - allocation[i][j];
}

// 🔹 Function to display Need matrix
void displayNeed() {
    printf("\nNEED MATRIX:\n");
    printf("Process\t");
    for (int j = 0; j < m; j++) printf("R%d\t", j);
    printf("\n");

    for (int i = 0; i < n; i++) {
        printf("P%d\t", i);
        for (int j = 0; j < m; j++)
            printf("%d\t", need[i][j]);
        printf("\n");
    }
}

// 🔹 Safety Algorithm
void checkSafeState() {
    int work[MAX], finish[MAX] = {0}, safeSeq[MAX];

    for (int j = 0; j < m; j++)
        work[j] = available[j];

    int count = 0;

    while (count < n) {
        int found = 0;

        for (int i = 0; i < n; i++) {
            if (!finish[i]) {
                int j;
                for (j = 0; j < m; j++) {
                    if (need[i][j] > work[j])
                        break;
                }

                if (j == m) {
                    for (int k = 0; k < m; k++)
                        work[k] += allocation[i][k];

                    safeSeq[count++] = i;
                    finish[i] = 1;
                    found = 1;
                }
            }
        }

        if (!found) {
            printf("\nSystem is NOT in a safe state.\n");
            return;
        }
    }

    printf("\nSystem is in SAFE state.\nSafe sequence: ");
    for (int i = 0; i < n; i++)
        printf("P%d ", safeSeq[i]);
    printf("\n");
}

// 🔹 Resource Request
void requestResources() {
    int process, request[MAX];

    printf("Enter process number: ");
    scanf("%d", &process);

    printf("Enter request vector:\n");
    for (int j = 0; j < m; j++)
        scanf("%d", &request[j]);

    // Check Request <= Need
    for (int j = 0; j < m; j++) {
        if (request[j] > need[process][j]) {
            printf("Error: Request exceeds need.\n");
            return;
        }
    }

    // Check Request <= Available
    for (int j = 0; j < m; j++) {
        if (request[j] > available[j]) {
            printf("Resources not available.\n");
            return;
        }
    }

    // Temporary allocation
    for (int j = 0; j < m; j++) {
        available[j] -= request[j];
        allocation[process][j] += request[j];
        need[process][j] -= request[j];
    }

    printf("Request processed. Checking safe state...\n");
    checkSafeState();
}

// 🔹 Input Data
void enterData() {
    printf("Enter number of processes: ");
    scanf("%d", &n);

    printf("Enter number of resources: ");
    scanf("%d", &m);

    printf("Enter Allocation Matrix:\n");
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            scanf("%d", &allocation[i][j]);

    printf("Enter Max Matrix:\n");
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            scanf("%d", &max[i][j]);

    printf("Enter Available Resources:\n");
    for (int j = 0; j < m; j++)
        scanf("%d", &available[j]);

    calculateNeed();
}

// 🔹 Main Menu
int main() {
    int choice;

    do {
        printf("\n--- BANKER'S ALGORITHM MENU ---\n");
        printf("1. Enter Data\n");
        printf("2. Display Need Matrix\n");
        printf("3. Check Safe State\n");
        printf("4. Process Resource Request\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                enterData();
                break;
            case 2:
                displayNeed();
                break;
            case 3:
                checkSafeState();
                break;
            case 4:
                requestResources();
                break;
            case 5:
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice!\n");
        }

    } while (choice != 5);

    return 0;
}