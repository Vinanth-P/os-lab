/*
 * Q1: Implementation of Memory Allocation Algorithms
 * Algorithms: First Fit, Next Fit, Best Fit, Worst Fit, Quick Fit
 * Uses a linked list to represent memory blocks
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEMORY_SIZE     1024   /* Total memory in KB */
#define QUICK_FIT_CLASSES 4   /* Number of size classes for Quick Fit */

/* Size class boundaries (KB) for Quick Fit */
int quick_fit_sizes[QUICK_FIT_CLASSES] = {32, 64, 128, 256};

/* ─────────────────────────────────────────────
 * Data Structures
 * ───────────────────────────────────────────── */

typedef struct MemBlock {
    int   block_id;          /* unique block identifier             */
    int   process_id;        /* -1 = free                           */
    int   start;             /* start address (KB)                  */
    int   size;              /* total size of block (KB)            */
    int   allocated_size;    /* size given to process (KB)          */
    struct MemBlock *next;
    struct MemBlock *prev;
} MemBlock;

typedef struct {
    MemBlock *head;          /* head of the main memory list        */
    MemBlock *last_allocated;/* for Next Fit: last allocated block  */
    int       total_size;
    int       next_block_id;
} MemoryManager;

/* Quick Fit free lists: one per size class */
typedef struct QuickNode {
    MemBlock      *block;
    struct QuickNode *next;
} QuickNode;

QuickNode *quick_lists[QUICK_FIT_CLASSES]; /* free lists            */

/* ─────────────────────────────────────────────
 * Helper Utilities
 * ───────────────────────────────────────────── */

static int next_block_id_counter = 1;

MemBlock *create_block(int start, int size) {
    MemBlock *b = (MemBlock *)malloc(sizeof(MemBlock));
    if (!b) { fprintf(stderr, "Out of memory!\n"); exit(1); }
    b->block_id       = next_block_id_counter++;
    b->process_id     = -1;
    b->start          = start;
    b->size           = size;
    b->allocated_size = 0;
    b->next           = NULL;
    b->prev           = NULL;
    return b;
}

/* ─────────────────────────────────────────────
 * Core Memory Manager Functions
 * ───────────────────────────────────────────── */

/* Initialize memory as one large free block */
void initialize_memory(MemoryManager *mm) {
    mm->head           = create_block(0, MEMORY_SIZE);
    mm->last_allocated = NULL;
    mm->total_size     = MEMORY_SIZE;
    mm->next_block_id  = 1;
    for (int i = 0; i < QUICK_FIT_CLASSES; i++)
        quick_lists[i] = NULL;
    printf("[INIT] Memory initialized: %d KB total, 1 free block.\n\n",
           MEMORY_SIZE);
}

/* Split a free block: allocate `size` KB, leave remainder free */
MemBlock *split_and_allocate(MemoryManager *mm, MemBlock *blk,
                              int size, int pid) {
    if (blk->size > size + 1) {          /* worth splitting         */
        MemBlock *remainder = create_block(blk->start + size,
                                           blk->size - size);
        remainder->next = blk->next;
        remainder->prev = blk;
        if (blk->next) blk->next->prev = remainder;
        blk->next = remainder;
        blk->size = size;
    }
    blk->process_id     = pid;
    blk->allocated_size = size;
    mm->last_allocated  = blk;
    return blk;
}

/* ── 1. First Fit ─────────────────────────── */
MemBlock *first_fit(MemoryManager *mm, int size, int pid) {
    MemBlock *cur = mm->head;
    while (cur) {
        if (cur->process_id == -1 && cur->size >= size)
            return split_and_allocate(mm, cur, size, pid);
        cur = cur->next;
    }
    return NULL;   /* allocation failed */
}

/* ── 2. Next Fit ──────────────────────────── */
MemBlock *next_fit(MemoryManager *mm, int size, int pid) {
    MemBlock *start_blk = mm->last_allocated
                          ? mm->last_allocated->next
                          : mm->head;
    if (!start_blk) start_blk = mm->head;

    /* Search from last allocated to end */
    MemBlock *cur = start_blk;
    while (cur) {
        if (cur->process_id == -1 && cur->size >= size)
            return split_and_allocate(mm, cur, size, pid);
        cur = cur->next;
    }
    /* Wrap around from beginning */
    cur = mm->head;
    while (cur && cur != start_blk) {
        if (cur->process_id == -1 && cur->size >= size)
            return split_and_allocate(mm, cur, size, pid);
        cur = cur->next;
    }
    return NULL;
}

