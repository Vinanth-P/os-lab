#include <stdio.h>

#define MAX 10

int main() {
    int n, m;
    int allocation[MAX][MAX], max[MAX][MAX], need[MAX][MAX];
    int available[MAX];

    printf("Enter number of processes: ");
    scanf("%d", &n);

    printf("Enter number of resources: ");
    scanf("%d", &m);

    // Input Allocation Matrix
    printf("Enter Allocation Matrix:\n");
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            scanf("%d", &allocation[i][j]);

    // Input Max Matrix
    printf("Enter Max Matrix:\n");
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            scanf("%d", &max[i][j]);

    // Input Available Resources
    printf("Enter Available Resources:\n");
    for (int j = 0; j < m; j++)
        scanf("%d", &available[j]);

    // Compute Need Matrix
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            need[i][j] = max[i][j] - allocation[i][j];

    // 🔹 Display Allocation Matrix
    printf("\nALLOCATION MATRIX:\n");
    printf("Process\t");
    for (int j = 0; j < m; j++) printf("R%d\t", j);
    printf("\n");

    for (int i = 0; i < n; i++) {
        printf("P%d\t", i);
        for (int j = 0; j < m; j++) {
            printf("%d\t", allocation[i][j]);
        }
        printf("\n");
    }

    // 🔹 Display Max Matrix
    printf("\nMAX MATRIX:\n");
    printf("Process\t");
    for (int j = 0; j < m; j++) printf("R%d\t", j);
    printf("\n");

    for (int i = 0; i < n; i++) {
        printf("P%d\t", i);
        for (int j = 0; j < m; j++) {
            printf("%d\t", max[i][j]);
        }
        printf("\n");
    }

    // 🔹 Display Need Matrix
    printf("\nNEED MATRIX:\n");
    printf("Process\t");
    for (int j = 0; j < m; j++) printf("R%d\t", j);
    printf("\n");

    for (int i = 0; i < n; i++) {
        printf("P%d\t", i);
        for (int j = 0; j < m; j++) {
            printf("%d\t", need[i][j]);
        }
        printf("\n");
    }

    // 🔹 Display Available Vector
    printf("\nAVAILABLE RESOURCES:\n");
    for (int j = 0; j < m; j++) {
        printf("R%d = %d\t", j, available[j]);
    }
    printf("\n");

    return 0;
}