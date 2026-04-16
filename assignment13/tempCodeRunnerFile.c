#include <stdio.h>

#define MAX 10

// Segment table structure
struct Segment {
    int base;
    int limit;
};

int main() {
    struct Segment seg[MAX];
    int n, i;

    // Input number of segments
    printf("Enter number of segments: ");
    scanf("%d", &n);

    // Input segment table
    printf("\nEnter base and limit for each segment:\n");
    for (i = 0; i < n; i++) {
        printf("\nSegment %d\n", i);
        printf("Base: ");
        scanf("%d", &seg[i].base);
        printf("Limit: ");
        scanf("%d", &seg[i].limit);
    }

    int choice;

    // Repeat translation
    do {
        int seg_no, offset;

        printf("\n--- Address Translation ---\n");
        printf("Enter segment number: ");
        scanf("%d", &seg_no);

        printf("Enter offset: ");
        scanf("%d", &offset);

        // Validation
        if (seg_no < 0 || seg_no >= n) {
            printf("❌ Invalid segment number!\n");
        }
        else if (offset >= seg[seg_no].limit) {
            printf("❌ Segmentation Fault! Offset exceeds limit.\n");
        }
        else {
            int physical = seg[seg_no].base + offset;
            printf("✅ Physical Address = %d\n", physical);
        }

        printf("\nDo you want to translate another address? (1 = Yes, 0 = No): ");
        scanf("%d", &choice);

    } while (choice == 1);

    printf("\nProgram terminated.\n");
    return 0;
}
// Enter number of segments: 2

// Segment 0 → Base: 1000, Limit: 400
// Segment 1 → Base: 2000, Limit: 300

// Enter segment number: 1
// Enter offset: 120

// ✅ Physical Address = 2120