/* ── 3. Best Fit ──────────────────────────── */
MemBlock *best_fit(MemoryManager *mm, int size, int pid) {
    MemBlock *best = NULL;
    MemBlock *cur  = mm->head;
    while (cur) {
        if (cur->process_id == -1 && cur->size >= size)
            if (!best || cur->size < best->size)
                best = cur;
        cur = cur->next;
    }
    return best ? split_and_allocate(mm, best, size, pid) : NULL;
}

/* ── 4. Worst Fit ─────────────────────────── */
MemBlock *worst_fit(MemoryManager *mm, int size, int pid) {
    MemBlock *worst = NULL;
    MemBlock *cur   = mm->head;
    while (cur) {
        if (cur->process_id == -1 && cur->size >= size)
            if (!worst || cur->size > worst->size)
                worst = cur;
        cur = cur->next;
    }
    return worst ? split_and_allocate(mm, worst, size, pid) : NULL;
}

/* ── 5. Quick Fit ─────────────────────────── */

/* Add a free block to the appropriate quick-fit list */
void quick_fit_add(MemBlock *blk) {
    for (int i = 0; i < QUICK_FIT_CLASSES; i++) {
        if (blk->size <= quick_fit_sizes[i]) {
            QuickNode *qn = (QuickNode *)malloc(sizeof(QuickNode));
            qn->block = blk;
            qn->next  = quick_lists[i];
            quick_lists[i] = qn;
            return;
        }
    }
    /* Larger than all classes – goes in last list */
    QuickNode *qn = (QuickNode *)malloc(sizeof(QuickNode));
    qn->block = blk;
    qn->next  = quick_lists[QUICK_FIT_CLASSES - 1];
    quick_lists[QUICK_FIT_CLASSES - 1] = qn;
}

/* Remove a specific block from quick-fit lists */
void quick_fit_remove(MemBlock *blk) {
    for (int i = 0; i < QUICK_FIT_CLASSES; i++) {
        QuickNode **cur = &quick_lists[i];
        while (*cur) {
            if ((*cur)->block == blk) {
                QuickNode *tmp = *cur;
                *cur = tmp->next;
                free(tmp);
                return;
            }
            cur = &(*cur)->next;
        }
    }
}

MemBlock *quick_fit(MemoryManager *mm, int size, int pid) {
    /* Find the smallest class that fits */
    for (int i = 0; i < QUICK_FIT_CLASSES; i++) {
        if (size <= quick_fit_sizes[i]) {
            QuickNode **cur = &quick_lists[i];
            while (*cur) {
                MemBlock *blk = (*cur)->block;
                if (blk->process_id == -1 && blk->size >= size) {
                    /* Remove from quick list */
                    QuickNode *tmp = *cur;
                    *cur = tmp->next;
                    free(tmp);
                    return split_and_allocate(mm, blk, size, pid);
                }
                cur = &(*cur)->next;
            }
        }
    }
    /* Fall back to first fit */
    return first_fit(mm, size, pid);
}

/* ─────────────────────────────────────────────
 * Allocate / Free / Coalesce / Display
 * ───────────────────────────────────────────── */

MemBlock *allocate_memory(MemoryManager *mm, int strategy,
                           int size, int pid) {
    MemBlock *result = NULL;
    switch (strategy) {
        case 1: result = first_fit (mm, size, pid); break;
        case 2: result = next_fit  (mm, size, pid); break;
        case 3: result = best_fit  (mm, size, pid); break;
        case 4: result = worst_fit (mm, size, pid); break;
        case 5: result = quick_fit (mm, size, pid); break;
        default: printf("Unknown strategy!\n"); return NULL;
    }
    if (result)
        printf("  [ALLOC] PID %d | %d KB | Block %d @ %d KB\n",
               pid, size, result->block_id, result->start);
    else
        printf("  [FAIL]  PID %d | %d KB | No suitable block found.\n",
               pid, size);
    return result;
}

