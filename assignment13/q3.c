#include <stdio.h>
#include <stdlib.h>

#define MAX_PROCESSES 10

// Structure for a segment
struct Segment {
    int base;
    int limit;
    int allocated; // 0 = free, 1 = allocated
};

// Structure for a process
struct Process {
    int pid;
    struct Segment code;
    struct Segment data;
    struct Segment stack;
};

// Simulated memory pointer
int memory_ptr = 1000;

// Allocate segment
void allocate_segment(struct Segment *seg, int size) {
    seg->base = memory_ptr;
    seg->limit = size;
    seg->allocated = 1;
    memory_ptr += size;
}

// Deallocate segment
void deallocate_segment(struct Segment *seg) {
    seg->allocated = 0;
    seg->base = -1;
    seg->limit = 0;
}

// Display process info
void display_process(struct Process p) {
    printf("\nProcess ID: %d\n", p.pid);

    printf("Code Segment -> Base: %d Limit: %d\n", p.code.base, p.code.limit);
    printf("Data Segment -> Base: %d Limit: %d\n", p.data.base, p.data.limit);
    printf("Stack Segment -> Base: %d Limit: %d\n", p.stack.base, p.stack.limit);
}

int main() {
    struct Process processes[MAX_PROCESSES];
    int count = 0;
    int choice;

    do {
        printf("\n--- Segmentation Memory Management ---\n");
        printf("1. Create Process\n");
        printf("2. Delete Process\n");
        printf("3. Display Processes\n");
        printf("4. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        if (choice == 1) {
            if (count >= MAX_PROCESSES) {
                printf("Process limit reached!\n");
                continue;
            }

            int code_size, data_size, stack_size;

            printf("\nEnter Process ID: ");
            scanf("%d", &processes[count].pid);

            printf("Enter Code Segment Size: ");
            scanf("%d", &code_size);

            printf("Enter Data Segment Size: ");
            scanf("%d", &data_size);

            printf("Enter Stack Segment Size: ");
            scanf("%d", &stack_size);

            allocate_segment(&processes[count].code, code_size);
            allocate_segment(&processes[count].data, data_size);
            allocate_segment(&processes[count].stack, stack_size);

            printf("✅ Process created successfully!\n");
            count++;
        }

        else if (choice == 2) {
            int pid, found = 0;
            printf("Enter Process ID to delete: ");
            scanf("%d", &pid);

            for (int i = 0; i < count; i++) {
                if (processes[i].pid == pid) {
                    deallocate_segment(&processes[i].code);
                    deallocate_segment(&processes[i].data);
                    deallocate_segment(&processes[i].stack);

                    printf("🗑️ Process %d deleted.\n", pid);
                    found = 1;
                }
            }

            if (!found)
                printf("Process not found!\n");
        }

        else if (choice == 3) {
            for (int i = 0; i < count; i++) {
                display_process(processes[i]);
            }
        }

    } while (choice != 4);

    return 0;
}