/* Coalesce adjacent free blocks */
void coalesce_free_blocks(MemoryManager *mm) {
    MemBlock *cur = mm->head;
    int merged = 0;
    while (cur && cur->next) {
        if (cur->process_id == -1 && cur->next->process_id == -1) {
            MemBlock *nxt = cur->next;
            cur->size += nxt->size;
            cur->next  = nxt->next;
            if (nxt->next) nxt->next->prev = cur;
            free(nxt);
            merged++;
        } else {
            cur = cur->next;
        }
    }
    if (merged)
        printf("  [COALESCE] Merged %d adjacent free block(s).\n", merged);
}

void free_memory(MemoryManager *mm, int pid) {
    MemBlock *cur = mm->head;
    int found = 0;
    while (cur) {
        if (cur->process_id == pid) {
            printf("  [FREE] PID %d | Block %d | %d KB freed.\n",
                   pid, cur->block_id, cur->allocated_size);
            cur->process_id     = -1;
            cur->allocated_size = 0;
            quick_fit_add(cur);   /* return to quick-fit pool        */
            found = 1;
        }
        cur = cur->next;
    }
    if (!found)
        printf("  [FREE] PID %d not found in memory.\n", pid);
    coalesce_free_blocks(mm);
}

void display_memory(MemoryManager *mm) {
    printf("\n  %-8s %-8s %-10s %-10s %-10s\n",
           "Block", "PID", "Start(KB)", "Size(KB)", "Status");
    printf("  %s\n", "---------------------------------------------------");
    MemBlock *cur = mm->head;
    int used = 0, free_mem = 0, frag = 0;
    int free_blocks = 0;
    while (cur) {
        printf("  %-8d %-8d %-10d %-10d %-10s\n",
               cur->block_id,
               cur->process_id,
               cur->start,
               cur->size,
               cur->process_id == -1 ? "FREE" : "USED");
        if (cur->process_id == -1) {
            free_mem += cur->size;
            free_blocks++;
        } else {
            used += cur->allocated_size;
            frag += cur->size - cur->allocated_size; /* internal frag */
        }
        cur = cur->next;
    }
    printf("  %s\n", "---------------------------------------------------");
    printf("  Used: %d KB | Free: %d KB | Internal Frag: %d KB | "
           "Free Blocks: %d\n\n",
           used, free_mem, frag, free_blocks);
}

/* ─────────────────────────────────────────────
 * Main – Demonstration of all algorithms
 * ───────────────────────────────────────────── */
int main(void) {
    printf("=======================================================\n");
    printf("   Q1: Memory Allocation Algorithm Implementations     \n");
    printf("=======================================================\n\n");

    const char *names[] = {"", "First Fit", "Next Fit",
                           "Best Fit", "Worst Fit", "Quick Fit"};

    for (int strategy = 1; strategy <= 5; strategy++) {
        printf("-------------------------------------------------------\n");
        printf("  Strategy: %s\n", names[strategy]);
        printf("-------------------------------------------------------\n");

        MemoryManager mm;
        initialize_memory(&mm);

        /* Demonstration: allocate 5 blocks then free one */
        int sizes[] = {64, 128, 32, 256, 16};
        int pids[]  = {101, 102, 103, 104, 105};
        for (int i = 0; i < 5; i++)
            allocate_memory(&mm, strategy, sizes[i], pids[i]);

        display_memory(&mm);

        free_memory(&mm, 102);
        allocate_memory(&mm, strategy, 80, 106);
        display_memory(&mm);

        /* Clean up linked list */
        MemBlock *cur = mm.head;
        while (cur) {
            MemBlock *tmp = cur;
            cur = cur->next;
            free(tmp);
        }
        /* Clean up quick-fit lists */
        for (int i = 0; i < QUICK_FIT_CLASSES; i++) {
            QuickNode *qn = quick_lists[i];
            while (qn) {
                QuickNode *tmp = qn;
                qn = qn->next;
                free(tmp);
            }
            quick_lists[i] = NULL;
        }
    }
    return 0;
}